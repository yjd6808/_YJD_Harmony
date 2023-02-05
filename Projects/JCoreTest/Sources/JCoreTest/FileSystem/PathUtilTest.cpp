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
	auto d = Path::Combine("aaa/////", "\\\\ggfd/bbb");
	auto e = Path::Combine("a", "b");
	auto f = Path::Combine("a", "b", "c");

	EXPECT_TRUE(a == "aaa/bbb");
	EXPECT_TRUE(b == "aaa/bbb");
	EXPECT_TRUE(c == "aaa/ggfd/bbb");
	EXPECT_TRUE(d == "aaa/ggfd/bbb");
	EXPECT_TRUE(e == "a/b");
	EXPECT_TRUE(f == "a/b/c");

	auto g = Path::Extension("fse.abc");
	auto h = Path::Extension(".abc");
	auto i = Path::Extension(".");
	auto j = Path::Extension("fsefes");
	auto k = Path::Extension("fsecfse/fsefes");
	auto l = Path::Extension("fsecfse/fsefes.k");
	EXPECT_TRUE(g == ".abc");
	EXPECT_TRUE(h == ".abc");
	EXPECT_TRUE(i == ".");
	EXPECT_TRUE(j == "");
	EXPECT_TRUE(k == "");
	EXPECT_TRUE(l == ".k");

	EXPECT_EQ(Path::FileNameLevel("a/b.a", 1), "a/b.a");
	EXPECT_EQ(Path::FileNameLevel("/a/b.a", 1), "a/b.a");
	EXPECT_EQ(Path::FileNameLevel("//a/b.a", 1), "a/b.a");
	EXPECT_EQ(Path::FileNameLevel("b////a/b.a", 1), "a/b.a");
	EXPECT_EQ(Path::FileNameLevel("b////a/b.a", 2), "b/a/b.a");
	EXPECT_EQ(Path::FileNameLevel("b////a/b.a", 3), "b/a/b.a");
	EXPECT_EQ(Path::FileNameLevel("b////a/\\\\b.a", 3), "b/a/b.a");
	EXPECT_EQ(Path::FileNameLevel("\\b////a/\\\\b.a", 4), "b/a/b.a");
	EXPECT_EQ(Path::FileNameLevel("", 4), "");

}

#endif