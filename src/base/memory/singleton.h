#ifndef BASE_MEMORY_SINGLETON_H__
#define BASE_MEMORY_SINGLETON_H__

#include <Windows.h>
#include "base/at_exit_manager.h"
#include "base/base_types.h"
#include "base/util/noncopyable.h"


namespace base {
	enum CreateInstanceState {
		kNotCreate = 0,
		kCreating,
		kCreated
	};

	template<typename Type>
	struct DefaultSingletonTraits {
		// Allocates the object.
		static Type* New() {
			return new Type();
		}

		// Delete the object.
		static void Delete(Type* x) {
			delete x;
		}
	};

	template<typename Type, typename Traits = DefaultSingletonTraits<Type>>
	class Singleton : public noncopyable {
	public:
		static Type* Get() {
			// if the instance had been created. just return the instance.
			if (state_ == kCreated) {
				return instance_;
			}
			// detect the value of state_, if another thread is creating the instance. wait for the
			// instance creating complete. and return the instance. 
			if (InterlockedExchange(&state_, static_cast<LONG>(kCreating)) == kNotCreate) {
				// only the first thread modify the value of state_ can get here.
				instance_ = Traits::New();
				AtExitManager::RegisterCallback(OnExit, nullptr);
				InterlockedExchange(&state_, static_cast<LONG>(kCreated));
				return instance_;
			}else {
				return WaitForInstance();
			}
		}
		static Type* WaitForInstance() {
			while(true) {
				if (state_ == kCreated) {
					break;
				}
				// TODO(tangjie): move Sleep(0) to Thread object. 
				Sleep(0);
			}
			return instance_;
		}
	private:
		static void OnExit(void *params) {
			Traits::Delete(instance_);
		}
		friend Type* Type::GetInstance();
		Singleton() {
		}
		static LONG state_;
		static Type* instance_;
	};
	template<typename Type, typename Traits>
	Type* Singleton<Type, Traits>::instance_ = nullptr;
	template<typename Type, typename Traits>
	LONG Singleton<Type, Traits>::state_ = 0;
}
#endif //BASE_MEMORY_SINGLETON_H_