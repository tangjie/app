#include "base/synchronization/waitable_event.h"

namespace base {
	WaitableEvent::WaitableEvent(bool manual_reset, bool was_signaled) {
		handle_ = CreateEvent(nullptr, manual_reset, was_signaled, nullptr);
		assert(handle_ != nullptr);
	}

	WaitableEvent::~WaitableEvent() {
		CloseHandle(handle_);
	}

	HANDLE WaitableEvent::Release() {
		HANDLE result = handle_;
		handle_ = INVALID_HANDLE_VALUE;
		return result;
	}

	void WaitableEvent::Reset() {
		ResetEvent(handle_);
	}

	void WaitableEvent::Signal() {
		SetEvent(handle_);
	}

	void WaitableEvent::Wait() {
		DWORD result = WaitForSingleObject(handle_, INFINITE);
		assert(result == WAIT_OBJECT_0);
	}

	bool WaitableEvent::WaitForTime(int64_t wait_ms) {
		assert(wait_ms >= 0);
		DWORD result = WaitForSingleObject(handle_, static_cast<DWORD>(wait_ms));
		bool time_out = true;
		switch(result) {
		case WAIT_OBJECT_0:
			time_out = false;
			break;
		case  WAIT_TIMEOUT:
			time_out = true;
			break;
		default:
			assert(false);
			// TODO(tangjie): add log for wait failed here.
		}
		// if time out return false.
		return !time_out;
	}

	size_t WaitableEvent::WaitMany(WaitableEvent **waitables, size_t count) {
		assert(count <= MAXIMUM_WAIT_OBJECTS);
		HANDLE handles[MAXIMUM_WAIT_OBJECTS];
		for (size_t i = 0; i < count; i++) {
			handles[i] =  waitables[i]->handle();
		}
		DWORD result = WaitForMultipleObjects(static_cast<DWORD>(count), handles, FALSE, INFINITE);
		if (result > WAIT_OBJECT_0 + count) {
			assert(false);
			// TODO(tangjie): add log for wait failed here.
			return 0;
		}
		return result - WAIT_OBJECT_0;
	}

	bool WaitableEvent::WasSignaled() {
		return WaitForTime(0);
	}

}