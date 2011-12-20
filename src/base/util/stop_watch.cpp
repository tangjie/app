#include "base/util/stop_watch.h"

#include <iostream>
#include "base/time/time.h"
#include "gflags/gflags.h"

DEFINE_bool(force_stop_watch_report, false, "whether or not report stop watch in release(opt) version");

namespace {
	//TODO(oldman): make this lazy initialized
	base::internal::StopWatchDefaultReporter g_defaultReporter;
#ifdef NDEBUG
	bool kReportStopWatch = FLAGS_force_stop_watch_report;
#else
	bool kReportStopWatch = true;
#endif
}

namespace base {
	namespace internal {
		void StopWatchDefaultReporter::Report(const StringPiece& watch_name, TimeSpan elapsed) {
			//TODO(oldman): use appropriate report method
			std::cout << "StopWatch " << watch_name.to_string() << " use " << elapsed.ToMilliseconds() << "ms" << std::endl;
		}
	}

	StopWatch::StopWatch(const StringPiece& watch_name)
		: watch_name_(watch_name), reporter_(&g_defaultReporter), is_running_(false), elapsed_(0) {
	}
	StopWatch::StopWatch(const StringPiece& watch_name, StopWatchReporter* reporter)
		: watch_name_(watch_name), reporter_(reporter), is_running_(false), elapsed_(0){
	}
	bool StopWatch::Start() { 
		if (is_running_) {
			return false;
		}
		last_timeticks_ = TimeTicks::HightResolutionNow();
		is_running_ = true;
		return true;
	}
	void StopWatch::Stop() {
		if (is_running_) {
			elapsed_ += (TimeTicks::HightResolutionNow() - last_timeticks_).ToInternalValue();
			is_running_ = false;
		}
	}
	void StopWatch::StopAndReset() {
		//Since we clear elapsed_, so we have no need to calculate it
		is_running_ = false;
		elapsed_ = 0;
	}
	void StopWatch::Restart() {
		StopAndReset();
		Start();
	}
	void StopWatch::Report() const {
		//TODO(oldman): assert reporter_ is not null
		int64_t elapsed_now = elapsed_;
		if (is_running_) {
			elapsed_now += (TimeTicks::HightResolutionNow() - last_timeticks_).ToInternalValue();
		}
		reporter_->Report(watch_name_, TimeSpan::FromMicroseconds(elapsed_now));
	}
}
	
