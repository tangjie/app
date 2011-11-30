#include "base/thread/thread_helper.h"

namespace base {
	struct ThreadParams{
		ThreadCallback callback;
		bool joinable;
	};

	DWORD _stdcall ThreadFunc(void *params) {
		ThreadParams *thread_params = static_cast<ThreadParams*>(params);
		thread_params->callback->Run();
		delete thread_params;
		return 0;
	}

	bool ThreadHelper::CreateThreadInternal(ThreadCallback callback, ThreadHandle *handle) {
		ThreadParams *thread_params = new ThreadParams();
		thread_params->callback = callback;
		thread_params->joinable = (handle == nullptr);
		// Using CreateThread here vs _beginthreadex makes thread creation a bit
		// faster and doesn't require the loader lock to be available.  Our code will
		// have to work running on CreateThread() threads anyway, since we run code
		// on the Windows thread pool, etc.  For some background on the difference:
		//   http://www.microsoft.com/msj/1099/win32/win321099.aspx
		ThreadHandle thread_handle;
		thread_handle = CreateThread(nullptr, 0, ThreadFunc, thread_params, 0, nullptr);
		if (thread_handle == nullptr) {
			delete thread_params;
			return false;
		}
		if (handle != nullptr) {
			*handle = thread_handle;
		}else {
			CloseHandle(thread_handle);
		}
		return true;
	}

	bool ThreadHelper::CreateNonJoinable(ThreadCallback callback) {
		return CreateThreadInternal(callback, nullptr);
	}

	bool ThreadHelper::Create(ThreadCallback callback, ThreadHandle *handle) {
		assert(handle != nullptr);
		return CreateThreadInternal(callback, handle);
	}

	DWORD ThreadHelper::CurrentId() {
		return GetCurrentThreadId();
	}

	void ThreadHelper::Join(ThreadHandle handle) {
		assert(handle != nullptr);
		DWORD result = WaitForSingleObject(handle, INFINITE);
		assert(result == WAIT_OBJECT_0);
		CloseHandle(handle);
	}

	void ThreadHelper::SetThreadPriority(ThreadHandle handle, ThreadPriority priority) {
		assert(handle != nullptr);
		DWORD system_priority = THREAD_PRIORITY_NORMAL;
		switch(priority) {
		case kThreadPriorityNormal:
			system_priority = THREAD_PRIORITY_NORMAL;
			break;
		case kThreadPriorityRealtimeAudio:
			system_priority = THREAD_PRIORITY_TIME_CRITICAL;
			break;
		default:
			assert(false);
		}
		::SetThreadPriority(handle, system_priority);
	}

	void ThreadHelper::Sleep(int sleep_ms) {
		::Sleep(sleep_ms);
	}

	void ThreadHelper::YliedCurrentThread() {
		::Sleep(0);
	}

}