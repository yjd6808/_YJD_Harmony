/*
	작성자 : 윤정도
	JStringUtil 클래스 테스트입니다.
*/

#include "JPCH.h"

#include <JString.h>
#include <JStringUtil.h>

TEST(JStringUtilTest, Length) {
	EXPECT_TRUE(JStringUtil::Length("abcd") == 4);
	EXPECT_TRUE(JStringUtil::Length("0") == 1);
	EXPECT_TRUE(JStringUtil::Length("") == 0);
}


TEST(JStringUtilTest, CopyAndEqual) {
	char dst[256];
	JStringUtil::Copy(dst, 256, "abcdefg");
	EXPECT_TRUE(JStringUtil::IsEqual(dst, 7, "abcdefg", 7));
	EXPECT_TRUE(JStringUtil::IsEqual(dst, 6, "abcdefg", 6));
}

TEST(JStringUtilTest, Swap) {
	JString szStr1 = "abcd";
	JString szStr2 = "efgh";

	JStringUtil::Swap(szStr1, szStr2);

	EXPECT_TRUE(szStr1 == "efgh");
	EXPECT_TRUE(szStr2 == "abcd");

}