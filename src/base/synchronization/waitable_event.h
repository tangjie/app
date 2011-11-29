/*
 * A WaitableEvent can be a useful thread synchronization tool when you want to
 * allow one thread to wait for another thread to finish some work. 
 * ref: http://src.chromium.org/svn/trunk/src/base/synchronization/waitable_event.h
 */
#ifndef BASE_SYNCHRONIZATION_WAITABLE_EVENT_H__
#define BASE_SYNCHRONIZATION_WAITABLE_EVENT_H__

#include <Windows.h>
#include "base/base_types.h"

namespace base {
	class WaitableEvent {
	public:
		// If manual_reset is true, then to set the event state to non-signaled, a
		// consumer must call the Reset method.  If this parameter is false, then the
		// system automatically resets the event state to non-signaled after a single
		// waiting thread has been released.
		WaitableEvent(bool manual_reset, bool was_signaled);
		// Create a WaitableEvent from an Event HANDLE which has already been
		// created. This objects takes ownership of the HANDLE and will close it when
		// deleted.
		explicit WaitableEvent(HANDLE handle) : handle_(handle) {
		}

		~WaitableEvent();
		HANDLE handle() const {
			return handle_;
		}
		// Release the owner ship of handle.
		HANDLE Release();
		// Set the event in the non-signaled state.
		void Reset();
		// Set the event in the signaled state.
		void Signal();
		// Return the state of the event. true when the state is signaled. Otherwise false.
		bool WasSignaled();
		// Wait for the event to be signaled.
		void Wait();
		// Wait until the wait_ms time passed or the event was signaled. Return true if the event was signaled. Otherwise false.
		bool WaitForTime(int64_t wait_ms);
		// Wait, synchronously, on multiple events.
		// waitables: an array of WaitableEvent pointers
		// count: the number of elements in @waitables
		// returns: the index of a WaitableEvent which has been signaled.
		// You MUST NOT delete any of the WaitableEvent objects while this wait is
		// happening.
		static size_t WaitMany(WaitableEvent **waitables, size_t count);
	private:
		HANDLE handle_;
	};
}

#endif// BASE_SYNCHRONIZATION_WAITABLE_EVENT_H__