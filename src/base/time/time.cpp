/*
 * This file implement a time class depend on windows
 */

#include "base/time/time.h"
#include <MMSystem.h>
#include <stdlib.h>

#pragma comment(lib, "winmm.lib")

namespace base {

	namespace time_helper {
		inline bool IsInRange(int value, int low, int hight) {
			return low <= value && value <= hight;
		}

		template<class Dest, class Src>
		inline Dest bit_cast(const Src &src) {
			typedef char VerifySizesAreEqual [sizeof(Dest) == sizeof(Src) ? 1 : -1];
			Dest dest;
			memcpy(&dest, &src, sizeof(dest));
			return dest;
		}

		int64_t FileTimeToMicroseconds(FILETIME ft) {
			// why do bit_cast ? why not divide by 10 directly
			return bit_cast<int64_t, FILETIME>(ft) / 10;
		}

		int64_t CurrentTimeToMicroseconds() {
			FILETIME ft;
			::GetSystemTimeAsFileTime(&ft);
			return FileTimeToMicroseconds(ft);
		}

		void MicrosecondsToFileTime(int64_t microseconds, FILETIME *ft) {
			if (microseconds < 0) {
				return ;
			}
			*ft = bit_cast<FILETIME, int64_t>(microseconds * 10);
		}

		DWORD last_seen_now = 0;
		int64_t rollover_ms = 0;

		TimeSpan RolloverProtectedNow() {
			//TODO: add lock for multi-thread operate
			DWORD now = timeGetTime();
			if (now < last_seen_now) {
				rollover_ms += 0x100000000LL;
			}
			last_seen_now = now;
			return TimeSpan::FromMilliseconds(now + rollover_ms);
		}

		class HighResolutionNowSingleton {
		public:
			static HighResolutionNowSingleton* GetInstance() {
				static HighResolutionNowSingleton instance;
				return &instance;
			}

			bool IsUsingHightResolutionClock() {
				return ticks_per_microsecond_ != 0.0;
			}

			void DisableHightResolutionClock() {
				ticks_per_microsecond_ = 0.0;
			}

			TimeSpan Now() {
				if (IsUsingHightResolutionClock()) {
					return TimeSpan::FromMicroseconds(UnreliableNow());
				}
				return RolloverProtectedNow();
			}

			int64_t GetQPCDriftMicroseconds() {
				if (!IsUsingHightResolutionClock()) {
					return 0;
				}
				return abs((UnreliableNow() - ReliableNow()) - skew_);
			}

		private:
			HighResolutionNowSingleton() : ticks_per_microsecond_(0.0), skew_(0) {
				InitializeClock();
				//TODO: hardware dependent detect
			}

			void InitializeClock() {
				LARGE_INTEGER ticks_per_microsecond = {{0}};
				if (!QueryPerformanceFrequency(&ticks_per_microsecond)) {
					return;
				}
				ticks_per_microsecond_ = static_cast<float>(ticks_per_microsecond.QuadPart);
				skew_ = UnreliableNow() - ReliableNow();
			}

			int64_t UnreliableNow() {
				LARGE_INTEGER now;
				QueryPerformanceCounter(&now);
				return static_cast<int64_t>(now.QuadPart / ticks_per_microsecond_);
			}

			int64_t ReliableNow() {
				return RolloverProtectedNow().ToMicroseconds();
			}

			float ticks_per_microsecond_;
			int64_t skew_;
		};
	}
	//TimeSpan
	TimeSpan TimeSpan::FromDays(int64_t days) {
		return TimeSpan(days * UnitConversion::kMicrosecondsPerDay);
	}

	TimeSpan TimeSpan::FromHours(int64_t hours) {
		return TimeSpan(hours * UnitConversion::kMicrosecondsPerHour);
	}

	TimeSpan TimeSpan::FromMinutes(int64_t minutes) {
		return TimeSpan(minutes * UnitConversion::kMicrosecondsPerMinute);
	}

	TimeSpan TimeSpan::FromSeconds(int64_t seconds) {
		return TimeSpan(seconds * UnitConversion::kMicrosecondsPerSecond);
	}

	TimeSpan TimeSpan::FromMilliseconds(int64_t milliseconds) {
		return TimeSpan(milliseconds * UnitConversion::kMicrosecondsPerMillisecond);
	}

	TimeSpan TimeSpan::FromMicroseconds(int64_t microseconds) {
		return TimeSpan(microseconds);
	}

	int64_t TimeSpan::ToInternalValue() {
		return span_;
	}

	int TimeSpan::ToDays() const {
		return static_cast<int>(span_ / UnitConversion::kMicrosecondsPerDay);
	}

	int TimeSpan::ToHours() const {
		return static_cast<int>(span_ / UnitConversion::kMicrosecondsPerHour);
	}

	int TimeSpan::ToMinutes() const {
		return static_cast<int>(span_ / UnitConversion::kMicrosecondsPerMinute);
	}

	double TimeSpan::ToSecondsF() const {
		return static_cast<double>(span_) / UnitConversion::kMicrosecondsPerSecond;
	}

