#include <sstream>
#include "base/util/stop_watch.h"
#include "gtest/gtest.h"

namespace {
	class ExpectReporter : public base::StopWatchReporter {
	public:
		ExpectReporter(base::StringPiece watch_name) : watch_name_(watch_name) {
		}

		virtual void Report(const base::StringPiece& watch_name, base::TimeSpan elapsed) {
			EXPECT_EQ(watch_name, watch_name_);
			EXPECT_GT(elapsed.ToMilliseconds(), 0);
		}
	private:
		base::StringPiece watch_name_;
	};
}

TEST(StopWatch, Basic) {
	base::StopWatch watch(base::StringPiece("a"));
	EXPECT_EQ(watch.name(), "a");
	EXPECT_FALSE(watch.IsRunning());
	watch.Start();
	EXPECT_TRUE(watch.IsRunning());
	watch.Stop();
	EXPECT_FALSE(watch.IsRunning());
	watch.Report();
	watch.StopAndReset();
	EXPECT_FALSE(watch.IsRunning());
	watch.Restart();
	EXPECT_TRUE(watch.IsRunning());
}

TEST(StopWatch, ScopedStopWatch) {
	for (int i = 0; i < 10; ++i) {
		std::stringstream ss;
		ss << i;
		std::string name = std::string("watch") + ss.str();
		base::StringPiece watch_name(name);
		ExpectReporter reporter(watch_name);
		base::ScopedStopWatch watch(base::StringPiece(name), implicit_cast<base::StopWatchReporter*>(&reporter));
		EXPECT_TRUE(watch.IsRunning());
		Sleep(100);
	}
}