/*
	작성자 : 윤정도
	StringUtil 클래스 테스트입니다.
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/Primitives/String.h>
#include <JCore/Primitives/StringUtil.h>



#if TEST_StringUtilTest == ON

TEST(StringUtilTest, Length) {
	EXPECT_TRUE(StringUtil::Length("abcd") == 4);
	EXPECT_TRUE(StringUtil::Length("0") == 1);
	EXPECT_TRUE(StringUtil::Length("") == 0);
}


TEST(StringUtilTest, CopyAndEqual) {
	char dst[256];
	StringUtil::Copy(dst, 256, "abcdefg");
	EXPECT_TRUE(StringUtil::IsEqual(dst, 7, "abcdefg", 7));
	EXPECT_TRUE(StringUtil::IsEqual(dst, 6, "abcdefg", 6));
}

TEST(StringUtilTest, Swap) {
	String szStr1 = "abcd";
	String szStr2 = "efgh";

	StringUtil::Swap(szStr1, szStr2);

	EXPECT_TRUE(szStr1 == "efgh");
	EXPECT_TRUE(szStr2 == "abcd");
}

TEST(StringUtilTest, CTLength) {

	const int len1 = StringUtil::CTLength("abcd");
	const int len2 = StringUtil::CTLength("abcdabcdabcdabcdabcd");
	const int len3 = StringUtil::CTLength("");

	EXPECT_TRUE(len1 == 4);
	EXPECT_TRUE(len2 == 20);
	EXPECT_TRUE(len3 == 0);
}

TEST(StringUtilTest, CTFindChar) {
	const int idx1 = StringUtil::CTFindChar("abcd", 'a');	// 0
	const int idx2 = StringUtil::CTFindChar("abcd", 'b');	// 1
	const int idx3 = StringUtil::CTFindChar("abcd", 'c');	// 2
	const int idx4 = StringUtil::CTFindChar("abcd", 'd');	// 3
	const int idx5 = StringUtil::CTFindChar("abcd", '1');	// -1

	EXPECT_TRUE(idx1 == 0);
	EXPECT_TRUE(idx2 == 1);
	EXPECT_TRUE(idx3 == 2);
	EXPECT_TRUE(idx4 == 3);
	EXPECT_TRUE(idx5 == -1);
}


TEST(StringUtilTest, CTFindCharReverse) {
	const int idx1 = StringUtil::CTFindCharReverse("abcd", 'a');	// 0
	const int idx2 = StringUtil::CTFindCharReverse("abcd", 'b');	// 1
	const int idx3 = StringUtil::CTFindCharReverse("abcd", 'c');	// 2
	const int idx4 = StringUtil::CTFindCharReverse("abcd", 'd');	// 3
	const int idx5 = StringUtil::CTFindCharReverse("abcd", '1');	// -1

	EXPECT_TRUE(idx1 == 0);
	EXPECT_TRUE(idx2 == 1);
	EXPECT_TRUE(idx3 == 2);
	EXPECT_TRUE(idx4 == 3);
	EXPECT_TRUE(idx5 == -1);
}

#endif // TEST_StringUtilTest == ON