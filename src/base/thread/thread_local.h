/*
 * To implement thread local storage.  
 * Usage:
 * class Foo {
 * public:
 *		Foo() {
 *			base::internal::LocalStorge<Foo>::GetInstance()->Set(this);	
 *		}
 *		static Foo* current() {
 *			return base::internal::LocalStorge<Test>::GetInstance()->Get();
 *		}
 * }
 */

#ifndef BASE_THREAD_THREAD_LOCAL_H__
#define BASE_THREAD_THREAD_LOCAL_H__

#include "base/base_types.h"
#include "base/memory/singleton.h"
#include "base/util/noncopyable.h"

namespace base {
	namespace internal {
		class ThreadLocalImpl {
		public:
			typedef unsigned long SlotType;
			static void AllocSlot(SlotType &slot);
			static void FreeSlot(SlotType &slot);
			static void SetValueInSlot(SlotType &slot, void *value);
			static void* GetValueFromSlot(SlotType &slot);
		};

	template<typename Type>
	class LocalStorage : noncopyable {
	public: 
		static LocalStorage<Type>* GetInstance() {
			return Singleton<LocalStorage<Type>>::Get();
		}
		~LocalStorage() {
			internal::ThreadLocalImpl::FreeSlot(slot_);
		}

		Type* Get() {
			return static_cast<Type*>(ThreadLocalImpl::GetValueFromSlot(slot_));
		}

		void Set(Type* value) {
			ThreadLocalImpl::SetValueInSlot(slot_, value);
		}
	private:
		LocalStorage(){
			ThreadLocalImpl::AllocSlot(slot_);
		}
		ThreadLocalImpl::SlotType slot_;
		friend struct DefaultSingletonTraits<LocalStorage<Type>>;
	};
}
}

#endif //#ifndef BASE_THREAD_THREAD_LOCAL_H__
