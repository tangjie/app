/*
 * This file defined the task class and  runnable method builder.
 */

#ifndef BASE_FRAMEWORK_TASK_H__
#define BASE_FRAMEWORK_TASK_H__

#include <memory>
#include "base/util/invoke_helper.h"

namespace base {
	class Task {
	public:
		virtual ~Task() {
		}

		virtual void Run() = 0;
	};

	class CancelableTask : public Task {
	public:
		CancelableTask() {
		}

		virtual ~CancelableTask() {
		}
		virtual void Cancel() = 0;
	};

	template<class T, class Method, class Params>
	class RunnableMethod : public CancelableTask {
	public:
		RunnableMethod(T *obj, Method method, Params params) 
			: obj_(obj), method_(method), params_(params) {
		}

		~RunnableMethod() {
			obj_ = nullptr;
		}

		virtual void Run() {
			// TODO(tangjie):the object may be destructed. so we need to check its validity.
			if (obj_) {
				DispatchToMethod(obj_,method_,params_);
			}
		}

		virtual void Cancel() {
			T *obj = obj_;
			obj_ = nullptr;
		}

	private:
		T *obj_;
		Method method_;
		Params params_;
	};

	template<class T, class Method>
	inline std::shared_ptr<CancelableTask> MakeRunnableMethod(T *obj, Method method) {
		return std::shared_ptr<CancelableTask>(new RunnableMethod<T, Method, std::tr1::tuple<std::tr1::_Nil>>(obj, method, std::tr1::make_tuple(std::tr1::_Nil_obj)));
	}

	template<class T, class Method, class A>
	inline std::shared_ptr<CancelableTask> MakeRunnableMethod(T *obj, Method method, const A &a) {
		return std::shared_ptr<CancelableTask>(new RunnableMethod<T, Method, std::tr1::tuple<A>>(obj, method, std::tr1::make_tuple(a)));
	}

	template<class T, class Method, class A, class B>
	inline std::shared_ptr<CancelableTask> MakeRunnableMethod(T *obj, Method method, const A &a, const B &b) {
		return std::shared_ptr<CancelableTask>(new RunnableMethod<T, Method, std::tr1::tuple<A, B>>(obj, method, std::tr1::make_tuple(a, b)));
	}

	template<class T, class Method, class A, class B, class C>
	inline std::shared_ptr<CancelableTask> MakeRunnableMethod(T *obj, Method method, const A &a, const B &b, const C &c) {
		return std::shared_ptr<CancelableTask>(new RunnableMethod<T, Method, std::tr1::tuple<A, B, C>>(obj, method, std::tr1::make_tuple(a, b, c)));
	}

	template<class T, class Method, class A, class B, class C, class D>
	inline std::shared_ptr<CancelableTask> MakeRunnableMethod(T *obj, Method method, const A &a, const B &b, const C &c, const D &d) {
		return std::shared_ptr<CancelableTask>(new RunnableMethod<T, Method, std::tr1::tuple<A, B, C, D>>(obj, method, std::tr1::make_tuple(a, b, c, d)));
	}

	template<class T, class Method, class A, class B, class C, class D, class E>
	inline std::shared_ptr<CancelableTask> MakeRunnableMethod(T *obj, Method method, const A &a, const B &b, const C &c, const D &d, const E &e) {
		return std::shared_ptr<CancelableTask>(new RunnableMethod<T, Method, std::tr1::tuple<A, B, C, D, E>>(obj, method, std::tr1::make_tuple(a, b, c, d, e)));
	}

	template<class T, class Method, class A, class B, class C, class D, class E, class F>
	inline std::shared_ptr<CancelableTask> MakeRunnableMethod(T *obj, Method method, const A &a, const B &b, const C &c, const D &d, const E &e, const F &f) {
		return std::shared_ptr<CancelableTask>(new RunnableMethod<T, Method, std::tr1::tuple<A, B, C, D, E, F>>(obj, method, std::tr1::make_tuple(a, b, c, d, e, f)));
	}

	template<class T, class Method, class A, class B, class C, class D, class E, class F, class G>
	inline std::shared_ptr<CancelableTask> MakeRunnableMethod(T *obj, Method method, const A &a, const B &b, const C &c, const D &d, const E &e, const F &f,
		const G &g) {
		return std::shared_ptr<CancelableTask>(new RunnableMethod<T, Method, std::tr1::tuple<A, B, C, D, E, F, G>>(obj, method, std::tr1::make_tuple(a, b, c, d, e, f,
			g)));
	}

	template<class T, class Method, class A, class B, class C, class D, class E, class F, class G, class H>
	inline std::shared_ptr<CancelableTask> MakeRunnableMethod(T *obj, Method method, const A &a, const B &b, const C &c, const D &d, const E &e, const F &f,
		const G &g, const H &h) {
			return std::shared_ptr<CancelableTask>(new RunnableMethod<T, Method, std::tr1::tuple<A, B, C, D, E, F, G, H>>(obj, method, std::tr1::make_tuple(a, b, c, d, e, f,
				g,h)));
	}

