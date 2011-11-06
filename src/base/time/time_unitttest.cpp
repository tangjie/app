#include "base/time/time.h"
#include "gtest/gtest.h"

using base::Time;
using base::TimeSpan;
using base::TimeTicks;

namespace {
	void WaitForTinyTicks() {
		Sleep(100);
	}
}

TEST(TimeSpan, Empty) {
	EXPECT_EQ(TimeSpan(), TimeSpan::FromDays(0));
	EXPECT_EQ(TimeSpan(), TimeSpan::FromHours(0));
	EXPECT_EQ(TimeSpan(), TimeSpan::FromMinutes(0));
	EXPECT_EQ(TimeSpan(), TimeSpan::FromSeconds(0));
	EXPECT_EQ(TimeSpan(), TimeSpan::FromMilliseconds(0));
	EXPECT_EQ(TimeSpan(), TimeSpan::FromMicroseconds(0));
	EXPECT_EQ(0, TimeSpan().ToInternalValue());
	EXPECT_EQ(0, TimeSpan().ToDays());
	EXPECT_EQ(0, TimeSpan().ToHours());
	EXPECT_EQ(0, TimeSpan().ToMinutes());
	EXPECT_EQ(0, TimeSpan().ToSeconds());
	EXPECT_EQ(0, TimeSpan().ToMilliseconds());
	EXPECT_EQ(0, TimeSpan().ToMillisecondsRoundedsUp());
	EXPECT_EQ(0, TimeSpan().ToMicroseconds());
}

TEST(TimeSpan, Conversion) {
	EXPECT_EQ(TimeSpan::FromDays(1), TimeSpan::FromHours(24));
	EXPECT_EQ(TimeSpan::FromHours(1), TimeSpan::FromMinutes(60));
	EXPECT_EQ(TimeSpan::FromMinutes(1), TimeSpan::FromSeconds(60));
	EXPECT_EQ(TimeSpan::FromSeconds(1), TimeSpan::FromMilliseconds(1000));
	EXPECT_EQ(TimeSpan::FromMilliseconds(1), TimeSpan::FromMicroseconds(1000));
	EXPECT_EQ(1, TimeSpan::FromDays(1).ToDays());
	EXPECT_EQ(1, TimeSpan::FromHours(1).ToHours());
	EXPECT_EQ(1, TimeSpan::FromMinutes(1).ToMinutes());
	EXPECT_EQ(1, TimeSpan::FromSeconds(1).ToSeconds());
	EXPECT_EQ(1, TimeSpan::FromSeconds(1).ToSecondsF());
	EXPECT_EQ(1, TimeSpan::FromMilliseconds(1).ToMilliseconds());
}

TEST(TimeSpan, FuzzyOperation) {
	EXPECT_EQ(TimeSpan::FromDays(1), TimeSpan::FromDays(1) + TimeSpan());
	EXPECT_EQ(TimeSpan::FromDays(3), TimeSpan::FromDays(1) + TimeSpan::FromDays(2));
	EXPECT_EQ(TimeSpan::FromSeconds(1), TimeSpan::FromSeconds(42) - TimeSpan::FromSeconds(41));
	EXPECT_EQ(TimeSpan::FromDays(42), TimeSpan::FromDays(6) * 7);
	EXPECT_EQ(TimeSpan::FromDays(42), TimeSpan::FromDays(3) * 14);
	EXPECT_EQ(TimeSpan::FromDays(42), 42 * TimeSpan::FromDays(1));
	EXPECT_EQ(TimeSpan::FromDays(7), TimeSpan::FromDays(42) / 6);
	EXPECT_EQ(TimeSpan::FromDays(14), TimeSpan::FromDays(42) / 3);
	EXPECT_GE(TimeSpan::FromMicroseconds(42).ToMillisecondsRoundedsUp(), TimeSpan::FromMicroseconds(999).ToMilliseconds());
	EXPECT_EQ(1, TimeSpan::FromMicroseconds(1000).ToMillisecondsRoundedsUp());
	EXPECT_EQ(2, TimeSpan::FromMicroseconds(1001).ToMillisecondsRoundedsUp());
}

TEST(TimeTicks, FuzzyOperation) {
	EXPECT_TRUE(TimeTicks().IsNull());
	EXPECT_FALSE((TimeTicks(42)-TimeSpan::FromMicroseconds(1)).IsNull());
	EXPECT_EQ(TimeTicks(21), TimeTicks(42) - TimeSpan::FromMicroseconds(21));
	EXPECT_EQ(42, (TimeTicks() + (TimeSpan::FromMicroseconds(21) * 2)).ToInternalValue());
}

TEST(Time, Basic) {
	EXPECT_TRUE(Time().IsNull());
	Time t = Time::Now();
	Time::TimeStruct tm_detail = t.ToTimeStruct(true);
	Time::TimeStruct tm_detail_2 = t.ToTimeStruct(false);
	//Since we always live in Eastern 8th area (or maybe not?), so our expectation is as follows.
	EXPECT_EQ(Time::FromTimeStruct(true, tm_detail), Time::FromTimeStruct(false, tm_detail_2));
	EXPECT_NE(Time::FromTimeStruct(false, tm_detail), Time::FromTimeStruct(true, tm_detail_2));

	tm t_detail;
	time_t t2 = t.ToTimeT();
	localtime_s(&t_detail, &t2);
	EXPECT_EQ(t_detail.tm_year + 1900, tm_detail.year());
	EXPECT_EQ(t_detail.tm_mon + 1, tm_detail.month());
	EXPECT_EQ(t_detail.tm_mday, tm_detail.day_of_month());
	EXPECT_EQ(t_detail.tm_hour, tm_detail.hour());
	EXPECT_EQ(t_detail.tm_min, tm_detail.minute());
	EXPECT_EQ(t_detail.tm_sec, tm_detail.second());
}

TEST(Time, Conversion) {
	TimeSpan span = TimeSpan::FromDays(1);
	Time t = Time::Now();
	EXPECT_EQ(t, t - span + span);
	EXPECT_EQ(t, span + t - span);
	EXPECT_EQ(t, Time::FromTimeStruct(true, t.ToTimeStruct(true)));
	EXPECT_EQ(t, Time::FromFileTime(t.ToFileTime()));
}

#define TIME_NOW_TEST(Class, Fun) \
	do { \
		Class t1 = Fun(); \
		WaitForTinyTicks(); \
		Class t2 = Fun(); \
		EXPECT_GT(t2, t1); \
	} while (0)
		

TEST(TimeNow, Basic) {
	TIME_NOW_TEST(TimeTicks, TimeTicks::Now);
	TIME_NOW_TEST(TimeTicks, TimeTicks::HightResolutionNow);
	TIME_NOW_TEST(Time, Time::Now);
}