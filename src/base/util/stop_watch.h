/*
 * StopWatch，秒表，可以方便的输出代码运行时间，便于开发者性能调优。
 * 使用方法：
 * (1) void foo() {
 *       StopWatch watch(base::StringPiece("bar"));
 *       watch.Start();
 *       ......             //some code
 *       watch.Stop();
 *       watch.Report();
 *       ......             //some code 2
 *       watch.Report();    //since watch already stopped, this result is equal to previous Report()
 *       watch.Start();
 *       ......             //some code 3
 *       watch.Report();
 *     }
 * (2) void foo() {
 *       ScopedStopWatch watch(base::StringPiece("bar"));
 *       ......
 *       ......
 *     } //watch.Report() should occur at this point.
 * (3) 上例中的ScopedStopWatch..一行可以用下面的方式代替：
 *       INSTALL_SCOPED_STOP_WATCH(bar);
 *     这样做有两点好处：
 *     a) 自动给变量取名字，避免watch对象重名；
 *     b) 输出的结果中会含有调用StopWatch的源文件的相关信息；
 * (4) 多路StopWatch TODO(oldman)
 *
 * 默认情况下，StopWatch只会工作在dbg模式下，如果希望其工作在opt版本中，请设置XXX参数（详见cpp文件）TODO(oldman)
 *       
 * ref: (1)http://static.springsource.org/spring/docs/2.0.x/api/org/springframework/util/StopWatch.html
 *      (2)http://msdn.microsoft.com/en-us/library/system.diagnostics.stopwatch.aspx
 */


#ifndef BASE_UTIL_STOP_WATCH_H__
#define BASE_UTIL_STOP_WATCH_H__

#include "base/base_types.h"
#include "base/string/string_piece.h"
#include "base/time/time.h"
#include "base/util/noncopyable.h"

namespace base {
	// Reporter for StopWatch
	class StopWatchReporter {
	public:
		// Actual Report for StopWatch, you can override this for custom reporter
		// see comments of StopWatch::StopWatch for detail
		virtual void Report(const StringPiece& watch_name, TimeSpan elapsed) = 0;
	};
	namespace internal {
		class StopWatchDefaultReporter : public StopWatchReporter {
		public:
			virtual void Report(const StringPiece& watch_name, TimeSpan elapsed);
		};
	}

	class StopWatch: public noncopyable {
	public:
		// build a stopwatch which name is |watch_name|, which reporter is default Reporter
		// (internal::StopWatchDefaultReport)
		StopWatch(const StringPiece& watch_name);
		// build a stopwatch which name is |watch_name|, which reporter is user-defined Reporter
		StopWatch(const StringPiece& watch_name, StopWatchReporter* reporter);
		// operation: Start this stopwatch, if already started, return false
		bool Start();
		// operation: Stop this stopwatch, if already stopped, do nothing
		void Stop();
		// operation: Stop this stopwatch and reset its elapsed time
		void StopAndReset();
		// operation: Restart = StopAndReset + Start.
		void Restart();
		// is this stopwatch running
		bool IsRunning() const {
			return is_running_;
		}
		// get elapsed time
		TimeSpan elapsed() const {
			return TimeSpan::FromMicroseconds(elapsed_);
		}
		// name of the watch
		void SetName(const StringPiece& watch_name) {
			watch_name_ = watch_name;
		}
		StringPiece name() const {
			return watch_name_;
		}
		// report data, actual result is decided by reporter_;
		// if the watch is running, Report will report data according current time,
		// but it don't stop this watch.
		void Report() const;
	protected:
		bool is_running_;
		int64_t elapsed_;
		TimeTicks last_timeticks_;
		StringPiece watch_name_;
		StopWatchReporter* reporter_;
	};

	//handy stopwatch which report elapsed_time when deleted
	class ScopedStopWatch: public StopWatch {
	public:
		ScopedStopWatch(const StringPiece& watch_name) : StopWatch(watch_name) {
		}
		ScopedStopWatch(const StringPiece& watch_name, StopWatchReporter* reporter)
			: StopWatch(watch_name, reporter) {
		}
		~ScopedStopWatch() {
			Report();
		}
	};
}

#define INSTALL_STOP_WATCH(name) base::ScopedStopWatch StopWatch##name(base::StringPiece(#name "##" __FILE__ "##" __LINE__));

#endif
