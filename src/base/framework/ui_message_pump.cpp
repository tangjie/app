/*
 * This file use to implement UI message pump,Modify from http://src.chromium.org/svn/trunk/src/base/message_pump_win.cc
 */

#include <assert.h>
#include <math.h>
#include "base/framework/ui_message_pump.h"
namespace base {
	static const wchar_t kWndClass[] = L"App_MessagePumpWindow";
	static const int kMsgHaveWork = WM_USER + 1;
	static const int kMessageFilterCode = 0x5001;

	void UIMessagePump::AddUIObserver(UIObserver *observer) {
		ui_observers_->AddObserver(observer);
	}

	void UIMessagePump::RemoveUIObserver(UIObserver *observer) {
		ui_observers_->RemoveObserver(observer);
	}

	void UIMessagePump::ScheduleWork() {
		if (InterlockedExchange(&have_work_, 1)) {
			return;
		}
		PostMessage(hwnd_, kMsgHaveWork, reinterpret_cast<UINT_PTR>(this), 0);
	}

	void UIMessagePump::ScheduleDelayWork(const TimeTicks &delayed_work_time) {
		delayed_work_time_ = delayed_work_time;
		int delay = GetCurrentDelay();
		assert(delay > 0);
		if (delay < USER_TIMER_MINIMUM) {
			delay = USER_TIMER_MINIMUM;
		}

		SetTimer(hwnd_, reinterpret_cast<UINT_PTR>(this), delay, nullptr);
	}

	void UIMessagePump::PreProcessMessage(const MSG& msg) {
		FOR_EACH_OBSERVER(UIObserver, ui_observers_, PreProcessMessage(msg));
	}

	void UIMessagePump::PostProcessMessage(const MSG& msg) {
		FOR_EACH_OBSERVER(UIObserver, ui_observers_, PostProcessMessage(msg));
	}

	void UIMessagePump::DoRunLoop() {
		// IF this was just a simple PeekMessage() loop (servicing all possible work
		// queues), then Windows would try to achieve the following order according
		// to MSDN documentation about PeekMessage with no filter):
		//    * Sent messages
		//    * Posted messages
		//    * Sent messages (again)
		//    * WM_PAINT messages
		//    * WM_TIMER messages
		//
		// Summary: none of the above classes is starved, and sent messages has twice
		// the chance of being processed (i.e., reduced service time).
		for(; ;) {
			// If we do any work, we may create more messages etc., and more work may
			// possibly be waiting in another task group.  When we (for example)
			// ProcessNextWindowsMessage(), there is a good chance there are still more
			// messages waiting.  On the other hand, when any of these methods return
			// having done no work, then it is pretty unlikely that calling them again
			// quickly will find any work to do.  Finally, if they all say they had no
			// work, then it is a good time to consider sleeping (waiting) for more
			// work.
			bool more_work_is_plausible = ProcessNextWindowsMessage();
			if (state_->should_quit_) {
				break;
			}
			more_work_is_plausible |= state_->delegate_->DoWork();
			if (state_->should_quit_) {
				break;
			}
			more_work_is_plausible |= state_->delegate_->DoDelayWork(&delayed_work_time_);
			if (more_work_is_plausible && delayed_work_time_.IsNull()) {
				KillTimer(hwnd_, reinterpret_cast<UINT_PTR>(this));
			}
			if (state_->should_quit_) {
				break;
			}
			if (more_work_is_plausible) {
				continue;
			}
			more_work_is_plausible = state_->delegate_->DoIdleWork();
			if (state_->should_quit_) {
				break;
			}
			if (more_work_is_plausible) {
				continue;
			}
			WaitForWork();
		}
	}

