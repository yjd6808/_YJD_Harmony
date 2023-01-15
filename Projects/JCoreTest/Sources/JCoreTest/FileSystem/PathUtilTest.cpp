/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 9:21:07 AM
 * =====================
 *
 */



#include <JCoreTest/CoreTest.h>
#include <JCore/FileSystem/Path.h>

#if TEST_PathTest == ON

TEST(PathTest, BasicTest) {
	EXPECT_EQ(Path::FileName("a/b.a"), "b.a");
	EXPECT_EQ(Path::FileName("a\\b.a"), "b.a");
	EXPECT_EQ(Path::FileName("a//k//bb.c"), "bb.c");

	auto a = Path::Combine("aaa/", "bbb");
	auto b = Path::Combine("aaa", "\\bbb");
	auto c = Path::Combine("aaa/", "\\ggfd/bbb");

}

#endif