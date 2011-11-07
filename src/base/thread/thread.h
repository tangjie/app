/*
 * Define the thread interface for windows.
 */

#ifndef BASE_THREAD_THREAD_H__
#define BASE_THREAD_THREAD_H__
#include <assert.h>
#include <process.h>
#include <Windows.h>
#include "base/base_types.h"

namespace base {
	typedef uint32_t ThreadId;
	typedef void* ThreadHandle;
	//define the priority for thread, the default value of thread is kThreadPriorityNormal
	enum ThreadPriority {
		kThreadPriorityLow,
		kThreadPriorityNormal,
		kThreadPriorityHigh,
		kThreadPriorityRealtime
	};

	enum ThreadState {
		kInitialized,
		kRunning,
		kSuspend,
		kTerminate,
		kStoped
	};

	class Thread {
	public:
		Thread();
		~Thread();
		// start a thread if a thread is not started.
		bool Start();
		// Stop method will wait for the thread end and return.
		void Stop();
		// Terminate will block the thread and return.
		void Terminate();
		//Pause a thread.
		bool Suspend();
		//Resume a thread.
		bool Resume();
		static void Sleep(int64_t milliseconds);
		// Get the current active thread id of process
		static ThreadId CurrentId();
		ThreadId thread_id() const;
		void set_thread_id(ThreadId thread_id);
		ThreadHandle thread_handle() const;
		ThreadPriority thread_priority() const;
		void set_thread_priority(ThreadPriority thread_priority);
		ThreadState thread_state();
	protected:
		virtual void Run() {
		};

	private:
		static uint32_t __stdcall ThreadProc(void* params);
		//Convert the ThreadPriority of thread to system identification priority level
		int ConvertPriorityToSystemPriority(ThreadPriority thread_priority); 
		ThreadState thread_state_;
		ThreadId thread_id_;
		ThreadHandle thread_handle_;
		ThreadPriority thread_priority_;
	};
}

#endif// BASE_THREAD_THREAD_H__