#include "base/thread/thread.h"
#include "gtest/gtest.h"

using base::Thread;

TEST(Thread, Basic) {
	Thread thread;
	EXPECT_FALSE(thread.was_started());
	EXPECT_FALSE(thread.IsRunning());
	thread.Start();
	EXPECT_TRUE(thread.was_started());
	EXPECT_TRUE(thread.IsRunning());
}