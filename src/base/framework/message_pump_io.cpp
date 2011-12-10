#include "base/framework/message_pump_io.h"

namespace base {
	IOMessagePump::IOMessagePump() {
		io_observers_ = std::shared_ptr<ObserverList<IOObserver>>(new ObserverList<IOObserver>());
		port_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1);
	}

	void IOMessagePump::AddIOObserver(IOObserver *observer) {
		io_observers_->AddObserver(observer);
	}

	void IOMessagePump::RemoveIOObserver(IOObserver *observer) {
		io_observers_->RemoveObserver(observer);
	}

	void IOMessagePump::DoRunLoop() {
		for (; ;) {
			bool more_work_is_plausible = state_->delegate_->DoWork();
			if (state_->should_quit_) {
				break;
			}

			more_work_is_plausible |= WaitForIOCompletion(0, nullptr);
			if (state_->should_quit_) {
				break;
			}

			more_work_is_plausible |= state_->delegate_->DoDelayWork(&delayed_work_time_);
			if (state_->should_quit_) {
				break;
			}
			if (more_work_is_plausible) {
				continue;
			}

			more_work_is_plausible |= state_->delegate_->DoIdleWork();
			if (state_->should_quit_) {
				break;
			}
			if (more_work_is_plausible) {
				continue;
			}
			// sleep some time!
			WaitForWork();
		}
	}

	void IOMessagePump::WaitForWork() {
		assert(1 == state_->run_depth_);
		int time_out = GetCurrentDelay();
		if (time_out < 0) {
			time_out = INFINITE;
		}
		WaitForIOCompletion(time_out, nullptr);
	}

	bool IOMessagePump::WaitForIOCompletion(DWORD time_out, IOHandler *filter) {
		IOItem item;
		// if the there is no item matched the filter in completed list, we just wait for it.
		if (completed_io_.empty() || !MatchCompleteIOItem(filter, &item)) {
			if (!GetIOItem(time_out, &item)) {
				return false;
			}
			if (ProcessInternalIOItem(item)) {
				return true;
			}
		}
		if (item.context_->handler_ != nullptr) {
			// if the item is not we expected, push it in the completed list.
			if (filter != nullptr && item.handler_ != filter) {
				completed_io_.push_back(item);
			}else {
				assert(item.context_->handler_ == item.handler_);
				PreProcessIOEvent();
				item.handler_->OnIOCompleted(item.context_, item.bytes_transfered_, item.error_);
				PostProcessIOEvent();
			}
		}else {
			delete item.context_;
		}
		return true;
	}

	bool IOMessagePump::GetIOItem(DWORD time_out, IOItem *item) {
		memset(item, 0, sizeof(*item));
		ULONG_PTR key = NULL;
		OVERLAPPED *overlapped = nullptr;
		if (!GetQueuedCompletionStatus(port_, &item->bytes_transfered_, &key, &overlapped, time_out)) {
			if (overlapped == nullptr) {
				return false;
			}
			item->error_ = GetLastError();
			item->bytes_transfered_ = 0;
		}
		item->context_ = reinterpret_cast<IOContext*>(overlapped);
		item->handler_ = reinterpret_cast<IOHandler*>(key);
		return true;
	}

	bool IOMessagePump::ProcessInternalIOItem(const IOItem &item) {
		if (this == reinterpret_cast<IOMessagePump*>(item.context_) && this == reinterpret_cast<IOMessagePump*>(item.handler_)) {
			assert(item.bytes_transfered_ == 0);
			InterlockedExchange(&have_work_, 0);
			return true;
		}
		return false;
	}

	bool IOMessagePump::MatchCompleteIOItem(IOHandler *filter, IOItem *item) {
		assert(!completed_io_.empty());
		for (std::list<IOItem>::iterator iter = completed_io_.begin(); iter != completed_io_.end(); ++iter) {
			if (filter == nullptr || iter->handler_ == filter) {
				*item = *iter;
				completed_io_.erase(iter);
				return true;
			}
		}
		return false;
	}

	void IOMessagePump::PreProcessIOEvent() {
		FOR_EACH_OBSERVER(IOObserver, io_observers_, PreProcessIOEvent());
	}

	void IOMessagePump::PostProcessIOEvent() {
		FOR_EACH_OBSERVER(IOObserver, io_observers_, PostProcessIOEvent());
	}

	void IOMessagePump::RegisterIOHandler(HANDLE handle, IOHandler *filter) {
		HANDLE port = CreateIoCompletionPort(handle, port_, reinterpret_cast<ULONG_PTR>(filter), 1);
		assert(port);
	}

	void IOMessagePump::ScheduleWork() {
		if (InterlockedExchange(&have_work_, 1)) {
			return;
		}
		BOOL ret = PostQueuedCompletionStatus(port_, 0, reinterpret_cast<ULONG_PTR>(this), reinterpret_cast<LPOVERLAPPED>(this));
		assert(ret);
	}

	void IOMessagePump::ScheduleDelayWork(const TimeTicks &delayed_work_time) {
		// this function can only be call on the same thread which called run.
		delayed_work_time_ = delayed_work_time;
	}

}