	template<class T, class Method, class A, class B, class C, class D, class E, class F, class G, class H, class I>
	inline std::shared_ptr<CancelableTask> MakeRunnableMethod(T *obj, Method method, const A &a, const B &b, const C &c, const D &d, const E &e, const F &f,
		const G &g, const H &h, const I &i) {
			return std::shared_ptr<CancelableTask>(new RunnableMethod<T, Method, std::tr1::tuple<A, B, C, D, E, F, G, H, I>>(obj, method, std::tr1::make_tuple(a, b, c, d, e, f,
				g,h, i)));
	}

	template<class Func, class Params>
	class RunnableFunction : public CancelableTask {
	public:
		RunnableFunction(Func func, Params params) : func_(func), params_(params), was_canceled_(false) {
		}

		~RunnableFunction() {
		}

		virtual void Run() {
			if (!was_canceled_ && func_) {
				DispatchToFunction(func_, params_);
			}
		}

		virtual void Cancel() {
			was_canceled_ = true;
		}

	private:
		Func func_;
		Params params_;
		bool was_canceled_;
	};

	template<class Func>
	inline std::shared_ptr<CancelableTask> MakeRunnableFunction(Func func) {
		return std::shared_ptr<CancelableTask>(new RunnableFunction<func, std::tr1::tuple<std::tr1::_Nil>>(func, std::tr1::make_tuple(std::tr1::_Nil_obj)));
	}

	template<class Func, class A>
	inline std::shared_ptr<CancelableTask> MakeRunnableFunction(Func func, const A &a) {
		return std::shared_ptr<CancelableTask>(new RunnableFunction<func, std::tr1::tuple<A>>(func, std::tr1::make_tuple(a)));
	}

	template<class Func, class A, class B>
	inline std::shared_ptr<CancelableTask> MakeRunnableFunction(Func func, const A &a, const B &b) {
		return std::shared_ptr<CancelableTask>(new RunnableFunction<func, std::tr1::tuple<A, B>>(func, std::tr1::make_tuple(a, b)));
	}

	template<class Func, class A, class B, class C>
	inline std::shared_ptr<CancelableTask> MakeRunnableFunction(Func func, const A &a, const B &b, const C &c) {
		return std::shared_ptr<CancelableTask>(new RunnableFunction<func, std::tr1::tuple<A, B, C>>(func, std::tr1::make_tuple(a, b, c)));
	}

	template<class Func, class A, class B, class C, class D>
	inline std::shared_ptr<CancelableTask> MakeRunnableFunction(Func func, const A &a, const B &b, const C &c, const D &d) {
		return std::shared_ptr<CancelableTask>(new RunnableFunction<func, std::tr1::tuple<A, B, C, D>>(func, std::tr1::make_tuple(a, b, c, d)));
	}

	template<class Func, class A, class B, class C, class D, class E>
	inline std::shared_ptr<CancelableTask> MakeRunnableFunction(Func func, const A &a, const B &b, const C &c, const D &d, const E &e) {
		return std::shared_ptr<CancelableTask>(new RunnableFunction<func, std::tr1::tuple<A, B, C, D, E>>(func, std::tr1::make_tuple(a, b, c, d, e)));
	}

	template<class Func, class A, class B, class C, class D, class E, class F>
	inline std::shared_ptr<CancelableTask> MakeRunnableFunction(Func func, const A &a, const B &b, const C &c, const D &d, const E &e, const F &f) {
		return std::shared_ptr<CancelableTask>(new RunnableFunction<func, std::tr1::tuple<A, B, C, D, E, F>>(func, std::tr1::make_tuple(a, b, c, d, e, f)));
	}

	template<class Func, class A, class B, class C, class D, class E, class F, class G>
	inline std::shared_ptr<CancelableTask> MakeRunnableFunction(Func func, const A &a, const B &b, const C &c, const D &d, const E &e, const F &f,
		const G &g) {
		return std::shared_ptr<CancelableTask>(new RunnableFunction<func, std::tr1::tuple<A, B, C, D, E, F, G>>(func, std::tr1::make_tuple(a, b, c, d, e, f, 
			g)));
	}

	template<class Func, class A, class B, class C, class D, class E, class F, class G, class H>
	inline std::shared_ptr<CancelableTask> MakeRunnableFunction(Func func, const A &a, const B &b, const C &c, const D &d, const E &e, const F &f,
		const G &g, const H &h) {
			return std::shared_ptr<CancelableTask>(new RunnableFunction<func, std::tr1::tuple<A, B, C, D, E, F, G, H>>(func, std::tr1::make_tuple(a, b, c, d, e, f, 
				g, h)));
	}

	template<class Func, class A, class B, class C, class D, class E, class F, class G, class H, class I>
	inline std::shared_ptr<CancelableTask> MakeRunnableFunction(Func func, const A &a, const B &b, const C &c, const D &d, const E &e, const F &f,
		const G &g, const H &h, const I &i) {
			return std::shared_ptr<CancelableTask>(new RunnableFunction<func, std::tr1::tuple<A, B, C, D, E, F, G, H, I>>(func, std::tr1::make_tuple(a, b, c, d, e, f, 
				g, h, i)));
	}
}
#endif// BASE_FRAMEWORK_TASK_H__