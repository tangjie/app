#include "base/string/string_piece.h"
#include "gtest/gtest.h"

//TODO(oldman):给StringPiece添加详细完备的注释

TEST(StringPiece, Fuzzy) {
	char s[] = "foobar";
	StringPiece sp(s);
	EXPECT_EQ(2, sp.find('o', 2));
	sp.replace(3, 3, 'f');
	EXPECT_STREQ(sp.to_string().c_str(), "foofff");
	EXPECT_EQ(3, sp.rfind('o'));
	sp.copy(sp.as_array(), 3, 3);
	EXPECT_STREQ(sp.to_string().c_str(), "ffffff");
	EXPECT_EQ(StringPiece::npos, sp.find_first_not_of('f'));
	EXPECT_EQ(sp, sp);
}