#include "base/framework/message_pump.h"

namespace base {
	MessagePump::MessagePump() : have_work_(0), state_(nullptr) {
	}

	MessagePump::~MessagePump() {
	}

	int MessagePump::GetCurrentDelay() const {
		if (delayed_work_time_.IsNull()) {
			return -1;
		}
		double time_out = ceil((delayed_work_time_ - TimeTicks::Now()).ToInternalValue() / 1000.0);
		int delay = static_cast<int>(time_out);
		delay = max(delay, 0);
		return delay;
	}

	void MessagePump::Quit() {
		assert(state_);
		if (state_ != nullptr) {
			state_->should_quit_ = true;
		}
	}

	void MessagePump::Run(Delegate *delegate) {
		RunWithDispatcher(delegate,nullptr);
	}

	void MessagePump::RunWithDispatcher(Delegate *delegate, Dispatcher *dispatcher) {
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

}