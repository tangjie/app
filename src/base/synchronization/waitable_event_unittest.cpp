#include "base/synchronization/waitable_event.h"
#include "gtest/gtest.h"

using base::WaitableEvent;

TEST(WaitableEvent, Basic) {
	WaitableEvent event(true, false);
	EXPECT_TRUE(false == event.WasSignaled());
	event.Signal();
	EXPECT_TRUE(true == event.WasSignaled());
	event.Reset();
	EXPECT_TRUE(false == event.WasSignaled());
	CloseHandle(event.Release());
	EXPECT_EQ(INVALID_HANDLE_VALUE, event.handle());
	event.Signal();
	EXPECT_TRUE(false == event.WasSignaled());
}
