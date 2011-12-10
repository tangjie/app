/*
 * This file use to implement IO message pump,Modify from http://src.chromium.org/svn/trunk/src/base/message_pump_win.h
 */

#ifndef BASE_FRAMEWORK_MESSAGE_PUMP_IO_H__
#define BASE_FRAMEWORK_MESSAGE_PUMP_IO_H__

#include <Windows.h>
#include <list>
#include "base/framework/message_pump.h"
#include "base/framework/observer_list.h"
#include "base/time/time.h"

namespace base {
	class IOMessagePump : public MessagePump {
	public:
		struct IOContext;

		// Clients interested in receiving OS notifications when asynchronous IO
		// operations complete should implement this interface and register themselves
		// with the message pump.
		//
		// Typical use #1:
		//   // Use only when there are no user's buffers involved on the actual IO,
		//   // so that all the cleanup can be done by the message pump.
		//   class MyFile : public IOHandler {
		//     MyFile() {
		//       ...
		//       context_ = new IOContext;
		//       context_->handler = this;
		//       message_pump->RegisterIOHandler(file_, this);
		//     }
		//     ~MyFile() {
		//       if (pending_) {
		//         // By setting the handler to NULL, we're asking for this context
		//         // to be deleted when received, without calling back to us.
		//         context_->handler = NULL;
		//       } else {
		//         delete context_;
		//      }
		//     }
		//     virtual void OnIOCompleted(IOContext* context, DWORD bytes_transfered,
		//                                DWORD error) {
		//         pending_ = false;
		//     }
		//     void DoSomeIo() {
		//       ...
		//       // The only buffer required for this operation is the overlapped
		//       // structure.
		//       ConnectNamedPipe(file_, &context_->overlapped);
		//       pending_ = true;
		//     }
		//     bool pending_;
		//     IOContext* context_;
		//     HANDLE file_;
		//   };
		//
		// Typical use #2:
		//   class MyFile : public IOHandler {
		//     MyFile() {
		//       ...
		//       message_pump->RegisterIOHandler(file_, this);
		//     }
		//     // Plus some code to make sure that this destructor is not called
		//     // while there are pending IO operations.
		//     ~MyFile() {
		//     }
		//     virtual void OnIOCompleted(IOContext* context, DWORD bytes_transfered,
		//                                DWORD error) {
		//       ...
		//       delete context;
		//     }
		//     void DoSomeIo() {
		//       ...
		//       IOContext* context = new IOContext;
		//       // This is not used for anything. It just prevents the context from
		//       // being considered "abandoned".
		//       context->handler = this;
		//       ReadFile(file_, buffer, num_bytes, &read, &context->overlapped);
		//     }
		//     HANDLE file_;
		//   };
		//
		// Typical use #3:
		// Same as the previous example, except that in order to deal with the
		// requirement stated for the destructor, the class calls WaitForIOCompletion
		// from the destructor to block until all IO finishes.
		//     ~MyFile() {
		//       while(pending_)
		//         message_pump->WaitForIOCompletion(INFINITE, this);
		//     }
		//
		struct IOHandler {
		public:
			virtual ~IOHandler() {
			}

			virtual void OnIOCompleted(IOContext *context, DWORD bytes_transfered, DWORD error) = 0;
		};

		struct IOObserver {
		public:
			IOObserver() {

			}

			virtual void PreProcessIOEvent() = 0;
			virtual void PostProcessIOEvent() = 0;
		protected:
			~IOObserver() {

			}
		};

		struct IOContext {
			OVERLAPPED overlapped_;
			IOHandler *handler_;
		};

		IOMessagePump();
		virtual ~IOMessagePump() {
		}

		virtual void ScheduleWork();
		virtual void ScheduleDelayWork(const TimeTicks &delayed_work_time);
		void RegisterIOHandler(HANDLE handle, IOHandler *filter);
		bool WaitForIOCompletion(DWORD time_out, IOHandler *filter);
		void AddIOObserver(IOObserver *observer);
		void RemoveIOObserver(IOObserver *observer);
	private:
		struct IOItem{
			IOContext *context_;
			IOHandler *handler_;
			DWORD bytes_transfered_;
			DWORD error_;
		};
		void DoRunLoop();
		void WaitForWork();
		bool MatchCompleteIOItem(IOHandler *filter, IOItem *item);
		bool GetIOItem(DWORD time_out, IOItem *item);
		bool ProcessInternalIOItem(const IOItem &item);
		void PreProcessIOEvent();
		void PostProcessIOEvent();
		HANDLE port_;
		// This list will be empty almost always. It stores IO completions that have
		// not been delivered yet because somebody was doing cleanup.
		std::list<IOItem> completed_io_;
		std::shared_ptr<ObserverList<IOObserver>> io_observers_;
	};
}

#endif// BASE_FRAMEWORK_MESSAGE_PUMP_IO_H__