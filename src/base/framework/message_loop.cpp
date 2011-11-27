#include "base/framework/message_loop.h"
#include "base/thread/thread_local.h"

namespace base {
	MessageLoop::MessageLoop(MessageLoopType type) : type_(type), state_(nullptr), next_sequence_num_(0) {
		if (type_ == kDefaultMessageLoop) {
			//TODO(tangjie):create default message pump;
		}else if (type_ == kUIMessageLoop) {
			pump_ = std::shared_ptr<MessagePump>(new UIMessagePump());
		}else if (type_ == kIOMessageLoop) {
			//TODO(tangjie):create io message pump;
		}
		task_observers_ = std::shared_ptr<ObserverList<TaskObserver>>(new ObserverList<TaskObserver>());
		destruction_observers_ = std::shared_ptr<ObserverList<DestructionObserver>>(new ObserverList<DestructionObserver>());
		assert(	internal::LocalStorage<MessageLoop>::GetInstance()->Get() == nullptr);
		//TODO(tangjie): Add log for error.
		internal::LocalStorage<MessageLoop>::GetInstance()->Set(this);
	}

	MessageLoop::~MessageLoop() {
		assert(this == current());
		// Clean up any unprocessed tasks, but take care: deleting a task could
		// result in the addition of more tasks (e.g., via DeleteSoon).  We set a
		// limit on the number of times we will allow a deleted task to generate more
		// tasks.  Normally, we should only pass through this loop once or twice.  If
		// we end up hitting the loop limit, then it is probably due to one task that
		// is being stubborn.  Inspect the queues to see who is left.
		bool did_work;
		for (int i = 0; i < 100; ++i) {
			DeletePendingTasks();
			ReloadWorkQueue();
			// If we end up with empty queues, then break out of the loop.
			did_work = DeletePendingTasks();
			if (!did_work)
				break;
		}
		FOR_EACH_OBSERVER(DestructionObserver, destruction_observers_, PreDestroyCurrentMessageLoop());
		internal::LocalStorage<MessageLoop>::GetInstance()->Set(nullptr);
	}

	MessageLoop* MessageLoop::current() {
		return internal::LocalStorage<MessageLoop>::GetInstance()->Get();
	}

	void MessageLoop::AddDestructionObserver(DestructionObserver *observer) {
		assert(this == current());
		destruction_observers_->AddObserver(observer);
	}

	void MessageLoop::RemoveDestructionObserver(DestructionObserver *observer) {
		assert(this == current());
		destruction_observers_->RemoveObserver(observer);
	}

	void MessageLoop::AddTaskObserver(TaskObserver *observer) {
		assert(this == current());
		task_observers_->AddObserver(observer);
	}

	void MessageLoop::RemoveTaskObserver(TaskObserver *observer) {
		assert(this == current());
		task_observers_->RemoveObserver(observer);
	}

	void MessageLoop::PreProcessTask() {
		FOR_EACH_OBSERVER(TaskObserver, task_observers_, PreProcessTask());
	}

	void MessageLoop::PostPrecessTask() {
		FOR_EACH_OBSERVER(TaskObserver, task_observers_, PostPrecessTask());
	}

	void MessageLoop::Run() {
		assert(this == current());
		AutoRunState auto_state(this);
		RunInternal();
	}

	void MessageLoop::RunInternal() {
		assert(this == current());
		if (type() == kUIMessageLoop && state_->dispatcher_ != nullptr) {
			down_cast<UIMessagePump*>(pump_.get())->RunWithDispatcher(this, state_->dispatcher_);
		}else {
			pump_->Run(this);
		}
	}

	void MessageLoop::RunAllPending() {
		assert(this == current());
		state_->quit_received_ = true;
		RunInternal();
	}

	void MessageLoop::Quit() {
		assert(this == current());
		if (state_ != nullptr) {
			state_->quit_received_ = true;
		}
	}

	void MessageLoop::QuitNow() {
		assert(this == current());
		if (state_ != nullptr) {
			pump_->Quit();
		}
	}

	void MessageLoop::PostTask(Task *task) {
		if (task != nullptr) {
			PendingTask pending_task(task, CalculateDelayedRuntime(0));
			AddToIncomingQueue(pending_task);
		}
	}

	void MessageLoop::PostDelayTask(Task *task, int64_t delay_ms) {
		if (task != nullptr) {
			PendingTask pending_task(task, CalculateDelayedRuntime(delay_ms));
			AddToIncomingQueue(pending_task);
		}
	}

	TimeTicks MessageLoop::CalculateDelayedRuntime(int64_t delay_ms) {
		TimeTicks delayed_run_time;
		if (delay_ms > 0) {
			delayed_run_time = TimeTicks::Now() + TimeSpan::FromMilliseconds(delay_ms);
		}
		return delayed_run_time;
	}

