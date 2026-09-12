/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 9:21:07 AM
 * =====================
 *
 */



#include "jc/IO/Path.h"

#if TEST_PathTest == ON

TEST(PathTest, BasicTest) {
	EXPECT_EQ(Path::FileName(_T("a/b.a")), _T("b.a"));
	EXPECT_EQ(Path::FileName(_T("a\\b.a")), _T("b.a"));
	EXPECT_EQ(Path::FileName(_T("a//k//bb.c")), _T("bb.c"));

	auto a = Path::Combine(_T("aaa/"), _T("bbb"));
	auto b = Path::Combine(_T("aaa"), _T("\\bbb"));
	auto c = Path::Combine(_T("aaa/"), _T("\\ggfd/bbb"));
	auto d = Path::Combine(_T("aaa/////"), _T("\\\\ggfd/bbb"));
	auto e = Path::Combine(_T("a"), _T("b"));
	auto f = Path::Combine(_T("a"), _T("b"), _T("c"));
	auto z = Path::Combine(_T(""), _T("b"), _T("c"));

	EXPECT_TRUE(a == _T("aaa/bbb"));
	EXPECT_TRUE(b == _T("aaa/bbb"));
	EXPECT_TRUE(c == _T("aaa/ggfd/bbb"));
	EXPECT_TRUE(d == _T("aaa/ggfd/bbb"));
	EXPECT_TRUE(e == _T("a/b"));
	EXPECT_TRUE(f == _T("a/b/c"));
	EXPECT_TRUE(z == _T("b/c"));

	auto g = Path::Extension(_T("fse.abc"));
	auto h = Path::Extension(_T(".abc"));
	auto i = Path::Extension(_T("."));
	auto j = Path::Extension(_T("fsefes"));
	auto k = Path::Extension(_T("fsecfse/fsefes"));
	auto l = Path::Extension(_T("fsecfse/fsefes.k"));
	EXPECT_TRUE(g == _T(".abc"));
	EXPECT_TRUE(h == _T(".abc"));
	EXPECT_TRUE(i == _T("."));
	EXPECT_TRUE(j == _T(""));
	EXPECT_TRUE(k == _T(""));
	EXPECT_TRUE(l == _T(".k"));

	EXPECT_EQ(Path::FileNameLevel(_T("a/b.a"), 1), _T("a/b.a"));
	EXPECT_EQ(Path::FileNameLevel(_T("/a/b.a"), 1), _T("a/b.a"));
	EXPECT_EQ(Path::FileNameLevel(_T("//a/b.a"), 1), _T("a/b.a"));
	EXPECT_EQ(Path::FileNameLevel(_T("b////a/b.a"), 1), _T("a/b.a"));
	EXPECT_EQ(Path::FileNameLevel(_T("b////a/b.a"), 2), _T("b/a/b.a"));
	EXPECT_EQ(Path::FileNameLevel(_T("b////a/b.a"), 3), _T("b/a/b.a"));
	EXPECT_EQ(Path::FileNameLevel(_T("b////a/\\\\b.a"), 3), _T("b/a/b.a"));
	EXPECT_EQ(Path::FileNameLevel(_T("\\b////a/\\\\b.a"), 4), _T("b/a/b.a"));
	EXPECT_EQ(Path::FileNameLevel(_T(""), 4), _T(""));

}

#endif
