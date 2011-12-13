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
 *       DEFINE_STOP_WATCH(bar);
 *     这样做有两点好处：
 *     a) 自动给变量取名字，避免watch对象重名；
 *     b) 输出的结果中会含有调用StopWatch的文件/函数信息；TODO(oldman)
 * (4) TODO(oldman)多路StopWatch
 *
 * 默认情况下，StopWatch只会工作在dbg模式下，如果希望其工作在opt版本中，请设置XXX参数（详见cpp文件）TODO(oldman)
 *       
 * ref: (1)http://static.springsource.org/spring/docs/2.0.x/api/org/springframework/util/StopWatch.html
 *      (2)http://msdn.microsoft.com/en-us/library/system.diagnostics.stopwatch.aspx
 */


#ifndef BASE_STOP_WATCH_H__
#define BASE_STOP_WATCH_H__

#include "base/base_types.h"
#include "base/string/string_piece.h"
#include "base/time/time.h"
#include "base/util/noncopyable.h"

namespace base {
	namespace internal {
		typedef void (*Reporter) (int64_t);
		void StopWatchDefaultReport(int64_t elapsed);
	}

	class StopWatch: public noncopyable {
	public:
		// build a stopwatch which name is |watch_name|
		StopWatch(const StringPiece& watch_name);
		// operation: Start this stopwatch, if already started, return false
		bool Start();
		// operation: Stop this stopwatch, if already stopped, do nothing
		void Stop();
		// operation: Stop this stopwatch and reset its elapsed time
		void StopAndReset();
		// operation: Restart = StopAndReset + Start.
		void Restart();
		// is this stopwatch running
		bool IsRunning() const;
		// get elapsed time
		TimeSpan elapsed() const;
		// report data in readable format
		void Report(internal::Reporter r = internal::StopWatchDefaultReport);
	protected:
		bool is_running_;
		int64_t elapsed_;
	};

	class MultiStopWatch: public noncopyable {
	public:
		// build a multiway stopwatch which name is |watch_name|
		StopWatch(const StringPiece& watch_name);
		// operation: Start this stopwatch, if already started, return false
		bool Start();
		// operation: Stop this stopwatch, if already stopped, do nothing
		void Stop();
		// operation: Start
	};

	//handy stopwatch which report elapsed_time when deleted
	class ScopedStopWatch: public StopWatch {
	public:
		ScopedStopWatch(const StringPiece& watch_name, internal::Reporter reporter = internal::StopWatchDefaultReport)
			:StopWatch(watch_name), reporter_(reporter) {
		}
		~ScopedStopWatch() {
			Stop();
			Report(reporter_);
		}
	protected:
		internal::Reporter* reporter_;
	};
}

#define DEFINE_STOP_WATCH(name) base::ScopedStopWatch StopWatch##name(base::StringPiece(#name "in" __FILE__));

#endif