	int64_t TimeSpan::ToSeconds() const {
		return span_ / UnitConversion::kMicrosecondsPerSecond;
	}

	double TimeSpan::ToMillisecondsF() const {
		return static_cast<double>(span_) / UnitConversion::kMicrosecondsPerMillisecond;
	}

	int64_t TimeSpan::ToMilliseconds() const {
		return span_ / UnitConversion::kMicrosecondsPerMillisecond;
	}

	int64_t TimeSpan::ToMillisecondsRoundedsUp() const {
		return (span_ + UnitConversion::kMicrosecondsPerMillisecond - 1) / UnitConversion::kMicrosecondsPerMillisecond;
	}

	int64_t TimeSpan::ToMicroseconds() const {
		return span_;
	}

	TimeSpan& TimeSpan::operator=(const TimeSpan &other) {
		span_ = other.span_;
		return *this;
	}

	TimeSpan TimeSpan::operator+(TimeSpan other) {
		return TimeSpan(span_ + other.span_);
	}

	TimeSpan TimeSpan::operator-(TimeSpan other) {
		return TimeSpan(span_ - other.span_);
	}

	TimeSpan& TimeSpan::operator+=(TimeSpan other) {
		span_ += other.span_;
		return *this;
	}

	TimeSpan& TimeSpan::operator-=(TimeSpan other) {
		span_ -= other.span_;
		return *this;
	}

	TimeSpan TimeSpan::operator-() {
		return TimeSpan(-span_);
	}

	TimeSpan TimeSpan::operator*(int64_t a) {
		return TimeSpan(a * span_);
	}

	TimeSpan TimeSpan::operator/(int64_t a ) {
		return TimeSpan(span_ / a);
	}

	TimeSpan& TimeSpan::operator*=(int64_t a) {
		span_ *= a;
		return *this;
	}

	TimeSpan& TimeSpan::operator/=(int64_t a) {
		span_ /= a;
		return *this;
	}

	Time TimeSpan::operator+(Time time) {
		return Time(span_ + time.microseconds_);
	}

	TimeTicks TimeSpan::operator+(TimeTicks ticks) {
		return TimeTicks(span_ + ticks.ticks_);
	}

	bool TimeSpan::operator==(TimeSpan other) {
		return span_ == other.span_;
	}

	bool TimeSpan::operator!=(TimeSpan other) {
		return span_ != other.span_;
	}

	bool TimeSpan::operator<(TimeSpan other) {
		return span_ < other.span_;
	}

	bool TimeSpan::operator<=(TimeSpan other) {
		return span_ <= other.span_;
	}

	bool TimeSpan::operator>(TimeSpan other) {
		return span_ > other.span_;
	}

	bool TimeSpan::operator>=(TimeSpan other) {
		return span_ > other.span_;
	}

	TimeSpan operator*(int64_t a, TimeSpan other) {
		return TimeSpan(a * other.span_);
	}

	//Time
	const int64_t Time::kTimeTToMicrosecondsOffset = 11644473600000000LL;

	Time::Time(bool is_local, TimeStruct &ts) {
		microseconds_ = FromTimeStruct(is_local,ts).microseconds_;
	}

	Time::Time(bool is_local, int year, int month, int day, int hour, int minute, int second, int millisecond /* = 0 */) {
		Time::TimeStruct ts;
		ts.year_ = year;
		ts.month_ = month;
		ts.day_of_month_ = day;
		ts.day_of_week_ = 0;
		ts.hour_ = hour;
		ts.minute_ = minute;
		ts.second_ = second;
		ts.millisecond_ = millisecond;
		microseconds_ = FromTimeStruct(is_local, ts).microseconds_;
	}

	Time Time::FromTimeT(time_t tt) {
		if (tt == 0) {
			return Time();
		}
		return Time((tt * UnitConversion::kMicrosecondsPerSecond) + kTimeTToMicrosecondsOffset);
	}

	time_t Time::ToTimeT() const {
		if (microseconds_ == 0) {
			return 0;
		}
		return (microseconds_ - kTimeTToMicrosecondsOffset) / UnitConversion::kMicrosecondsPerSecond;
	}

	bool Time::TimeStruct::IsvalidValues() const {
		using namespace base::time_helper;
		return IsInRange(month_, 1, 12) &&
				IsInRange(day_of_week_, 0, 6) &&
				IsInRange(day_of_month_, 1, 31) &&
				IsInRange(hour_, 0, 23) &&
				IsInRange(minute_, 0, 59) &&
				IsInRange(second_, 0, 59) &&
				IsInRange(millisecond_, 0, 999);
	}

	Time Time::Now() {
		int64_t microseconds = time_helper::CurrentTimeToMicroseconds();
		return Time(microseconds);
	}

