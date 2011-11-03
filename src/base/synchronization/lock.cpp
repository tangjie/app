/*
 * Implementation of lock 
 */

#include "base/synchronization/lock.h"

namespace base {
	LockImpl::LockImpl() {
		InitializeCriticalSectionAndSpinCount(&critical_section_,2000);
	}

	LockImpl::~LockImpl() {
		DeleteCriticalSection(&critical_section_);
	}

	bool LockImpl::Try() {
		bool result = false;
		if (TryEnterCriticalSection(&critical_section_)) {
			result = true;
		}
		return result;
	}

	void LockImpl::Lock() {
		EnterCriticalSection(&critical_section_);
	}

	void LockImpl::Unlock() {
		LeaveCriticalSection(&critical_section_);
	}
}