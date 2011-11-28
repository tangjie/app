/*
 * Thread factory is used to produce Lightweight thread.you must provide a RunnableMethod as the params.
 * you can use MakeRunnableMethod method to generate the params.
 * WARNING: You should *NOT* be using this class directly. ThreadHelper is the low-level threading interface.
 * You should instead be using Thread Object, see thread.h.
 */
#ifndef BASE_THREAD_THREAD_FACTORY_H__
#define BASE_THREAD_THREAD_FACTORY_H__

#include <assert.h>
#include <process.h>
#include <Windows.h>
#include "base/base_types.h"
#include "base/framework/task.h"

namespace base {
	enum ThreadPriority{
		kThreadPriorityNormal,
		// Suitable for low-latency, glitch-resistant audio.
		kThreadPriorityRealtimeAudio
	};

	class ThreadHelper {
	public:
		typedef std::shared_ptr<CancelableTask> ThreadCallback;
		typedef void* ThreadHandle;
		typedef DWORD ThreadId;
		// Create a thread. handle will return the Handle to the thread.it can not be null.
		static bool Create(ThreadCallback callback, ThreadHandle *handle);
		// NonJoinable means you can never wait the the thread.it has been out of control.
		static bool CreateNonJoinable(ThreadCallback callback);
		static void Sleep(int sleep_ms);
		// Switch to other thread.
		static void YliedCurrentThread();
		static DWORD CurrentId();
		// Wait a thread to complete.
		static void Join(ThreadHandle handle);
		static void SetThreadPriority(ThreadHandle handle, ThreadPriority priority);
	private:
		ThreadHelper(){
		}

		static bool CreateThreadInternal(ThreadCallback callback, ThreadHandle *handle);
	};
}

#endif// BASE_THREAD_THREAD_FACTORY_H__