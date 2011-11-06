/*
 * To define lock
 */

#ifndef BASE_SYNCHRONIZATION_LOCK_H__
#define BASE_SYNCHRONIZATION_LOCK_H__

#include <Windows.h>
#include "base/util/noncopyable.h"

namespace base {
	class LockImpl {
	public:
		LockImpl();
		~LockImpl();
		bool Try();
		void Lock();
		void Unlock();
	private:
		CRITICAL_SECTION critical_section_;
	};

	class AutoLock :public noncopyable {
	public: 
		AutoLock(LockImpl &lock) : lock_(lock){
			lock_.Lock();
		}

		~AutoLock() {
			lock_.Unlock();
		}

	private:
		LockImpl& lock_;
	};
}
#endif// BASE_SYNCHRONIZATION_LOCK_H__