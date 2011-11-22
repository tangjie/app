#include "base/at_exit_manager.h"

#include <assert.h>
#include <memory>

namespace {
	//当前的AtExitManager，目前不允许多个manager存在
	base::AtExitManager* g_manager = nullptr;
}

namespace base {
	AtExitManager::AtExitManager() {
		assert(g_manager == nullptr);
		g_manager = this;
	}
	
	AtExitManager::~AtExitManager() {
		assert(g_manager == this);
		ProcessCallbacksNow();
		g_manager = nullptr;
	}
	//static
	void AtExitManager::RegisterCallback(CallbackType func, void* param) {
		if(g_manager == nullptr) {
			//TODO(oldman): log以及报错
			assert(false);
		}
		base::AutoLock lock(g_manager->lock_);
		g_manager->callback_stack_.push(std::make_pair(func, param));
	}
	//static
	void AtExitManager::ProcessCallbacksNow() {
		if(g_manager == nullptr) {
			//TODO(oldman): log以及报错
			assert(false);
		}
		AutoLock lock(g_manager->lock_);
		std::stack<CallbackAndParam>& stack = g_manager->callback_stack_;
		while(!stack.empty()) {
			//call the callback
			stack.top().first(stack.top().second);
			stack.pop();
		}
	}
}
			

