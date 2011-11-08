#include "base/thread/thread.h"

namespace base {
	Thread::Thread() : thread_id_(0), thread_handle_(NULL), thread_priority_(kThreadPriorityNormal), thread_state_(kInitialized) {
	}

	Thread::~Thread() {
		Stop();
	}

	bool Thread::Start() {
		assert(thread_state_ == kInitialized);
		if (thread_state_ != kInitialized) {
			return false;
		}
		thread_handle_ = reinterpret_cast<ThreadHandle>(_beginthreadex(NULL, 0, ThreadProc, this, 0, (unsigned int*)&thread_id_));
		int priority = THREAD_PRIORITY_BELOW_NORMAL;
		ConvertPriorityToSystemPriority(thread_priority_);
		SetThreadPriority (thread_handle_, priority);
		thread_state_ = kRunning;
		return true;
	}

	void Thread::Stop() {
		if (thread_handle_ != NULL) {
			if (WAIT_OBJECT_0 == WaitForSingleObject(thread_handle_, INFINITE)) {
				CloseHandle(thread_handle_);
				thread_handle_ = NULL;
				delete runnable_delegate_;
				runnable_delegate_ = NULL;
				thread_id_ = 0;
			}else {
				Terminate();
			}
		}
		thread_state_ = kStoped;
	}

	void Thread::Terminate() {
		if (thread_handle_ !=NULL) {
			TerminateThread(thread_handle_, 0);
			CloseHandle(thread_handle_);
			thread_handle_ = NULL;
			delete runnable_delegate_;
			runnable_delegate_ = NULL;
			thread_id_ = 0;
		}
		thread_state_ = kTerminate;
	}

	bool Thread::Suspend() {
		if (thread_state_ == kRunning && thread_handle_ != NULL) {
			SuspendThread(thread_handle_);
			thread_state_ = kSuspend;
			return true;
		}
		return false;
	}

	bool Thread::Resume() {
		if (thread_state_ == kSuspend && thread_handle_ != NULL) {
			ResumeThread(thread_handle_);
			thread_state_ = kRunning;
			return true;
		}
		return false;
	}

	void Thread::Sleep(int64_t milliseconds) {
		::Sleep(milliseconds);
	}

	ThreadId Thread::CurrentId() {
		return GetCurrentThreadId();
	}

	ThreadId Thread::thread_id() const {
		return thread_id_;
	}

	void Thread::set_thread_id(ThreadId thread_id) {
		thread_id_ = thread_id;
	}

	ThreadHandle Thread::thread_handle() const {
		return thread_handle_;
	}

	ThreadPriority Thread::thread_priority() const {
		return thread_priority_;
	}

	void Thread::set_thread_priority(ThreadPriority thread_priority) {
		thread_priority_ = thread_priority;
		int priority = THREAD_PRIORITY_NORMAL;
		ConvertPriorityToSystemPriority(thread_priority_);
		SetThreadPriority (thread_handle_, priority);
	}

	void Thread::set_runnable_delegate(Runnable *delegate) {
		assert(thread_state_ == kInitialized);
		if (thread_state_ != kInitialized) {
			return;
		}
		runnable_delegate_ = delegate;
	}

	ThreadState Thread::thread_state() {
		return thread_state_;
	}

	uint32_t Thread::ThreadProc(void* params) {
		Thread* thread = reinterpret_cast<Thread*>(params);
		assert(thread != NULL);
		if (thread != NULL) {
			thread->Execute();
		}
		// why call _endthreadex.
		_endthreadex(0);
		return 0;
	}

	int Thread::ConvertPriorityToSystemPriority(ThreadPriority thread_priority) {
		int priority = THREAD_PRIORITY_BELOW_NORMAL;
		if (thread_priority_ == kThreadPriorityLow) {
			priority = THREAD_PRIORITY_LOWEST;
		}else if (thread_priority_ == kThreadPriorityNormal) {
			priority = THREAD_PRIORITY_BELOW_NORMAL;
		}else if (thread_priority_ == kThreadPriorityHigh) {
			priority = THREAD_PRIORITY_HIGHEST;
		}else if (thread_priority_ == kThreadPriorityRealtime) {
			priority = THREAD_PRIORITY_TIME_CRITICAL;
		}
		return priority;
	}
}