/*
 * The base class of message pump implemention.
 */

#ifndef BASE_FRAMEWORK_MESSAGE_PUMP_H__
#define BASE_FRAMEWORK_MESSAGE_PUMP_H__

#include "base/time/time.h"

namespace base {
	class TimeTicks;

	class MessagePump {
	public:
		class Dispatcher {
		public:
			virtual ~Dispatcher() {
			}

			virtual bool Dispatch(const MSG &msg) = 0;
		};

		class Delegate {
		public:
			virtual ~Delegate() {
			}

			virtual bool DoWork() = 0;
			virtual bool DoDelayWork(TimeTicks *next_time) = 0;
			virtual bool DoIdleWork() = 0;
		};

		MessagePump();
		virtual ~MessagePump();
		virtual void DoRunLoop() = 0;
		virtual void Run(Delegate* delegate);
		virtual void RunWithDispatcher(Delegate *delegate, Dispatcher * dispatcher);
		virtual void Quit();
		virtual void ScheduleWork() = 0;
		virtual void ScheduleDelayWork(const TimeTicks &delayed_work_time) = 0;
	protected:
		int GetCurrentDelay() const;
		struct RunState {
			int run_depth_;
			bool should_quit_;
			Delegate *delegate_;
			Dispatcher *dispatcher_;
		};
		RunState* state_;
		long have_work_;
		TimeTicks delayed_work_time_;
	};
}

#endif// BASE_FRAMEWORK_MESSAGE_PUMP_H__