	bool MessageLoop::DeletePendingTasks() {
		//TODO(tangjie): add nestable task process.
		bool did_work = !work_queue_.empty();
		while (!work_queue_.empty()) {
			PendingTask pending_task = work_queue_.front();
			work_queue_.pop();
			if (!pending_task.delayed_run_time_.IsNull()) {
				AddToDelayedQueue(pending_task);
			}
		}
		did_work |= delayed_work_queue_.empty();
		while(!delayed_work_queue_.empty()) {
			delayed_work_queue_.pop();
		}
		return did_work;
	}

	void MessageLoop::AddToIncomingQueue(const PendingTask &task) {
		std::shared_ptr<MessagePump> pump;
		{
			AutoLock lock(incoming_queue_lock_);
			bool was_empty = incoming_queue_.empty();
			incoming_queue_.push(task);
			if (!was_empty) {
				return;
			}
			pump = pump_;
		}
		pump->ScheduleWork();
	}

	void MessageLoop::AddToDelayedQueue(const PendingTask &task) {
		PendingTask pending_task(task);
		pending_task.sequence_num_ = next_sequence_num_++;
		delayed_work_queue_.push(pending_task);
	}

	bool MessageLoop::DoWork() {
		//TODO(tangjie): add nestable task process.
		for (; ;) {
			ReloadWorkQueue();
			if (work_queue_.empty()) {
				break;
			}
			do {
				PendingTask task = work_queue_.front();
				work_queue_.pop();
				if (!task.delayed_run_time_.IsNull()) {
					AddToDelayedQueue(task);
				}else {
					if (DeferOrRunPendingTask(task)) {
						return true;
					}
				}
			}while(!work_queue_.empty());
		}
		return false;
	}

	bool MessageLoop::DoDelayWork(TimeTicks *next_delayed_work_time) {
		//TODO(tangjie): add nestable task process.
		if (delayed_work_queue_.empty()) {
			recent_time_ = *next_delayed_work_time = TimeTicks();
			return false;
		}
		TimeTicks next_time = delayed_work_queue_.top().delayed_run_time_;
		if (next_time > recent_time_) {
			recent_time_ = TimeTicks::Now();
			if (next_time > recent_time_) {
				*next_delayed_work_time = next_time;
				return false;
			}
		}
		PendingTask task = delayed_work_queue_.top();
		delayed_work_queue_.pop();
		if (!delayed_work_queue_.empty()) {
			*next_delayed_work_time = delayed_work_queue_.top().delayed_run_time_;
		}
		return DeferOrRunPendingTask(task);
	}

	bool MessageLoop::DoIdleWork() {
		//TODO(tangjie): add nestable task process.
		if (state_->quit_received_) {
			pump_->Quit();
		}
		return false;
	}

	bool MessageLoop::DeferOrRunPendingTask(const PendingTask& task) {
		//TODO(tangjie): add nestable task process.
		RunTask(task);
		return true;
	}

	bool MessageLoop::RunTask(const PendingTask &task) {
		PendingTask pending_task = task;
		PreProcessTask();
		pending_task.task_->Run();
		PostPrecessTask();
		return true;
	}

	void MessageLoop::ReloadWorkQueue() {
		if (!work_queue_.empty()) {
			return;
		}
		{
			AutoLock lock(incoming_queue_lock_);
			if (incoming_queue_.empty()) {
				return;
			}
			incoming_queue_.Swap(&work_queue_);
			assert(incoming_queue_.empty());
		}
	}

	MessageLoop::AutoRunState::AutoRunState(MessageLoop *loop) : loop_(loop) {
		//TODO(tangjie): add nestable task process.
		prevous_state_ = loop_->state_;
		if (prevous_state_ != nullptr) {
			run_depth_++;
		}else {
			run_depth_ = 1;
		}
		loop_->state_ = this;
		quit_received_ = false;
		dispatcher_ = nullptr;
	}

	MessageLoop::AutoRunState::~AutoRunState() {
		loop_->state_ = prevous_state_;
	}

	MessageLoop::PendingTask::PendingTask(Task *task, TimeTicks delayed_run_time) : task_(task), delayed_run_time_(delayed_run_time) {

	}

	MessageLoop::PendingTask::~PendingTask() {

	}

	bool MessageLoop::PendingTask::operator<(const PendingTask &other) const {
		if (delayed_run_time_ > other.delayed_run_time_) {
			return true;
		}
		if (delayed_run_time_ < other.delayed_run_time_) {
			return false;
		}
		return sequence_num_ > other.sequence_num_;
	}

}