#include "base/framework/message_pump_default.h"

namespace base {
	DefaultMessagePump::DefaultMessagePump() : keep_running_(true), event_(false, false) {
	}

	void DefaultMessagePump::Quit() {
		keep_running_ = false;
	}

	void DefaultMessagePump::Run(Delegate* delegate) {
		assert(keep_running_ && delegate);
		for(; ;) {
			bool more_work_is_plausible = delegate->DoWork();
			if (!keep_running_) {
				break;
			}
			more_work_is_plausible |= delegate->DoDelayWork(&delayed_work_time_);
			if (!keep_running_) {
				break;
			}
			if (more_work_is_plausible) {
				continue;
			}
			more_work_is_plausible |= delegate->DoIdleWork();
			if (!keep_running_) {
				break;
			}
			if (more_work_is_plausible) {
				continue;
			}
			if (delayed_work_time_.IsNull()) {
				event_.Wait();
			}else {
				TimeSpan span = delayed_work_time_ - TimeTicks::Now();
				if (span > TimeSpan()) {
					event_.WaitForTime(span.ToMilliseconds());
				}else {
					delayed_work_time_ = TimeTicks();
				}
			}
		}
		keep_running_ = true;
	}

	void DefaultMessagePump::ScheduleDelayWork(const TimeTicks &next_time) {
		delayed_work_time_ = next_time;
	}

	void DefaultMessagePump::ScheduleWork() {
		event_.Signal();
	}
}