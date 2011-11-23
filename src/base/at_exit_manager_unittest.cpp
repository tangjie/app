#include "base/at_exit_manager.h"

#include "base/memory/casts.h"
#include "gtest/gtest.h"


void foo(void* param) {
	*static_cast<int*>(param) = 1;
}

class Calculateor {
public:
	static void Increase(void* origin_value) {
		++(*static_cast<int*>(origin_value));
	}

	static void Decrease(void* origin_value) {
		--(*static_cast<int*>(origin_value));
	}
};

struct OrderParams {
	int total_;
	int put_order_;
};

int g_exec_order = 0;

class ExecOrderHelper {
public:
	static void Exec(void * params) {
		OrderParams* order_params = static_cast<OrderParams*>(params);
		g_exec_order++;
		EXPECT_TRUE(g_exec_order == order_params->total_ - order_params->put_order_);
	}
};

TEST(AtExitManager, Basic) {
	int i = 0;
	{
		base::AtExitManager manager;
		manager.RegisterCallback(foo, &i);
	}
	EXPECT_EQ(1, i);
	int origin_num = 0;
	{
		base::AtExitManager manager;
		for (int k = 0; k < 10; k++ ) {
			base::AtExitManager::RegisterCallback(Calculateor::Increase, &origin_num);
			EXPECT_EQ(0, origin_num);
		}
		base::AtExitManager::ProcessCallbacksNow();
		EXPECT_EQ(10, origin_num);
		for (int k = 0; k < 10; k++ ) {
			base::AtExitManager::RegisterCallback(Calculateor::Decrease, &origin_num);
			EXPECT_EQ(10, origin_num);
		}
		base::AtExitManager::ProcessCallbacksNow();
		EXPECT_EQ(0, origin_num);
		for (int k = 0; k < 10; k++ ) {
			base::AtExitManager::RegisterCallback(Calculateor::Decrease, &origin_num);
			base::AtExitManager::RegisterCallback(Calculateor::Increase, &origin_num);
		}
		EXPECT_EQ(0, origin_num);
	}
	EXPECT_EQ(0, origin_num);
}

TEST(AtExitManager, ExecOrder) {
	{
		const int kTotal = 10;
		OrderParams params[kTotal];
		base::AtExitManager manager;
		for (int i = 0; i < 10; i++) {
			params[i].total_ = kTotal;
			params[i].put_order_ = i;
			base::AtExitManager::RegisterCallback(ExecOrderHelper::Exec, &params[i]);
		}
	}
}