	bool UIMessagePump::ProcessNextWindowsMessage() {
		// If there are sent messages in the queue then PeekMessage internally
		// dispatches the message and returns false. We return true in this
		// case to ensure that the message loop peeks again instead of calling
		// MsgWaitForMultipleObjectsEx again.
		 bool sent_messages_in_queue = false;
		 DWORD queue_state = GetQueueStatus(QS_SENDMESSAGE);
		 if (HIWORD(queue_state) & QS_SENDMESSAGE) {
			 sent_messages_in_queue = true;
		 }
		 MSG msg;
		 if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			 return ProcessMessageHelper(msg);
		 }
		 return sent_messages_in_queue;
	}

	bool UIMessagePump::ProcessMessageHelper(const MSG& msg) {
		if (WM_QUIT == msg.message) {
			state_->should_quit_ = true;
			PostQuitMessage(implicit_cast<int>(msg.wParam));
			return false;
		}
		// While running our main message pump, we discard kMsgHaveWork messages.
		if (kMsgHaveWork == msg.message && msg.hwnd == hwnd_) {
			return ProcessPumpReplacementMessage();
		}
		if (CallMsgFilter(const_cast<MSG*>(&msg), kMessageFilterCode)) {
			return true;
		}
		PreProcessMessage(msg);
		if (state_->dispatcher_ != nullptr) {
			state_->dispatcher_->Dispatch(msg);
		}else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		PostProcessMessage(msg);
		return true;
	}

	bool UIMessagePump::ProcessPumpReplacementMessage() {
		MSG msg;
		bool have_work = (0 != PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE));
		assert(!have_work || msg.message != kMsgHaveWork || msg.hwnd != hwnd_);
		//TODO: Add log info. 16/11/2011 by tangjie.
		int old_have_work = InterlockedExchange(&have_work_, 0);
		assert(old_have_work);
		if (!have_work) {
			return false;
		}
		ScheduleWork();
		return ProcessMessageHelper(msg);
	}

	void UIMessagePump::RunWithDispatcher(Delegate *delegate, Dispatcher *dispatcher) {
		assert(delegate);
		if (delegate == nullptr) {
			return;
		}
		RunState state;
		state.delegate_ = delegate;
		state.dispatcher_ = dispatcher;
		state.should_quit_ = false;
		state.run_depth_ = state_ == nullptr ? 1 : state_->run_depth_ + 1;
		RunState* previous_state = state_;
		state_ = &state;
		DoRunLoop();
		state_ = previous_state;
	}

	void UIMessagePump::Run(Delegate *delegate) {
		RunWithDispatcher(delegate,nullptr);
	}

	void UIMessagePump::WaitForWork() {
		int delay = GetCurrentDelay();
		if (delay < 0) {
			delay = INFINITE;
		}
		DWORD result = MsgWaitForMultipleObjectsEx(0, nullptr, delay, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
		if (result == WAIT_OBJECT_0) {
			MSG msg = { 0 };
			DWORD queue_state = GetQueueStatus(QS_MOUSE);
			if (HIWORD(queue_state) & QS_MOUSE && !PeekMessage(&msg, nullptr, 
				WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE)) {
					WaitMessage();
			}
			return;
		}
	}

	void UIMessagePump::Quit() {
		assert(state_);
		if (state_ != nullptr) {
			state_->should_quit_ = true;
		}
	}

	int UIMessagePump::GetCurrentDelay() const {
		if (delayed_work_time_.IsNull()) {
			return -1;
		}
		double time_out = ceil((delayed_work_time_ - TimeTicks::Now()).ToInternalValue() / 1000.0);
		int delay = static_cast<int>(time_out);
		delay = min(delay, 0);
		return delay;
	}

	void UIMessagePump::InitMessageWnd() {
		HINSTANCE instance = GetModuleHandle(nullptr);
		WNDCLASSEX wnd_class = { 0 };
		wnd_class.cbSize = sizeof(WNDCLASSEX);
		wnd_class.hInstance = instance;
		wnd_class.lpfnWndProc = WindowProc;
		wnd_class.lpszClassName = kWndClass;
		RegisterClassEx(&wnd_class);
		hwnd_ = CreateWindow(kWndClass, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, instance, 0);
		assert(hwnd_ != nullptr);
		//TODO: Add log info. 16/11/2011 by tangjie.
	}

	void UIMessagePump::HandleWorkMessage() {
		if (state_ == nullptr) {
			InterlockedExchange(&have_work_, 0);
			return;
		}
		ProcessPumpReplacementMessage();
		if (state_->delegate_->DoWork()) {
			ScheduleWork();
		}
	}

	void UIMessagePump::HandleTimerMessage() {
		KillTimer(hwnd_, reinterpret_cast<UINT_PTR>(this));
		if (state_ == nullptr) {
			return;
		}
		state_->delegate_->DoDelayWork(&delayed_work_time_);
		if (!delayed_work_time_.IsNull()) {
			ScheduleDelayWork(delayed_work_time_);
		}
	}

	LRESULT UIMessagePump::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		switch(message) {
		case kMsgHaveWork:
			reinterpret_cast<UIMessagePump*>(wparam)->HandleWorkMessage();
			break;
		case WM_TIMER:
			reinterpret_cast<UIMessagePump*>(wparam)->HandleTimerMessage();
			break;
		}
		return DefWindowProc(hwnd, message, wparam, lparam);
	}

	UIMessagePump::~UIMessagePump() {
		DestroyWindow(hwnd_);
		UnregisterClass(kWndClass, GetModuleHandle(nullptr));
	}
}