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

	template<class T, class Method, class A, class B>
	inline void DispatchToMethod(T *obj, Method method, const tuple<A, B> &arg) {
		(obj->*method)(get<0>(arg), get<1>(arg));
	}

	template<class T, class Method, class A, class B, class C>
	inline void DispatchToMethod(T *obj, Method method, const tuple<A, B, C> &arg) {
		(obj->*method)(get<0>(arg), get<1>(arg), get<2>(arg));
	}

	template<class T, class Method, class A, class B, class C, class D>
	inline void DispatchToMethod(T *obj, Method method, const tuple<A, B, C, D> &arg) {
		(obj->*method)(get<0>(arg), get<1>(arg), get<2>(arg), get<3>(arg));
	}

	template<class T, class Method, class A, class B, class C, class D, class E>
	inline void DispatchToMethod(T *obj, Method method, const tuple<A, B, C, D, E> &arg) {
		(obj->*method)(get<0>(arg), get<1>(arg), get<2>(arg), get<3>(arg), get<4>(arg));
	}

	template<class T, class Method, class A, class B, class C, class D, class E, class F>
	inline void DispatchToMethod(T *obj, Method method, const tuple<A, B, C, D, E, F> &arg) {
		(obj->*method)(get<0>(arg), get<1>(arg), get<2>(arg), get<3>(arg), get<4>(arg), get<5>(arg));
	}

	template<class T, class Method, class A, class B, class C, class D, class E, class F, class G>
	inline void DispatchToMethod(T *obj, Method method, const tuple<A, B, C, D, E, F, G> &arg) {
		(obj->*method)(get<0>(arg), get<1>(arg), get<2>(arg), get<3>(arg), get<4>(arg), get<5>(arg), get<6>(arg));
	}

	template<class T, class Method, class A, class B, class C, class D, class E, class F, class G, class H>
	inline void DispatchToMethod(T *obj, Method method, const tuple<A, B, C, D, E, F, G, H> &arg) {
		(obj->*method)(get<0>(arg), get<1>(arg), get<2>(arg), get<3>(arg), get<4>(arg), get<5>(arg), get<6>(arg), get<7>(arg));
	}

	template<class T, class Method, class A, class B, class C, class D, class E, class F, class G, class H, class I>
	inline void DispatchToMethod(T *obj, Method method, const tuple<A, B, C, D, E, F, G, H, I> &arg) {
		(obj->*method)(get<0>(arg), get<1>(arg), get<2>(arg), get<3>(arg), get<4>(arg), get<5>(arg), get<6>(arg), get<7>(arg), get<8>(arg));
	}

	template<class Func>
	inline void DispatchToFunction(Func func, const tuple<std::tr1::_Nil> &arg) {
		(*func)();
	}

	template<class Func,  class A>
	inline void DispatchToFunction(Func func, const tuple<A> &arg) {
		(*func)(get<0>(arg));
	}

	template<class Func,  class A, class B>
	inline void DispatchToFunction(Func func, const tuple<A, B> &arg) {
		(*func)(get<0>(arg), get<1>(arg));
	}

	template<class Func,  class A, class B, class C>
	inline void DispatchToFunction(Func func, const tuple<A, B, C> &arg) {
		(*func)(get<0>(arg), get<1>(arg), get<2>(arg));
	}

	template<class Func,  class A, class B, class C, class D>
	inline void DispatchToFunction(Func func, const tuple<A, B, C, D> &arg) {
		(*func)(get<0>(arg), get<1>(arg), get<2>(arg), get<3>(arg));
	}

	template<class Func,  class A, class B, class C, class D, class E>
	inline void DispatchToFunction(Func func, const tuple<A, B, C, D, E> &arg) {
		(*func)(get<0>(arg), get<1>(arg), get<2>(arg), get<3>(arg), get<4>(arg));
	}

	template<class Func,  class A, class B, class C, class D, class E, class F>
	inline void DispatchToFunction(Func func, const tuple<A, B, C, D, E, F> &arg) {
		(*func)(get<0>(arg), get<1>(arg), get<2>(arg), get<3>(arg), get<4>(arg), get<5>(arg));
	}

	template<class Func,  class A, class B, class C, class D, class E, class F, class G>
	inline void DispatchToFunction(Func func, const tuple<A, B, C, D, E, F, G> &arg) {
		(*func)(get<0>(arg), get<1>(arg), get<2>(arg), get<3>(arg), get<4>(arg), get<5>(arg), get<6>(arg));
	}

	template<class Func,  class A, class B, class C, class D, class E, class F, class G, class H>
	inline void DispatchToFunction(Func func, const tuple<A, B, C, D, E, F, G, H> &arg) {
		(*func)(get<0>(arg), get<1>(arg), get<2>(arg), get<3>(arg), get<4>(arg), get<5>(arg), get<6>(arg), get<7>(arg));
	}

	template<class Func,  class A, class B, class C, class D, class E, class F, class G, class H, class I>
	inline void DispatchToFunction(Func func, const tuple<A, B, C, D, E, F, G, H, I> &arg) {
		(*func)(get<0>(arg), get<1>(arg), get<2>(arg), get<3>(arg), get<4>(arg), get<5>(arg), get<6>(arg), get<7>(arg), get<8>(arg));
	}
}

#endif// BASE_FRAMEWORK_TASK_BUILDER_H__