	Time Time::FromTimeStruct(bool is_local, TimeStruct &ts) {
		SYSTEMTIME st;
		st.wYear			= ts.year();
		st.wMonth		 = ts.month();
		st.wDayOfWeek = ts.day_of_week();
		st.wDay			 = ts.day_of_month();
		st.wHour			= ts.hour();
		st.wMinute		= ts.minute();
		st.wSecond		= ts.second();
		st.wMilliseconds = ts.millisecond();
		FILETIME ft;
		int64_t microseconds = 0;
		if (!SystemTimeToFileTime(&st, &ft)) {
			microseconds = 0;
		}
		if (is_local) {
			FILETIME utc_ft;
			LocalFileTimeToFileTime(&ft, &utc_ft);
			microseconds = time_helper::FileTimeToMicroseconds(utc_ft);
		}else {
			microseconds = time_helper::FileTimeToMicroseconds(ft);
		}
		return Time(microseconds);
	}

	Time::TimeStruct Time::ToTimeStruct(bool is_local) {
		FILETIME utc_ft;
		TimeStruct ts;
		time_helper::MicrosecondsToFileTime(microseconds_, &utc_ft);
		FILETIME ft;
		BOOL success = TRUE;
		if (is_local) {
			success = FileTimeToLocalFileTime(&utc_ft, &ft);
		}else {
			ft = utc_ft;
		}
		SYSTEMTIME st;
		if (!success || !FileTimeToSystemTime(&ft,&st)) {
			return ts;
		}
		ts.year_				 = st.wYear;
		ts.month_				= st.wMonth;
		ts.day_of_week_	= st.wDayOfWeek;
		ts.day_of_month_ = st.wDay;
		ts.hour_				 = st.wHour;
		ts.minute_			 = st.wMinute;
		ts.second_			 = st.wSecond;
		ts.millisecond_	= st.wMilliseconds;
		return ts;
	}

	Time Time::FromFileTime(FILETIME ft) {
		return Time(time_helper::FileTimeToMicroseconds(ft));
	}

	FILETIME Time::ToFileTime() const {
		FILETIME utc_ft;
		time_helper::MicrosecondsToFileTime(microseconds_,&utc_ft);
		return utc_ft;
	}

	Time& Time::operator=(const Time &other) {
		microseconds_ = other.microseconds_;
		return *this;
	}

	TimeSpan Time::operator-(Time other) const {
		return TimeSpan(microseconds_ - other.microseconds_);
	}

	Time Time::operator+(TimeSpan span) {
		return Time(microseconds_ + span.span_);
	}

	Time Time::operator-(TimeSpan span) {
		return Time(microseconds_ - span.span_);
	}

	Time& Time::operator+=(TimeSpan span) {
		microseconds_ += span.span_;
		return *this;
	}

	Time& Time::operator-=(TimeSpan span) {
		microseconds_ -= span.span_;
		return *this;
	}

	bool Time::operator==(Time other) {
		return microseconds_ == other.microseconds_;
	}

	bool Time::operator!=(Time other) {
		return microseconds_ != other.microseconds_;
	}

	bool Time::operator<(Time other) {
		return microseconds_ < other.microseconds_;
	}

	bool Time::operator<=(Time other) {
		return microseconds_ <= other.microseconds_;
	}

	bool Time::operator>(Time other) {
		return microseconds_ > other.microseconds_;
	}

	bool Time::operator>=(Time other) {
		return microseconds_ > other.microseconds_;
	}

	//TimeTicks
	TimeTicks TimeTicks::Now() {
		return TimeTicks() + time_helper::RolloverProtectedNow();
	}

	TimeTicks TimeTicks::HightResolutionNow() {
		return TimeTicks() + time_helper::HighResolutionNowSingleton::GetInstance()->Now();
	}

  TimeTicks& TimeTicks::operator=(const TimeTicks &other) {
    ticks_ = other.ticks_;
    return *this;
  }

  TimeSpan TimeTicks::operator-(TimeTicks other) {
    return TimeSpan(ticks_ - other.ticks_);
  }

  TimeTicks TimeTicks::operator+(TimeSpan span) {
    return TimeTicks(ticks_ + span.span_);
  }

  TimeTicks TimeTicks::operator-(TimeSpan span) {
    return TimeTicks(ticks_ - span.span_);
  }

  TimeTicks& TimeTicks::operator+=(TimeSpan span) {
    ticks_ += span.span_;
    return *this;
  }

  TimeTicks& TimeTicks::operator-=(TimeSpan span) {
    ticks_ -= span.span_;
    return *this;
  }

  bool TimeTicks::operator==(TimeTicks other) {
    return ticks_ == other.ticks_;
  }

  bool TimeTicks::operator!=(TimeTicks other) {
    return ticks_ != other.ticks_;
  }

  bool TimeTicks::operator<(TimeTicks other) {
    return ticks_ < other.ticks_;
  }

  bool TimeTicks::operator<=(TimeTicks other) {
    return ticks_ <= other.ticks_;
  }

  bool TimeTicks::operator>(TimeTicks other) {
    return ticks_ > other.ticks_;
  }

  bool TimeTicks::operator>=(TimeTicks other) {
    return ticks_ >= other.ticks_;
  }
}
