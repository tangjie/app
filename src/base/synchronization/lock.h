/*
 * To define lock
 */

#ifndef BASE_SYNCHRONIZATION_LOCK_H__
#define BASE_SYNCHRONIZATION_LOCK_H__

#include <Windows.h>

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

	class AutoLock {
	public: 
		AutoLock(LockImpl &lock) : lock_(lock){
			lock_.Lock();
		}

		~AutoLock() {
			lock_.Unlock();
		}

	private:
		LockImpl& lock_;
		AutoLock(const AutoLock &auto_lock);
		AutoLock& operator=(const AutoLock &auto_lock);
	};
}
#endif// BASE_SYNCHRONIZATION_LOCK_H__