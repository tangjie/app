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
			obj_ = NULL;
		}

		virtual void Run() {
			if (obj_) {
				DispatchToMethod(obj_,method_,params_);
			}
		}

		virtual void Cancel() {
			T *obj = obj_;
			obj_ = NULL;
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
}
#endif// BASE_FRAMEWORK_TASK_H__