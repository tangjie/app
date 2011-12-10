/*
 * Default message pump is a simply imply of message pump with waitable event.
 */

#ifndef BASE_FRAMEWORK_MESSAGE_PUMP_DEFAULT_H__
#define BASE_FRAMEWORK_MESSAGE_PUMP_DEFAULT_H__

#include "base/framework/message_pump.h"
#include "base/synchronization/waitable_event.h"
#include "base/time/time.h"
namespace base {
	class DefaultMessagePump : public MessagePump {
	public:
		DefaultMessagePump();
		virtual ~DefaultMessagePump(){
		}
		virtual void DoRunLoop() {
			// we override run not use DoRunLoop;
		}
		virtual void Run(Delegate* delegate);
		virtual void Quit();
		virtual void ScheduleWork();
		virtual void ScheduleDelayWork(const TimeTicks &next_time);
	private:
		bool keep_running_;
		TimeTicks delayed_work_time_;
		WaitableEvent event_;
	};
}
#endif// BASE_FRAMEWORK_MESSAGE_PUMP_DEFAULT_H__