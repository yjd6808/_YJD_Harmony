/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * WString (BasicString<wchar_t, StringImpl_SSO<wchar_t>>) smoke 테스트 +
 * StringConvert 경계 테스트 (실제 W API 호출 포함)
 */

#include "jc/Primitives/StringConvert.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/Wrapper/WinApi.h"

#if TEST_WStringTest == ON

TEST(WStringTest, Core) {
	WString s;
	EXPECT_TRUE(s.Length() == 0);
	EXPECT_TRUE(s.Capacity() == 10);

	s = L"hello";
	EXPECT_TRUE(s.Length() == 5);
	EXPECT_TRUE(s == L"hello");

	WString copied(s);
	EXPECT_TRUE(copied == L"hello");

	WString moved(std::move(s));
	EXPECT_TRUE(moved == L"hello");
	EXPECT_TRUE(s.Length() == 0);

	moved.Append(L" world", 6);
	EXPECT_TRUE(moved == L"hello world");

	EXPECT_TRUE(moved.Compare(L"hello world") == 0);
	EXPECT_TRUE(moved.Find(L"world") == 6);
	EXPECT_TRUE(moved.Find(L"WORLD", false) == 6);

	WString sub = moved.SubStr(6, 5);
	EXPECT_TRUE(sub == L"world");
}

TEST(WStringTest, Format) {
	WString s;
	s.Format(L"%d %s %c", 12345, L"bench", L'x');
	EXPECT_TRUE(s == L"12345 bench x");
}

TEST(WStringTest, Numbers) {
	WString s = L"12345";
	EXPECT_TRUE(s.ToInt32() == 12345);
	EXPECT_TRUE(s.ToInt64() == 12345ll);
	EXPECT_TRUE(s.ToDouble() == 12345.0);

	WString f = L"1.5";
	EXPECT_TRUE(f.ToFloat() == 1.5f);

	WString neg = L"-42";
	EXPECT_TRUE(neg.ToInt32() == -42);

	_s32 v = 0;
	EXPECT_TRUE(s.TryToInt32(v));
	EXPECT_TRUE(v == 12345);
	EXPECT_FALSE(WString(L"abc").TryToInt32(v));

	WString leading = L"00123";
	EXPECT_TRUE(leading.ToInt32() == 123);
}

TEST(WStringTest, Convert) {
	AString narrow = "hello";
	WString wide = StringConvert::ToWide(narrow);
	EXPECT_TRUE(wide == L"hello");

	AString back = StringConvert::ToUtf8(wide);
	EXPECT_TRUE(back == "hello");

	// 한글 왕복 (UTF-8 <-> UTF-16)
	AString korean = StringConvert::ToUtf8(L"한글", -1);
	WString koreanBack = StringConvert::ToWide(korean);
	EXPECT_TRUE(koreanBack == L"한글");
	EXPECT_TRUE(StringConvert::ToUtf8(koreanBack) == korean);
}

TEST(WStringTest, RealWin32Boundary) {
	// 실제 W API에 WString 버퍼를 넘기고 결과를 돌려받는다.
	WString dir;
	dir.Resize(260);
	const DWORD len = ::GetCurrentDirectoryW(dir.Capacity() + 1, dir.Source());
	EXPECT_TRUE(len > 0);
	dir.SetLength(static_cast<int>(len));	// API가 NUL 종결함
	EXPECT_TRUE(dir.Length() > 0);

	AString narrow = StringConvert::ToUtf8(dir);
	EXPECT_TRUE(narrow.Length() > 0);

	WString roundtrip = StringConvert::ToWide(narrow);
	EXPECT_TRUE(roundtrip == dir);
}

#endif
