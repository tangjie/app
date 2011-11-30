#include "base/thread/thread.h"

namespace base {

	struct Thread::StartupData{
		const Thread::Options &options_;
		WaitableEvent event_;
		explicit StartupData(const Thread::Options &options) : options_(options), event_(false, false) {

		}
	};

	Thread::Thread() : thread_id_(kInvalidThreadId), thread_handle_(nullptr), stopping_(false), was_started_(false), startup_data_(nullptr) {
	}

	Thread::~Thread() {
		Stop();
	}

	bool Thread::Start() {
		return StartWithOptions(Options());
	}

	bool Thread::StartWithOptions(const Options &options) {
		StartupData startup_data(options);
		startup_data_ = &startup_data;
		if (!ThreadHelper::Create(MakeRunnableMethod(this, &Thread::ThreadMain), &thread_handle_)) {
			assert(false);
			// TODO(tangjie): add log for create thread failed!
			startup_data_ = nullptr;
			return false;
		}
		startup_data.event_.Wait();
		startup_data_ = nullptr;
		was_started_ = true;
		assert(message_loop_ != false);
		return true;
	}

	void Thread::ThreadMain(){
		{
			assert(startup_data_ != nullptr);
			//note: we can only create message loop here because of the tls feature.
			MessageLoop message_loop(startup_data_->options_.message_loop_type_);
			message_loop_ = &message_loop;
			thread_id_ = ThreadHelper::CurrentId();
			SetUp();
			// wait message loop to be created.
			startup_data_->event_.Signal();
			//from now on you can never use startup data.
			Run(message_loop_);
			TearDown();
			message_loop_ = nullptr;
		}
		thread_id_ = kInvalidThreadId;
	}

	void Thread::Stop() {
		if (!was_started()) {
			return;
		}
		StopInternal();
		ThreadHelper::Join(thread_handle_);
		assert(!message_loop_);
		was_started_ = false;
		stopping_ = false;
	}

	void Thread::StopInternal() {
		if (stopping_ || message_loop_ == nullptr) {
			return;
		}
		stopping_ = true;
		message_loop_->PostTask(MakeRunnableMethod(this, &Thread::Quit));
	}

	void Thread::Terminate() {
		if (thread_handle_ !=nullptr) {
			TerminateThread(thread_handle_, 0);
			CloseHandle(thread_handle_);
			thread_handle_ = nullptr;
			thread_id_ = kInvalidThreadId;
		}
		stopping_ = false;
		was_started_ = false;
	}

	bool Thread::Suspend() {
		if (was_started() && thread_handle_ != nullptr) {
			SuspendThread(thread_handle_);
			return true;
		}
		return false;
	}

	bool Thread::Resume() {
		if (was_started() && thread_handle_ != nullptr) {
			ResumeThread(thread_handle_);
			return true;
		}
		return false;
	}

	ThreadId Thread::thread_id() const {
		return thread_id_;
	}

	ThreadHandle Thread::thread_handle() const {
		return thread_handle_;
	}

	void Thread::Quit() {
		MessageLoop::current()->Quit();
	}
}