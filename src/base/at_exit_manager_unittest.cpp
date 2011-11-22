#include "base/at_exit_manager.h"

#include "base/memory/casts.h"
#include "gtest/gtest.h"


void foo(void* param) {
	*static_cast<int*>(param) = 1;
}

TEST(AtExitManager, Basic) {
	int i = 0;
	{
		base::AtExitManager manager;
		manager.RegisterCallback(foo, &i);
	}
	EXPECT_EQ(1, i);
}