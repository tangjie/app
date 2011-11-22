/*
 * AtExitManager,就像是crt里的atexit一样，可以允许你指定退出时调用的函数，只不过AtExitManager还可以确保按照FILO的方式
 * 调用这一系列函数。之所以做这样一个AtExitManager，是因为C++中静态(static/global)的变量析构顺序是undefined。接口及实
 * 现参考了chromium，并且按照我们的需求做了一部分更改。
 *
 * 示例：
 * int main() {
 * 	base::AtExitManager at_exit;
 * 	.....
 * 	foo();			//bar()函数就会注册到|at_exit|上了
 * 	.....
 * 	return 0;
 * }				//bar()函数被调用
 * void bar(void* param) {
 * 	....
 * }
 * void foo() {
 * 	....
 * 	base::AtExitManager::RegisterCallback(bar, foobarparam);
 * }
 *
 * chromium的实现里是有ShadowManager存在的，根据其注释Shadow用作unittest，正常情况下用不到。因此此处去掉了ShadowManager
 * 的支持。不过此处有一个问题就是多模块（DLL）情况下如何使用AtExitManager。
 * 多模块下，如果各模块下均有一个Manager，并且Manager为静态变量，则其析构顺序不受我们控制，达不到我们想要的目的，并且多
 * 模块的Manager管理也是个较为麻烦的问题。因此此处采取的方案是由主模块（EXE/DLL）初始化ExitManager，其他模块均使用此
 * Manager。需要注意的是这个Manager的初始化要发生在所有模块的RegisterCallback调用之前。
 * ref:(1) http://src.chromium.org/svn/trunk/src/base/at_exit.h
 */

#ifndef BASE_AT_EXIT_MANAGER_H__
#define BASE_AT_EXIT_MANAGER_H__

#include <stack>
#include "base/synchronization/lock.h"
#include "base/util/noncopyable.h"

namespace base {
	class AtExitManager: public noncopyable {
	public:
		//构造函数。构建一个Manager的时候会隐藏掉已经存在的Manager
		AtExitManager();
		//析构函数。Manager析构时会按照FILO的方式调用已经注册的函数，并且在这之后被隐藏掉的
		~AtExitManager();
		//注册callback，注册的函数会在Manager析构或者Process*ManagerCallbacks()时按照LIFO的方式被调用
		typedef void (*CallbackType) (void*);
		static void RegisterCallback(CallbackType func, void* param);
		//立即执行manager内已经注册的函数，并且清除记录（相当于Manager在这里析构了一次）
		static void ProcessCallbacksNow();
	private:
		typedef std::pair<CallbackType, void*> CallbackAndParam;
		std::stack<CallbackAndParam> callback_stack_;
		base::LockImpl lock_;
	};
}

#endif // #ifndef BASE_AT_EXIT_MANAGER_H__
