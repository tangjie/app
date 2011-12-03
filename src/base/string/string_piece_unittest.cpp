#include "base/string/string_piece.h"
#include "gtest/gtest.h"

using base::StringPiece;
using base::StringPiece16;
//TODO(oldman):给StringPiece添加详细完备的注释

TEST(StringPiece, Fuzzy) {
	char s[] = "foobar";
	StringPiece sp(s);
	EXPECT_EQ(2, sp.find('o', 2));
	sp.replace(3, 3, 'f');
	EXPECT_STREQ(sp.to_string().c_str(), "foofff");
	EXPECT_EQ(2, sp.rfind('o'));
	sp.copy(sp.as_array(), 3, 3);
	EXPECT_STREQ(sp.to_string().c_str(), "ffffff");
	EXPECT_EQ(StringPiece::npos, sp.find_first_not_of('f'));
	EXPECT_EQ(sp, sp);
}

TEST(StringPiece, NullTest) {
	StringPiece a;
	StringPiece b(nullptr);
	EXPECT_EQ(0, a.size());
	EXPECT_EQ(nullptr, a.data());
	EXPECT_EQ(0, b.size());
	EXPECT_EQ(nullptr, b.data());
	StringPiece c(b);
	EXPECT_EQ(0, c.size());
	EXPECT_EQ(nullptr, c.data());
}

//note: to call this function must ensure that sp can't appear two or more same characters.
void TestForRandAccess(StringPiece sp, char* origin) {
	for (size_t i = 0; i < sp.size(); i++) {
		EXPECT_EQ(sp[i], origin[i]);
		EXPECT_EQ(sp.find(origin[i]), i);
		EXPECT_EQ(sp.rfind(origin[i]), i);
	}
	if (origin != nullptr) {
		EXPECT_EQ(sp.find(origin), 0);
		EXPECT_EQ(sp.find_first_not_of(origin), StringPiece::npos);
	}
}

TEST(StringPiece, STLTest) {
	StringPiece a("abcdefgh");
	StringPiece b;
	char buf[] = {'\0', '\0', '\0'};
	StringPiece c(buf);
	StringPiece d("abcd");
	StringPiece e("efgh");
	EXPECT_EQ(8, a.size());
	EXPECT_EQ('a', *a.data());
	EXPECT_EQ('a', a.data()[0]);
	EXPECT_TRUE(a.begin() != a.end());
	EXPECT_EQ('a', *a.begin());
	EXPECT_EQ('h', *a.rbegin());
	EXPECT_EQ('h', *(a.end() - 1));
	EXPECT_FALSE(a.empty());
	EXPECT_EQ(a.find(d), 0);
	EXPECT_EQ(a.find_first_of(d), 0);
	EXPECT_EQ(a.find(e), 4);
	EXPECT_EQ(a.find_first_of(e), 4);
	TestForRandAccess(a, "abcdefgh");

	EXPECT_TRUE(b.empty());
	EXPECT_TRUE(b.begin() == b.end());
	EXPECT_TRUE(b.rbegin() == b.rend());
	TestForRandAccess(b, nullptr);

	StringPiece f("====");
	EXPECT_EQ(f.find('='), 0);
	EXPECT_EQ(f.find("="), 0);
	EXPECT_EQ(f.find_first_of('='), 0);
	EXPECT_EQ(f.find_first_of("="), 0);
	EXPECT_EQ(f.find_last_of('='), 3);
	EXPECT_EQ(f.find_last_of("="), 3);
	EXPECT_EQ(f.find_first_not_of('='), StringPiece::npos);
	EXPECT_EQ(f.find_first_not_of("="), StringPiece::npos);
	EXPECT_EQ(f.find_last_not_of('='), StringPiece::npos);
	EXPECT_EQ(f.find_last_not_of("="), StringPiece::npos);
	std::string temp("123");
	temp += '\0';
	temp += "456";
	StringPiece g(temp);
	EXPECT_EQ(3, g.find('\0'));
	g.replace(3, 1, '\n');
	EXPECT_EQ(3, g.find('\n'));
}

TEST(StringPiece, SubstrTest) {
	char buf[7] = {'a', 'b', 'c', 'd', 'e', 'f' , '\0'};
	StringPiece a(buf);
	EXPECT_EQ(6, a.size());
	TestForRandAccess(a, buf);
	EXPECT_STREQ(a.substr().to_string().c_str(), a.to_string().c_str());
	EXPECT_STREQ(a.substr(0, a.size()).to_string().c_str(), a.to_string().c_str());
	EXPECT_EQ(a.substr(0,1), "a");
	EXPECT_EQ(a.substr(1,1), "b");
	EXPECT_EQ(a.substr(2,1), "c");
	EXPECT_EQ(a.substr(3,1), "d");
	EXPECT_EQ(a.substr(4,1), "e");
	EXPECT_EQ(a.substr(5,1), "f");
	EXPECT_EQ(a.substr(a.find("c")), "cdef");
}

TEST(StringPiece, CompareTest) {
	StringPiece a("a");
	StringPiece A("A");
	StringPiece B(A);
	StringPiece b(a);
	EXPECT_LT('A' , 'a');
	EXPECT_TRUE(a == "a");
	EXPECT_TRUE(A == "A");
	EXPECT_GT('a', 'A');
	EXPECT_FALSE(a != "a");
	EXPECT_FALSE(A != "A");
	EXPECT_TRUE(A <= a);
	EXPECT_FALSE(A >= a);
	EXPECT_TRUE(a >= A);
	EXPECT_FALSE(a <= A);
	EXPECT_EQ(A, B);
	EXPECT_EQ(a, b);
	EXPECT_GT(a, A);
	EXPECT_GT(b, B);
	EXPECT_TRUE(a <= b);
	EXPECT_TRUE(a == b);
	EXPECT_TRUE(a >= b);
	EXPECT_FALSE(a != b);
	EXPECT_TRUE(A.to_string() <= a.to_string());
	EXPECT_TRUE(A <= a.to_string());
	EXPECT_TRUE(A.to_string() <= a);
	EXPECT_FALSE(A.to_string() >= a.to_string());
	EXPECT_FALSE(A >= a.to_string());
	EXPECT_FALSE(A.to_string() >= a);
}