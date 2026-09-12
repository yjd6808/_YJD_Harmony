/*
	작성자 : 윤정도
	StringUtil 클래스 템플릿 테스트 (A/W 양쪽)
*/

#include <type_traits>

#include "jc/Primitives/String.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/Primitives/StringConvert.h"

#if TEST_StringUtilTest == ON

template <typename CharT>
using TStr = jc::BasicString<CharT, jc::StringImpl_SSO<CharT>>;

// narrow 테스트 데이터를 CharT 문자열로 변환한다 (wchar_t판은 UTF-8 디코드).
template <typename CharT>
TStr<CharT> C(const char* _str)
{
	if constexpr (std::is_same_v<CharT, char>)
		return TStr<CharT>(_str);
	else
		return jc::StringConvert::ToWide(_str);
}

template <typename CharT>
class StringUtilTypedTest : public ::testing::Test {};

// googletest 1.8.1이라 TYPED_TEST_SUITE 대신 TYPED_TEST_CASE를 쓴다.
// 1.8.1 TYPED_TEST_CASE는 가변 인자라 콤마가 있으면 쪼개지므로 typedef로 묶어야 한다.
typedef ::testing::Types<char, wchar_t> StringUtilTestTypes;
TYPED_TEST_CASE(StringUtilTypedTest, StringUtilTestTypes);

TYPED_TEST(StringUtilTypedTest, Length) {
	auto src1 = C<TypeParam>("abcd");
	auto src2 = C<TypeParam>("0");
	auto src3 = C<TypeParam>("");
	EXPECT_TRUE((jc::BasicStringUtil<TypeParam>::Length(src1.Source())) == 4);
	EXPECT_TRUE((jc::BasicStringUtil<TypeParam>::Length(src2.Source())) == 1);
	EXPECT_TRUE((jc::BasicStringUtil<TypeParam>::Length(src3.Source())) == 0);
}


TYPED_TEST(StringUtilTypedTest, CopyAndEqual) {
	TypeParam dst[256];
	auto src = C<TypeParam>("abcdefg");
	jc::BasicStringUtil<TypeParam>::Copy(dst, 256, src.Source());
	EXPECT_TRUE(jc::BasicStringUtil<TypeParam>::IsEqual(dst, 7, src.Source(), 7));
	EXPECT_TRUE(jc::BasicStringUtil<TypeParam>::IsEqual(dst, 6, src.Source(), 6));
}

TYPED_TEST(StringUtilTypedTest, Swap) {
	TStr<TypeParam> szStr1 = C<TypeParam>("abcd");
	TStr<TypeParam> szStr2 = C<TypeParam>("efgh");

	jc::BasicStringUtil<TypeParam>::Swap(szStr1, szStr2);

	EXPECT_TRUE(szStr1 == C<TypeParam>("efgh"));
	EXPECT_TRUE(szStr2 == C<TypeParam>("abcd"));
}

TYPED_TEST(StringUtilTypedTest, CTLength) {

	const int len1 = jc::BasicStringUtil<TypeParam>::CTLength(C<TypeParam>("abcd").Source());
	const int len2 = jc::BasicStringUtil<TypeParam>::CTLength(C<TypeParam>("abcdabcdabcdabcdabcd").Source());
	const int len3 = jc::BasicStringUtil<TypeParam>::CTLength(C<TypeParam>("").Source());

	EXPECT_TRUE(len1 == 4);
	EXPECT_TRUE(len2 == 20);
	EXPECT_TRUE(len3 == 0);
}

TYPED_TEST(StringUtilTypedTest, CTFindChar) {
	auto src = C<TypeParam>("abcd");
	const int idx1 = jc::BasicStringUtil<TypeParam>::CTFindChar(src.Source(), TypeParam('a'));	// 0
	const int idx2 = jc::BasicStringUtil<TypeParam>::CTFindChar(src.Source(), TypeParam('b'));	// 1
	const int idx3 = jc::BasicStringUtil<TypeParam>::CTFindChar(src.Source(), TypeParam('c'));	// 2
	const int idx4 = jc::BasicStringUtil<TypeParam>::CTFindChar(src.Source(), TypeParam('d'));	// 3
	const int idx5 = jc::BasicStringUtil<TypeParam>::CTFindChar(src.Source(), TypeParam('1'));	// -1

	EXPECT_TRUE(idx1 == 0);
	EXPECT_TRUE(idx2 == 1);
	EXPECT_TRUE(idx3 == 2);
	EXPECT_TRUE(idx4 == 3);
	EXPECT_TRUE(idx5 == -1);
}


TYPED_TEST(StringUtilTypedTest, CTFindCharReverse) {
	auto src = C<TypeParam>("abcd");
	const int idx1 = jc::BasicStringUtil<TypeParam>::CTFindCharReverse(src.Source(), TypeParam('a'));	// 0
	const int idx2 = jc::BasicStringUtil<TypeParam>::CTFindCharReverse(src.Source(), TypeParam('b'));	// 1
	const int idx3 = jc::BasicStringUtil<TypeParam>::CTFindCharReverse(src.Source(), TypeParam('c'));	// 2
	const int idx4 = jc::BasicStringUtil<TypeParam>::CTFindCharReverse(src.Source(), TypeParam('d'));	// 3
	const int idx5 = jc::BasicStringUtil<TypeParam>::CTFindCharReverse(src.Source(), TypeParam('1'));	// -1

	EXPECT_TRUE(idx1 == 0);
	EXPECT_TRUE(idx2 == 1);
	EXPECT_TRUE(idx3 == 2);
	EXPECT_TRUE(idx4 == 3);
	EXPECT_TRUE(idx5 == -1);
}

TYPED_TEST(StringUtilTypedTest, ToNumber) {
	auto src = C<TypeParam>("12345");
	EXPECT_TRUE((jc::BasicStringUtil<TypeParam>::template ToNumber<_s32>(src.Source())) == 12345);
	EXPECT_TRUE((jc::BasicStringUtil<TypeParam>::template ToNumber<_u32>(src.Source())) == 12345u);
	EXPECT_TRUE((jc::BasicStringUtil<TypeParam>::template ToNumber<_s64>(src.Source())) == 12345ll);

	_s32 v = 0;
	EXPECT_TRUE(jc::BasicStringUtil<TypeParam>::TryToNumber(v, src.Source()));
	EXPECT_TRUE(v == 12345);
}

TYPED_TEST(StringUtilTypedTest, ToString) {
	EXPECT_TRUE(jc::BasicStringUtil<TypeParam>::ToString(12345) == C<TypeParam>("12345"));
	EXPECT_TRUE(jc::BasicStringUtil<TypeParam>::ToString(-42) == C<TypeParam>("-42"));
}

#endif // TEST_StringUtilTest == ON
