/*
 * This file use to implement UI message pump,Modify from http://src.chromium.org/svn/trunk/src/base/message_pump_win.h
 */

#ifndef BASE_FRAMEWORK_UI_MESSAGE_PUMP_H__
#define BASE_FRAMEWORK_UI_MESSAGE_PUMP_H__


#include <Windows.h>
#include "base/framework/message_pump.h"
#include "base/framework/observer_list.h"
#include "base/time/time.h"

namespace base {
  class UIMessagePump : public MessagePump {
  public:
    class Dispatcher {
    public:
      virtual ~Dispatcher() {
      }

      virtual bool Dispatch(const MSG &msg) = 0;
    };

    class UIObserver {
    public:
      virtual void PreProcessMessage(const MSG &msg) = 0;
      virtual void PostProcessMessage(const MSG &msg) = 0;
    };
	void PreProcessMessage(const MSG& msg);
	void PostProcessMessage(const MSG& msg);
    void AddUIObserver(UIObserver *observer);
    void RemoveUIObserver(UIObserver *observer);
	// Start UI message punp.
    void RunWithDispatcher(Delegate *delegate, Dispatcher *dispatcher);
    virtual void Run(Delegate *delegate);
    virtual void Quit();
    virtual void ScheduleWork();
    virtual void ScheduleDelayWork(const TimeTicks &delayed_work_time);
    UIMessagePump() : have_work_(0), state_(nullptr), hwnd_(nullptr) {
		ui_observers_ = std::shared_ptr<ObserverList<UIObserver>>(new ObserverList<UIObserver>());
		InitMessageWnd();
    }

    virtual ~UIMessagePump();

  private:
    struct RunState {
      int run_depth_;
      bool should_quit_;
      Delegate *delegate_;
      Dispatcher *dispatcher_;
    };

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
    int GetCurrentDelay() const;
    void DoRunLoop();
    void InitMessageWnd();
	 void WaitForWork();
    void HandleWorkMessage();
    void HandleTimerMessage();
	bool ProcessNextWindowsMessage();
	bool ProcessMessageHelper(const MSG& msg);
	bool ProcessPumpReplacementMessage();

    RunState* state_;
    long have_work_;
    TimeTicks delayed_work_time_;
    HWND hwnd_;
    std::shared_ptr<ObserverList<UIObserver>> ui_observers_;
  };
}

#endif// BASE_FRAMEWORK_UI_MESSAGE_PUMP_H__