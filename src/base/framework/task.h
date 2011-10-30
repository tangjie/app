/*
 * This file defined the task class and  runnable method builder.
 */

#ifndef BASE_FRAMEWORK_TASK_H__
#define BASE_FRAMEWORK_TASK_H__

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
	inline CancelableTask* MakeRunnableMethod(T *obj, Method method) {
		return new RunnableMethod<T, Method, std::tr1::_Nil>(obj, method, std::tr1::make_tuple());
	}

	template<class T, class Method, class A>
	inline CancelableTask* MakeRunnableMethod(T *obj, Method method, const A &a) {
		return new RunnableMethod(obj, method, std::tr1::make_tuple(a));
	}
}
#endif// BASE_FRAMEWORK_TASK_H__