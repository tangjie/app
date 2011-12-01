#include "base/synchronization/waitable_event.h"
#include "base/thread/thread_helper.h"
#include "base/time/time.h"
#include "gtest/gtest.h"

using base::WaitableEvent;

namespace {
	void Normal(const base::TimeSpan& sleep_time, WaitableEvent* event) {
		base::ThreadHelper::Sleep(sleep_time.ToMilliseconds());
		event->Signal();
	}
	void SignalMulEvents(const base::TimeSpan& sleep_time, WaitableEvent* event, int count) {
		base::ThreadHelper::Sleep(sleep_time.ToMilliseconds());
		for (int i = 0; i < count; ++i) {
			event[i].Signal();
		}
	}
	void MulEventsSignalOne(const base::TimeSpan& sleep_time, WaitableEvent* event, int count, int signal_index) {
		base::ThreadHelper::Sleep(sleep_time.ToMilliseconds());
		event[signal_index].Signal();
	}
}

TEST(WaitableEvent, ManualBasic) {
	WaitableEvent event(true, false);
	EXPECT_FALSE(event.WasSignaled());
	event.Signal();
	EXPECT_TRUE(event.WasSignaled());
	EXPECT_TRUE(event.WasSignaled());
	event.Reset();
	EXPECT_FALSE(event.WasSignaled());
	event.Signal();
	event.Signal();	//double signal
	EXPECT_TRUE(event.WasSignaled());
	CloseHandle(event.Release());
	EXPECT_EQ(INVALID_HANDLE_VALUE, event.handle());
	event.Signal();
	EXPECT_FALSE(event.WasSignaled());
}

TEST(WaitableEvent, AutoBasic) {
	WaitableEvent event(false, false);
	EXPECT_FALSE(event.WasSignaled());
	event.Signal();
	EXPECT_TRUE(event.WasSignaled());
	EXPECT_FALSE(event.WasSignaled());
	event.Reset();
	EXPECT_FALSE(event.WasSignaled());
	EXPECT_FALSE(event.WasSignaled());
	event.Signal();
	event.Signal();	//double signal
	EXPECT_TRUE(event.WasSignaled());
	EXPECT_FALSE(event.WasSignaled());
}

TEST(WaitableEvent, WaitForSomeTimes) {
	//TODO(oldman): Complete the test case once we complete MakeRunnableMethod and MakeRunnableFunction.
	//See the few functions in anonymous namespace above.
}

