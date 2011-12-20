#include "base/util/stop_watch.h"

#include "gtest/gtest.h"

TEST(StopWatch, Fuzzy) {
	base::ScopedStopWatch wScoped(base::StringPiece("foobar"));
	base::StopWatch w(base::StringPiece("Fuzzy"));
	w.Start();
	Sleep(100);
	w.Report();
	Sleep(100);
	w.Stop();
	w.Report();
	Sleep(100);
	w.Report();
	w.Start();
	Sleep(100);
	w.Report();
}