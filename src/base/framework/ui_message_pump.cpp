/*
 * This file use to implement UI message pump
 */

#include <assert.h>
#include <math.h>
#include "base/framework/ui_message_pump.h"
namespace base {
	void UIMessagePump:::AddUIObserver(UIObserver *observer) {
		ui_observers_.AddObserver(observer);
	}

	void UIMessagePump::RemoveUIObserver(UIObserver *observer) {
		ui_observers_.RemoveObserver(observer);
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
		double time_out = ceil((delayed_work_time_ - TimeTicks::Now()).ToInternalValue()/1000);
		int delay = implicit_cast<int>(time_out);
		delay = min(delay, 0);
		return delay;
	}
}