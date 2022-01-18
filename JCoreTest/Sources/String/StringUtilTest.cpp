/*
	작성자 : 윤정도
	StringUtil 클래스 테스트입니다.
*/

#include "JPCH.h"

#include <JCore/String/String.h>
#include <JCore/String/StringUtil.h>

using namespace JCore;

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