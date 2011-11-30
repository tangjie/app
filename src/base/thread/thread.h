/*
 * Define the thread interface for windows.
 */

#ifndef BASE_THREAD_THREAD_H__
#define BASE_THREAD_THREAD_H__
#include <assert.h>
#include "base/base_types.h"
#include "base/framework/message_loop.h"
#include "base/synchronization/waitable_event.h"
#include "base/thread/thread_helper.h"

namespace base {
	class Thread {
	public:
		struct Options{
			Options(MessageLoop::MessageLoopType type=MessageLoop::kDefaultMessageLoop)
				: message_loop_type_(type){
			}

			MessageLoop::MessageLoopType message_loop_type_;
		};

		Thread();
		~Thread();
		// Start a thread if a thread is not started.
		bool Start();
		//
		bool StartWithOptions(const Options &options);
		// Stop method will wait for the thread end and return.
		void Stop();
		// Terminate will block the thread and return.
		void Terminate();
		// Pause a thread.
		bool Suspend();
		// Resume a thread.
		bool Resume();
		ThreadId thread_id() const;
		ThreadHandle thread_handle() const;
		MessageLoop* message_loop() const {
			return message_loop_;
		}

		bool was_started() const {
			return was_started_;
		}

		bool IsRunning() const {
			return thread_id_ != kInvalidThreadId;
		}
	protected:
		// extra work before the thread start.
		virtual void SetUp() {
		}

		virtual void Run(MessageLoop *message_loop) {
			if (message_loop != nullptr) {
				message_loop->Run();
			}
		};

		// extra work before the thread end.
		virtual void TearDown() {
		}

	private:
		void ThreadMain();
		void StopInternal();
		void Quit();
		//Convert the ThreadPriority of thread to system identification priority level
		ThreadId thread_id_;
		ThreadHandle thread_handle_;
		MessageLoop *message_loop_;
		// Used to pass data to ThreadMain.
		struct StartupData;
		StartupData* startup_data_;
		bool was_started_;
		bool stopping_;
	};
}

#endif// BASE_THREAD_THREAD_H__