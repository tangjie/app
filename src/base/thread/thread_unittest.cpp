#include "base/thread/thread.h"
#include "base/test/test_with_exit_manager.h"

using base::Thread;

TEST_WITH_EM(Thread, Basic) {
	Thread thread;
	EXPECT_FALSE(thread.was_started());
	EXPECT_FALSE(thread.IsRunning());
	thread.Start();
	EXPECT_TRUE(thread.was_started());
	EXPECT_TRUE(thread.IsRunning());
}