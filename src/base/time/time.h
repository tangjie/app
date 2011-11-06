/*
 * This file defined TimeSpan Time and TimeTicks
 */

#ifndef BASE_TIME_TIME_H__
#define BASE_TIME_TIME_H__

#include <time.h>
#include <Windows.h>
#include "base/base_types.h"

namespace base {

	class Time;
	class TimeTicks;

	namespace UnitConversion{
		static const int64_t kMillisecondsPerSecond = 1000;
		static const int64_t kMicrosecondsPerMillisecond = 1000;
		static const int64_t kMicrosecondsPerSecond = kMillisecondsPerSecond * kMicrosecondsPerMillisecond;
		static const int64_t kMicrosecondsPerMinute = kMicrosecondsPerSecond * 60;
		static const int64_t kMicrosecondsPerHour = kMicrosecondsPerMinute * 60;
		static const int64_t kMicrosecondsPerDay = kMicrosecondsPerHour * 24;
		static const int64_t kMicrosecondsPerWeek = kMicrosecondsPerDay * 7;
		static const int64_t kNanosecondsPerMicrosecond = 1000;
		static const int64_t kNanosecondsPerSecond = kMicrosecondsPerSecond * kNanosecondsPerMicrosecond;
	}
	class TimeSpan {
	public:
		TimeSpan() : span_(0) {
		}

		static TimeSpan FromDays(int64_t days);
		static TimeSpan FromHours(int64_t hours);
		static TimeSpan FromMinutes(int64_t minutes);
		static TimeSpan FromSeconds(int64_t seconds);
		static TimeSpan FromMilliseconds(int64_t milliseconds);
		static TimeSpan FromMicroseconds(int64_t microseconds);
		int64_t ToInternalValue() const;
		int ToDays() const;
		int ToHours() const;
		int ToMinutes() const;
		double ToSecondsF() const;
		int64_t ToSeconds() const;
		double ToMillisecondsF() const;
		int64_t ToMilliseconds() const;
		int64_t ToMillisecondsRoundedsUp() const;
		int64_t ToMicroseconds() const;
		TimeSpan& operator=(TimeSpan other);
		TimeSpan operator+(TimeSpan other) const;
		TimeSpan operator-(TimeSpan other) const;
		TimeSpan& operator+=(TimeSpan other);
		TimeSpan& operator-=(TimeSpan other);
		TimeSpan operator-() const;
		TimeSpan operator*(int64_t a) const;
		TimeSpan operator/(int64_t a) const;
		TimeSpan& operator*=(int64_t a);
		TimeSpan& operator/=(int64_t a);
		Time operator+(Time time) const;
		TimeTicks operator+(TimeTicks ticks) const;
		bool operator==(TimeSpan other) const;
		bool operator!=(TimeSpan other) const;
		bool operator<(TimeSpan other) const;
		bool operator<=(TimeSpan other) const;
		bool operator>(TimeSpan other) const;
		bool operator>=(TimeSpan other) const;
	private:
		int64_t span_;
		explicit TimeSpan(int64_t span) : span_(span) {
		}

		friend TimeSpan operator*(int64_t a, TimeSpan other);
		friend class Time;
		friend class TimeTicks;
	};

	class Time {
	public:
		struct TimeStruct {
			int year_;
			int month_;
			int day_of_week_;
			int day_of_month_;
			int hour_;
			int minute_;
			int second_;
			int millisecond_;

			TimeStruct()
				: year_(0), month_(0), day_of_week_(0), day_of_month_(0), hour_(0), 
				minute_(0), second_(0), millisecond_(0){

			}
			bool IsValidValues() const;
			int year() const {
				return year_;
			}

			int month() const {
				return month_;
			}

			int day_of_month() const {
				return day_of_month_;
			}

			int day_of_week() const {
				return day_of_week_;
			}

			int hour() const {
				return hour_;
			}

			int minute() const {
				return minute_;
			}

			int second() const {
				return second_;
			}

			int millisecond() const {
				return millisecond_;
			}
		};
		Time() : microseconds_(0) {
		}

		explicit Time(int64_t microseconds) : microseconds_(microseconds) {
		}

		Time(bool is_local, const TimeStruct &ts);
		Time(bool is_local, int year, int month, int day, 
			int hour, int minute, int second, int millisecond = 0);
		int64_t ToInternalValue() const {
			return microseconds_;
		};

		bool IsNull() const {
			return microseconds_ == 0;
		}

		static Time Now();
		static Time FromTimeStruct(bool is_local, const TimeStruct &ts);
		TimeStruct ToTimeStruct(bool is_local) const;
		static Time FromTimeT(time_t tt);
		time_t ToTimeT() const;
		static Time FromFileTime(FILETIME ft);
		FILETIME ToFileTime() const;
		Time& operator=(Time other);
		TimeSpan operator-(Time other) const;
		Time& operator+=(TimeSpan span);
		Time& operator-=(TimeSpan span);
		Time operator+(TimeSpan span) const;
		Time operator-(TimeSpan span) const;
		bool operator==(Time other) const;
		bool operator!=(Time other) const;
		bool operator<(Time other) const;
		bool operator<=(Time other) const;
		bool operator>(Time other) const;
		bool operator>=(Time other) const;
	private:
		friend class TimeSpan;
		int64_t microseconds_;
		static const int64_t kTimeTToMicrosecondsOffset;
	};

	class TimeTicks {
	public:
		TimeTicks() : ticks_(0) {
		}

		explicit TimeTicks(int64_t ticks) : ticks_(ticks) {
		}
		static TimeTicks Now();
		static TimeTicks HightResolutionNow();
		bool IsNull() const {
			return ticks_ == 0;
		}

		int64_t ToInternalValue() const {
			return ticks_;
		}

		TimeTicks& operator=(TimeTicks other);
		TimeSpan operator-(TimeTicks other) const;
		TimeTicks operator+(TimeSpan span) const;
		TimeTicks operator-(TimeSpan span) const;
		TimeTicks& operator+=(TimeSpan span);
		TimeTicks& operator-=(TimeSpan span);
		bool operator==(TimeTicks other) const;
		bool operator!=(TimeTicks other) const;
		bool operator<(TimeTicks other) const;
		bool operator<=(TimeTicks other) const;
		bool operator>(TimeTicks other) const;
		bool operator>=(TimeTicks other) const;
	protected:
		int64_t ticks_;
		friend class TimeSpan;
	};
}

#endif
