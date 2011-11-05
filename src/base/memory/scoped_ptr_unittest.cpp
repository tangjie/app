/*
 * scoped_ptr unit test
 */

#include "base/memory/scoped_ptr.h"
#include "gtest/gtest.h"

namespace {
	class ConDecLogger {
	public:
		ConDecLogger() : ptr_(NULL) {
		}

		explicit ConDecLogger(int *ptr) : ptr_(ptr) {
			++*ptr_;
		}

		~ConDecLogger() {
			--*ptr_;
		}

		int GetInput(int input) {
			return input;
		}

	private:
		int *ptr_;
	};
}

TEST(ScopedPtrTest, PointerFeature) {
	int num = 0;
	{
		scoped_ptr<ConDecLogger> scoper(new ConDecLogger(&num));
		EXPECT_EQ(1, num);
		EXPECT_TRUE(scoper.get() != nullptr);
		EXPECT_EQ(10, scoper.get()->GetInput(10));
		EXPECT_EQ(10, (*scoper).GetInput(10));
		EXPECT_EQ(10, scoper->GetInput(10));
	}
	EXPECT_EQ(0, num);
}

TEST(ScopedPtrTest, ReleaseAndReset) {
	int num = 0;
	{
		scoped_ptr<ConDecLogger> scoper(new ConDecLogger(&num));
		EXPECT_EQ(1, num);
		EXPECT_TRUE(scoper.get() != nullptr);
		scoper.reset(new ConDecLogger(&num));
		EXPECT_EQ(1, num);
		EXPECT_TRUE(scoper.get() != nullptr);
		scoper.reset();
		EXPECT_EQ(0, num);
		EXPECT_FALSE(scoper.get() != nullptr);
		scoper.reset(new ConDecLogger(&num));
		EXPECT_EQ(1, num);
		EXPECT_TRUE(scoper.get() != nullptr);
		ConDecLogger* logger = scoper.release();
		EXPECT_EQ(1, num);
		EXPECT_TRUE(scoper.get() == nullptr);
		EXPECT_TRUE(scoper.release() == nullptr);
		scoper.reset(logger);
		EXPECT_EQ(1, num);
		EXPECT_FALSE(scoper.get() == nullptr);
	}
	EXPECT_EQ(0, num);
}

TEST(ScopedPtrTest, SwapAndCompare) {
	int num = 0;
	{
		scoped_ptr<ConDecLogger> scoper1;
		scoped_ptr<ConDecLogger> scoper2;
		EXPECT_EQ(scoper1.get(), scoper2.get());
		EXPECT_TRUE(scoper1.get() == scoper2);
		EXPECT_FALSE(scoper1 != scoper2.get());
		ConDecLogger* logger = new ConDecLogger(&num);
		scoper1.reset(logger);
		EXPECT_EQ(1, num);
		EXPECT_TRUE(scoper1 != scoper2.get());
		EXPECT_TRUE(scoper1 == scoper1.get());
		EXPECT_TRUE(scoper1.get() == logger);
		EXPECT_TRUE(scoper1 == logger);
		scoper2.swap(scoper1);
		EXPECT_EQ(1, num);
		EXPECT_TRUE(scoper1 != scoper2.get());
		EXPECT_TRUE(scoper1 == scoper1.get());
		EXPECT_FALSE(scoper1.get() == logger);
		EXPECT_TRUE(scoper2 == logger);
		EXPECT_TRUE(scoper1 == nullptr);
		swap(scoper1,scoper2);
		EXPECT_EQ(1, num);
		EXPECT_TRUE(scoper1 != scoper2.get());
		EXPECT_TRUE(scoper1 == scoper1.get());
		EXPECT_TRUE(scoper1.get() == logger);
		EXPECT_TRUE(scoper1 == logger);
	}
	EXPECT_EQ(0, num);
}
