/*
 * The base class of message pump implemention.
 */

#ifndef BASE_FRAMEWORK_MESSAGE_PUMP_H__
#define BASE_FRAMEWORK_MESSAGE_PUMP_H__

namespace base {
	class TimeTicks;

	class MessagePump {
	public:
		class Delegate {
		public:
			virtual ~Delegate() {
			}

			virtual bool DoWork() = 0;
			virtual bool DoDelayWork(TimeTicks* next_time) = 0;
			virtual bool DoIdleWork() = 0;
		};

		MessagePump();
		virtual ~MessagePump();
		virtual void Start(Delegate* delegate) = 0;
		virtual void Quit() = 0;
		virtual void DispatchWork() = 0;
		virtual void DispatchDelayWork(const TimeTicks &next_times) = 0;
	};
}

#endif// BASE_FRAMEWORK_MESSAGE_PUMP_H__