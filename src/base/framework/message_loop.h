/*
 * The message loop mechanism reference from google chromium project.
 */
#ifndef BASE_FRAMEWORK_MESSAGE_LOOP_H__
#define BASE_FRAMEWORK_MESSAGE_LOOP_H__

#include <queue>
#include "base/base_types.h"
#include "base/framework/observer_list.h"
#include "base/framework/task.h"
#include "base/framework/message_pump_default.h"
#include "base/framework/message_pump_ui.h"
#include "base/synchronization/lock.h"
#include "base/util/noncopyable.h"

namespace base {
	class UIMessageLoop;
	class IOMessageLoop;
	typedef UIMessagePump::Dispatcher Dispatcher;
	class MessageLoop : public MessagePump::Delegate, public noncopyable {
	public:
		enum MessageLoopType {
			kDefaultMessageLoop,
			kUIMessageLoop,
			kIOMessageLoop
		};

		explicit MessageLoop(MessageLoopType type = kDefaultMessageLoop);
		virtual ~MessageLoop();
		static MessageLoop* current();
		MessageLoopType type() const {
			return type_;
		}

		class DestructionObserver {
		public:
			virtual void PreDestroyCurrentMessageLoop();
		protected:
			virtual ~DestructionObserver();
		};

		void AddDestructionObserver(DestructionObserver *observer);
		void RemoveDestructionObserver(DestructionObserver *observer);

		class TaskObserver {
		public:
			virtual void PreProcessTask();
			virtual void PostPrecessTask();
		protected:
			virtual ~TaskObserver();
		};
		void AddTaskObserver(TaskObserver *observer);
		void RemoveTaskObserver(TaskObserver *observer);
		void PreProcessTask();
		void PostPrecessTask();
		void Run();
		void RunAllPending();
		void RunInternal();
		void Quit();
		void QuitNow();
		void PostTask(std::shared_ptr<Task> task);
		void PostDelayTask(std::shared_ptr<Task> task, int64_t delay_ms);
	protected:
		struct RunState {
			int run_depth_;
			bool quit_received_;
			Dispatcher *dispatcher_;
		};

		class AutoRunState : public RunState {
		public:
			explicit AutoRunState(MessageLoop *loop);
			~AutoRunState();
		private:
			MessageLoop* loop_;
			RunState *prevous_state_;
		};

		struct PendingTask {
			PendingTask(std::shared_ptr<Task> task, TimeTicks delayed_run_time);
			~PendingTask();
			bool operator<(const PendingTask &other) const;
			std::shared_ptr<Task> task_;
			int sequence_num_;
			TimeTicks delayed_run_time_;
		};

		class TaskQueue : public std::queue<PendingTask> {
		public:
			void Swap(TaskQueue *queue) {
				c.swap(queue->c);
			}
		};

		typedef std::priority_queue<PendingTask> DelayedTaskQueue;
		TimeTicks CalculateDelayedRuntime(int64_t delay_ms);
		virtual bool DoWork();
		virtual bool DoDelayWork(TimeTicks *next_delayed_work_time);
		virtual bool DoIdleWork();
		bool DeletePendingTasks();
		void AddToIncomingQueue(const PendingTask &task);
		void AddToDelayedQueue(const PendingTask &task);
		void ReloadWorkQueue();
		bool DeferOrRunPendingTask(const PendingTask& task);
		bool RunTask(const PendingTask &task);
	protected:
		MessageLoopType type_;
		RunState *state_;
		std::shared_ptr<MessagePump> pump_;
		std::shared_ptr<ObserverList<DestructionObserver>> destruction_observers_;
		std::shared_ptr<ObserverList<TaskObserver>> task_observers_;
		TaskQueue incoming_queue_;
		TaskQueue work_queue_;
		DelayedTaskQueue delayed_work_queue_;
		int next_sequence_num_;
		TimeTicks recent_time_;
		LockImpl incoming_queue_lock_;
	};

	class UIMessageLoop : public MessageLoop {
	public:
		typedef UIMessagePump::UIObserver UIObserver;
		UIMessageLoop() : MessageLoop(kUIMessageLoop) {

		}

		static UIMessageLoop* current() {
			return down_cast<UIMessageLoop*>(MessageLoop::current());
		}

		void AddUIObserver(UIObserver *observer) {
			GetPump()->AddUIObserver(observer);
		}
		void RemoveUIObserver(UIObserver *observer) {
			GetPump()->RemoveUIObserver(observer);
		}
	protected:
		UIMessagePump* GetPump() {
			return down_cast<UIMessagePump*>(pump_.get());
		}
	};
}

#endif //BASE_FRAMEWORK_MESSAGE_LOOP_H__