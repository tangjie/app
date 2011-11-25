/*
 * This file defined some helper functions to Invoke a method or function with certain params.it used
 * std::trl::tuple to pass the arguments.
 */

#ifndef BASE_FRAMEWORK_TASK_BUILDER_H__
#define BASE_FRAMEWORK_TASK_BUILDER_H__

#include <tuple>

namespace base {
	using std::tr1::tuple;
	template<class T, class Method>
	inline void DispatchToMethod(T *obj, Method method, const tuple<std::tr1::_Nil> &arg) {
		(obj->*method)();
	}

	template<class T, class Method, class A>
	inline void DispatchToMethod(T *obj, Method method, const A &arg) {
		(obj->*method)(arg);
	}

	template<class T, class Method, class A>
	inline void DispatchToMethod(T *obj, Method method, const tuple<A> &arg) {
		(obj->*method)(get<0>(arg));
	}
}

#endif// BASE_FRAMEWORK_TASK_BUILDER_H__