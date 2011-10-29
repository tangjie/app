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
		auto a;
		std::tr1::tie(a) = arg;
		(obj->*method)(a);
	}
}

#endif// BASE_FRAMEWORK_TASK_BUILDER_H__