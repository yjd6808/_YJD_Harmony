/*
	작성자 : 윤정도
	StringView 클래스 테스트입니다. (A/W 양쪽)
*/


#include <type_traits>

#include "jc/Primitives/StringView.h"
#include "jc/Primitives/String.h"
#include "jc/Primitives/StringConvert.h"
#include "jc/Container/Vector.h"


using namespace std;

#if TEST_StringViewTest == ON

template <typename CharT>
using TStr = jc::BasicString<CharT, jc::StringImpl_SSO<CharT>>;

template <typename CharT>
using V = BasicStringView<CharT>;

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
class StringViewTypedTest : public ::testing::Test {};

// googletest 1.8.1이라 TYPED_TEST_SUITE 대신 TYPED_TEST_CASE를 쓴다.
// 1.8.1 TYPED_TEST_CASE는 가변 인자라 콤마가 있으면 쪼개지므로 typedef로 묶어야 한다.
typedef ::testing::Types<char, wchar_t> StringViewTestTypes;
TYPED_TEST_CASE(StringViewTypedTest, StringViewTestTypes);

// StringView 생성자 테스트
TYPED_TEST(StringViewTypedTest, Constructor) {
	// 기본 생성자
	V<TypeParam> view1;
	EXPECT_TRUE(view1.IsNull());
	EXPECT_TRUE(view1.IsEmpty());
	EXPECT_EQ(view1.Length(), 0);

	// char* 생성자
	auto pStr = C<TypeParam>("abcd");
	V<TypeParam> view2(pStr.Source());
	EXPECT_FALSE(view2.IsNull());
	EXPECT_FALSE(view2.IsEmpty());
	EXPECT_EQ(view2.Length(), 4);
	EXPECT_EQ(view2.Source(), pStr.Source());

	// char*, length 생성자
	V<TypeParam> view3(pStr.Source(), 2);
	EXPECT_EQ(view3.Length(), 2);

	// String 생성자
	TStr<TypeParam> szStr = C<TypeParam>("hello");
	V<TypeParam> view4(szStr);
	EXPECT_EQ(view4.Length(), 5);
	EXPECT_EQ(view4.Source(), szStr.Source());
}

// StringView 기본 정보 조회 테스트
TYPED_TEST(StringViewTypedTest, BasicInformation) {
	auto pStr = C<TypeParam>("test");
	V<TypeParam> view(pStr.Source());

	EXPECT_EQ(view.Length(), 4);
	EXPECT_EQ(view.LengthWithNull(), 5);
	EXPECT_FALSE(view.IsEmpty());
	EXPECT_FALSE(view.IsNull());
	EXPECT_EQ(view.Source(), pStr.Source());
	EXPECT_TRUE(view == C<TypeParam>("test"));

	// Null StringView
	V<TypeParam> nullView;
	EXPECT_TRUE(nullView.IsNull());
	EXPECT_TRUE(nullView == C<TypeParam>(""));

	// Empty StringView
	auto sEmpty = C<TypeParam>("");
	V<TypeParam> emptyView(sEmpty.Source(), 0);
	EXPECT_TRUE(emptyView.IsEmpty());
	EXPECT_FALSE(emptyView.IsNull());
}

// StringView 인덱스 검증 테스트
TYPED_TEST(StringViewTypedTest, IndexValidation) {
	auto pStr = C<TypeParam>("abcde");
	V<TypeParam> view(pStr.Source());

	// 유효한 인덱스
	EXPECT_TRUE(view.IsValidIndex(0));
	EXPECT_TRUE(view.IsValidIndex(4));
	EXPECT_FALSE(view.IsValidIndex(5));
	EXPECT_FALSE(view.IsValidIndex(-1));

	// 범위 검증
	EXPECT_TRUE(view.IsValidIndexRange(0, 4));
	EXPECT_TRUE(view.IsValidIndexRange(1, 3));
	EXPECT_FALSE(view.IsValidIndexRange(0, 5));
	EXPECT_FALSE(view.IsValidIndexRange(4, 3));
}

// StringView 문자 접근 테스트
TYPED_TEST(StringViewTypedTest, CharacterAccess) {
	auto pStr = C<TypeParam>("abcde");
	V<TypeParam> view(pStr.Source());

	EXPECT_EQ(view.GetAt(0), TypeParam('a'));
	EXPECT_EQ(view.GetAt(4), TypeParam('e'));
	EXPECT_EQ(view.GetAt(5), TypeParam('\0'));
	EXPECT_EQ(view.GetAt(-1), TypeParam('\0'));

	EXPECT_EQ(view.First(), TypeParam('a'));
	EXPECT_EQ(view.Last(), TypeParam('e'));

	EXPECT_EQ(view[0], TypeParam('a'));
	EXPECT_EQ(view[4], TypeParam('e'));
	EXPECT_EQ(view[5], TypeParam('\0'));

	// Empty StringView
	V<TypeParam> emptyView;
	EXPECT_EQ(emptyView.First(), TypeParam('\0'));
	EXPECT_EQ(emptyView.Last(), TypeParam('\0'));
}

// StringView::Find 테스트
TYPED_TEST(StringViewTypedTest, Find) {
	auto pStr = C<TypeParam>("abcdefg");
	V<TypeParam> view(pStr.Source());

	// 기본 Find
	EXPECT_EQ(view.Find(C<TypeParam>("g").Source()), 6);
	EXPECT_EQ(view.Find(C<TypeParam>("fg").Source()), 5);
	EXPECT_EQ(view.Find(C<TypeParam>("efg").Source()), 4);
	EXPECT_EQ(view.Find(C<TypeParam>("defg").Source()), 3);
	EXPECT_EQ(view.Find(C<TypeParam>("cdefg").Source()), 2);
	EXPECT_EQ(view.Find(C<TypeParam>("bcdefg").Source()), 1);
	EXPECT_EQ(view.Find(C<TypeParam>("abcdefg").Source()), 0);
	EXPECT_EQ(view.Find(C<TypeParam>("-abcdefg").Source()), -1);
	EXPECT_EQ(view.Find(C<TypeParam>("abcdefg-").Source()), -1);

	// 왼쪽에서 검사
	EXPECT_EQ(view.Find(C<TypeParam>("a").Source()), 0);
	EXPECT_EQ(view.Find(C<TypeParam>("ab").Source()), 0);
	EXPECT_EQ(view.Find(C<TypeParam>("abc").Source()), 0);
	EXPECT_EQ(view.Find(C<TypeParam>("abcd").Source()), 0);
	EXPECT_EQ(view.Find(C<TypeParam>("abcde").Source()), 0);
	EXPECT_EQ(view.Find(C<TypeParam>("abcdef").Source()), 0);
	EXPECT_EQ(view.Find(C<TypeParam>("abcdefg").Source()), 0);
	EXPECT_EQ(view.Find(C<TypeParam>("abcdefg-").Source()), -1);
	EXPECT_EQ(view.Find(C<TypeParam>("-abcdefg").Source()), -1);

	// 범위 검사
	EXPECT_EQ(view.Find(0, 6, C<TypeParam>("abcdefg").Source()), 0);
	EXPECT_EQ(view.Find(1, 6, C<TypeParam>("abcdefg").Source()), -1);
	EXPECT_EQ(view.Find(0, 5, C<TypeParam>("abcdefg").Source()), -1);
}

// StringView::FindReverse 테스트
TYPED_TEST(StringViewTypedTest, FindReverse) {
	auto pStr = C<TypeParam>("abcdefg");
	V<TypeParam> view(pStr.Source());

	// 기본 FindReverse
	EXPECT_EQ(view.FindReverse(C<TypeParam>("g").Source()), 6);
	EXPECT_EQ(view.FindReverse(C<TypeParam>("fg").Source()), 5);
	EXPECT_EQ(view.FindReverse(C<TypeParam>("efg").Source()), 4);
	EXPECT_EQ(view.FindReverse(C<TypeParam>("abcdefg").Source()), 0);
	EXPECT_EQ(view.FindReverse(C<TypeParam>("-abcdefg").Source()), -1);

	// 범위 지정
	EXPECT_EQ(view.FindReverse(0, 6, C<TypeParam>("abcdefg").Source()), 0);
	EXPECT_EQ(view.FindReverse(1, 6, C<TypeParam>("abcdefg").Source()), -1);
	EXPECT_EQ(view.FindReverse(0, 5, C<TypeParam>("abcdefg").Source()), -1);

	// StringView와 String 타입
	auto sCd = C<TypeParam>("cd");
	V<TypeParam> searchView(sCd.Source());
	TStr<TypeParam> searchStr = C<TypeParam>("cd");
	EXPECT_EQ(view.FindReverse(searchView), 2);
	EXPECT_EQ(view.FindReverse(searchStr), 2);
}

// StringView::StartWith 테스트
TYPED_TEST(StringViewTypedTest, StartWith) {
	auto pStr = C<TypeParam>("abcdefg");
	V<TypeParam> view(pStr.Source());

	EXPECT_TRUE(view.StartWith(V<TypeParam>(C<TypeParam>("a").Source())));
	EXPECT_TRUE(view.StartWith(V<TypeParam>(C<TypeParam>("abc").Source())));
	EXPECT_TRUE(view.StartWith(V<TypeParam>(C<TypeParam>("abcdefg").Source())));
	EXPECT_FALSE(view.StartWith(V<TypeParam>(C<TypeParam>("b").Source())));
	EXPECT_FALSE(view.StartWith(V<TypeParam>(C<TypeParam>("abcdefgh").Source())));

	// String 타입
	TStr<TypeParam> prefixStr = C<TypeParam>("abc");
	EXPECT_TRUE(view.StartWith(prefixStr));
	EXPECT_FALSE(view.StartWith(C<TypeParam>("xyz")));
}

// StringView::EndWith 테스트
TYPED_TEST(StringViewTypedTest, EndWith) {
	auto pStr = C<TypeParam>("abcdefg");
	V<TypeParam> view(pStr.Source());

	EXPECT_TRUE(view.EndWith(V<TypeParam>(C<TypeParam>("g").Source())));
	EXPECT_TRUE(view.EndWith(V<TypeParam>(C<TypeParam>("efg").Source())));
	EXPECT_TRUE(view.EndWith(V<TypeParam>(C<TypeParam>("abcdefg").Source())));
	EXPECT_FALSE(view.EndWith(V<TypeParam>(C<TypeParam>("f").Source())));
	EXPECT_FALSE(view.EndWith(V<TypeParam>(C<TypeParam>("xabcdefg").Source())));

	// String 타입
	TStr<TypeParam> suffixStr = C<TypeParam>("efg");
	EXPECT_TRUE(view.EndWith(suffixStr));
	EXPECT_FALSE(view.EndWith(C<TypeParam>("xyz")));
}

// StringView::Contain 테스트
TYPED_TEST(StringViewTypedTest, Contain) {
	auto pStr = C<TypeParam>("abcdefg");
	V<TypeParam> view(pStr.Source());

	EXPECT_TRUE(view.Contain(C<TypeParam>("a").Source()));
	EXPECT_TRUE(view.Contain(C<TypeParam>("bcd").Source()));
	EXPECT_TRUE(view.Contain(C<TypeParam>("g").Source()));
	EXPECT_FALSE(view.Contain(C<TypeParam>("xyz").Source()));
	EXPECT_FALSE(view.Contain(C<TypeParam>("abcdefgh").Source()));

	// StringView와 String 타입
	auto sCd = C<TypeParam>("cd");
	V<TypeParam> searchView(sCd.Source());
	TStr<TypeParam> searchStr = C<TypeParam>("cd");
	EXPECT_TRUE(view.Contain(searchView));
	EXPECT_TRUE(view.Contain(searchStr));
}

// StringView::Count 테스트
TYPED_TEST(StringViewTypedTest, Count) {
	auto pStr = C<TypeParam>("aabbccaa");
	V<TypeParam> view(pStr.Source());

	EXPECT_EQ(view.Count(C<TypeParam>("a").Source()), 4);
	EXPECT_EQ(view.Count(C<TypeParam>("aa").Source()), 2);
	EXPECT_EQ(view.Count(C<TypeParam>("b").Source()), 2);
	EXPECT_EQ(view.Count(C<TypeParam>("c").Source()), 2);
	EXPECT_EQ(view.Count(C<TypeParam>("cc").Source()), 1);
	EXPECT_EQ(view.Count(C<TypeParam>("xyz").Source()), 0);

	// StringView와 String 타입
	auto sAa = C<TypeParam>("aa");
	V<TypeParam> searchView(sAa.Source());
	TStr<TypeParam> searchStr = C<TypeParam>("aa");
	EXPECT_EQ(view.Count(searchView), 2);
	EXPECT_EQ(view.Count(searchStr), 2);

	// 범위 지정
	EXPECT_EQ(view.Count(0, 3, C<TypeParam>("a").Source()), 2);
	EXPECT_EQ(view.Count(4, 7, C<TypeParam>("a").Source()), 2);
}

// StringView::Compare 테스트
TYPED_TEST(StringViewTypedTest, Compare) {
	auto pStr1 = C<TypeParam>("abcd");
	auto pStr2 = C<TypeParam>("abc");
	auto pStr3 = C<TypeParam>("abcd");
	auto pStr4 = C<TypeParam>("abce");

	V<TypeParam> view1(pStr1.Source());
	V<TypeParam> view2(pStr2.Source());
	V<TypeParam> view3(pStr3.Source());
	V<TypeParam> view4(pStr4.Source());

	EXPECT_EQ(view1.Compare(view3), 0);  // abcd == abcd
	EXPECT_GT(view1.Compare(view2), 0);  // abcd > abc
	EXPECT_LT(view2.Compare(view1), 0);  // abc < abcd
	EXPECT_LT(view1.Compare(view4), 0);  // abcd < abce

	// Compare with CharT*
	EXPECT_EQ(view1.Compare(C<TypeParam>("abcd").Source()), 0);
	EXPECT_GT(view1.Compare(C<TypeParam>("abc").Source()), 0);

	// Compare with String
	TStr<TypeParam> str2 = C<TypeParam>("abc");
	EXPECT_GT(view1.Compare(str2), 0);

	TStr<TypeParam> a = C<TypeParam>("abcdefg");
	TStr<TypeParam> b = C<TypeParam>("");
	TStr<TypeParam> c(0);
	std::vector<V<TypeParam>> vec;

	vec.push_back(V<TypeParam>(a));
	vec.push_back(V<TypeParam>(b));
	vec.push_back(V<TypeParam>(c));

	EXPECT_TRUE(vec[0] == V<TypeParam>(a));
	EXPECT_TRUE(vec[0] == a);

	EXPECT_TRUE(vec[1] == V<TypeParam>(b));
	EXPECT_TRUE(vec[1] == b);
	EXPECT_TRUE(vec[1] == C<TypeParam>(""));
	EXPECT_TRUE(vec[1].Length() == 0);

	EXPECT_TRUE(vec[2] == V<TypeParam>(c));
	EXPECT_TRUE(vec[2] == c);
	EXPECT_TRUE(vec[2] == C<TypeParam>(""));
	EXPECT_TRUE(vec[2].Length() == 0);
}

// StringView::Comparison Operators 테스트
TYPED_TEST(StringViewTypedTest, ComparisonOperators) {
	auto pStr1 = C<TypeParam>("abcd");
	auto pStr2 = C<TypeParam>("abc");
	auto pStr3 = C<TypeParam>("abcd");

	V<TypeParam> view1(pStr1.Source());
	V<TypeParam> view2(pStr2.Source());
	V<TypeParam> view3(pStr3.Source());

	// Equality
	EXPECT_TRUE(view1 == view3);
	EXPECT_TRUE(view1 == C<TypeParam>("abcd"));
	EXPECT_FALSE(view1 == view2);

	// Less than
	EXPECT_TRUE(view2 < view1);
	EXPECT_FALSE(view1 < view2);

	// Greater than
	EXPECT_TRUE(view1 > view2);
	EXPECT_FALSE(view2 > view1);

	// Less than or equal
	EXPECT_TRUE(view1 <= view3);
	EXPECT_TRUE(view2 <= view1);

	// Greater than or equal
	EXPECT_TRUE(view1 >= view3);
	EXPECT_TRUE(view1 >= view2);

	// String 타입
	TStr<TypeParam> str2 = C<TypeParam>("abc");
	EXPECT_TRUE(view1 > str2);
	EXPECT_TRUE(view1 >= str2);
	EXPECT_FALSE(view1 < str2);
}

// StringView::Split 테스트
TYPED_TEST(StringViewTypedTest, Split) {
	auto pStr1 = C<TypeParam>("abcd_cd_efg");
	auto pStr2 = C<TypeParam>("___");
	auto pStr3 = C<TypeParam>("_bbbbb_");
	auto pStr4 = C<TypeParam>("a,b,c");

	V<TypeParam> view1(pStr1.Source());
	V<TypeParam> view2(pStr2.Source());
	V<TypeParam> view3(pStr3.Source());
	V<TypeParam> view4(pStr4.Source());

	// CharT* delimiter with includeEmpty = true
	{
		LeakCheck;
		auto tokens = view1.Split(C<TypeParam>("_").Source(), true);
		EXPECT_EQ(tokens.Size(), 3);
		if (tokens.Size() == 3) {
			EXPECT_TRUE(tokens[0] == V<TypeParam>(C<TypeParam>("abcd").Source(), 4));
			EXPECT_TRUE(tokens[1] == V<TypeParam>(C<TypeParam>("cd").Source(), 2));
			EXPECT_TRUE(tokens[2] == V<TypeParam>(C<TypeParam>("efg").Source(), 3));
		}
	}

	// char delimiter
	{
		LeakCheck;
		auto tokens = view4.Split(TypeParam(','), true);
		EXPECT_EQ(tokens.Size(), 3);
		if (tokens.Size() == 3) {
			EXPECT_TRUE(tokens[0] == V<TypeParam>(C<TypeParam>("a").Source(), 1));
			EXPECT_TRUE(tokens[1] == V<TypeParam>(C<TypeParam>("b").Source(), 1));
			EXPECT_TRUE(tokens[2] == V<TypeParam>(C<TypeParam>("c").Source(), 1));
		}
	}

	// Multiple delimiters
	{
		LeakCheck;
		auto tokens = view2.Split(C<TypeParam>("_").Source(), true);
		EXPECT_EQ(tokens.Size(), 4);
	}

	// With includeEmpty = false
	{
		LeakCheck;
		auto tokens = view3.Split(C<TypeParam>("_").Source(), false);
		EXPECT_EQ(tokens.Size(), 1);
	}
}

// StringView::SubStr 테스트
TYPED_TEST(StringViewTypedTest, SubStr) {
	auto pStr = C<TypeParam>("abcdefg");
	V<TypeParam> view(pStr.Source());

	V<TypeParam> sub1 = view.SubStr(0, 3);
	EXPECT_EQ(sub1.Length(), 3);
	EXPECT_TRUE(sub1 == V<TypeParam>(C<TypeParam>("abc").Source(), 3));

	V<TypeParam> sub2 = view.SubStr(3, 2);
	EXPECT_EQ(sub2.Length(), 2);
	EXPECT_TRUE(sub2 == V<TypeParam>(C<TypeParam>("de").Source(), 2));

	// Count exceeds remaining length
	V<TypeParam> sub3 = view.SubStr(5, 10);
	EXPECT_EQ(sub3.Length(), 2);

	// Invalid start index
	V<TypeParam> sub4 = view.SubStr(10, 5);
	EXPECT_TRUE(sub4.IsEmpty());
}

// StringView::GetRange 테스트
TYPED_TEST(StringViewTypedTest, GetRange) {
	auto pStr = C<TypeParam>("abcdefg");
	V<TypeParam> view(pStr.Source());

	V<TypeParam> range1 = view.GetRange(0, 2);
	EXPECT_EQ(range1.Length(), 3);
	EXPECT_TRUE(range1 == V<TypeParam>(C<TypeParam>("abc").Source(), 3));

	V<TypeParam> range2 = view.GetRange(3, 5);
	EXPECT_EQ(range2.Length(), 3);
	EXPECT_TRUE(range2 == V<TypeParam>(C<TypeParam>("def").Source(), 3));

	V<TypeParam> range3 = view.GetRange(6, 6);
	EXPECT_EQ(range3.Length(), 1);
	EXPECT_TRUE(range3 == V<TypeParam>(C<TypeParam>("g").Source(), 1));

	// Invalid range
	V<TypeParam> range4 = view.GetRange(5, 3);
	EXPECT_TRUE(range4.IsEmpty());
}

// StringView::ToLowerCase 테스트
TYPED_TEST(StringViewTypedTest, ToLowerCase) {
	auto pStr = C<TypeParam>("AbCdEfG");
	V<TypeParam> view(pStr.Source());

	TStr<TypeParam> lower = view.ToLowerCase();
	EXPECT_EQ(lower.Length(), 7);
	EXPECT_EQ(lower[0], TypeParam('a'));
	EXPECT_EQ(lower[1], TypeParam('b'));
	EXPECT_EQ(lower[2], TypeParam('c'));

	// Empty StringView
	V<TypeParam> emptyView;
	TStr<TypeParam> emptyLower = emptyView.ToLowerCase();
	EXPECT_TRUE(emptyLower.IsEmpty());
}

// StringView::ToUpperCase 테스트
TYPED_TEST(StringViewTypedTest, ToUpperCase) {
	auto pStr = C<TypeParam>("AbCdEfG");
	V<TypeParam> view(pStr.Source());

	TStr<TypeParam> upper = view.ToUpperCase();
	EXPECT_EQ(upper.Length(), 7);
	EXPECT_EQ(upper[0], TypeParam('A'));
	EXPECT_EQ(upper[1], TypeParam('B'));
	EXPECT_EQ(upper[2], TypeParam('C'));

	// Empty StringView
	V<TypeParam> emptyView;
	TStr<TypeParam> emptyUpper = emptyView.ToUpperCase();
	EXPECT_TRUE(emptyUpper.IsEmpty());
}

// StringView::Type Conversion 테스트 - ToInt32, ToUInt32 등
TYPED_TEST(StringViewTypedTest, TypeConversion) {
	auto pStr1 = C<TypeParam>("12345");
	auto pStr2 = C<TypeParam>("3.14");
	auto pStr3 = C<TypeParam>("-100");

	V<TypeParam> view1(pStr1.Source());
	V<TypeParam> view2(pStr2.Source());
	V<TypeParam> view3(pStr3.Source());

	// ToInt32
	EXPECT_EQ(view1.ToInt32(), 12345);
	EXPECT_EQ(view3.ToInt32(), -100);

	// ToUInt32
	EXPECT_EQ(view1.ToUInt32(), 12345);

	// ToFloat
	_f32 floatVal = view2.ToFloat();
	EXPECT_TRUE(floatVal > 3.1f && floatVal < 3.2f);

	// ToDouble
	_f64 doubleVal = view2.ToDouble();
	EXPECT_TRUE(doubleVal > 3.1 && doubleVal < 3.2);

	// ToInt8
	auto pInt8Str = C<TypeParam>("127");
	V<TypeParam> int8View(pInt8Str.Source());
	EXPECT_EQ(int8View.ToInt8(), 127);

	// ToInt16
	auto pInt16Str = C<TypeParam>("32000");
	V<TypeParam> int16View(pInt16Str.Source());
	EXPECT_EQ(int16View.ToInt16(), 32000);

	// ToInt64
	auto pInt64Str = C<TypeParam>("9223372036854775807");
	V<TypeParam> int64View(pInt64Str.Source());
	EXPECT_EQ(int64View.ToInt64(), 9223372036854775807LL);
}

// StringView::TryToXXX 테스트
TYPED_TEST(StringViewTypedTest, TryTypeConversion) {
	auto pValidInt = C<TypeParam>("12345");
	auto pInvalidInt = C<TypeParam>("abc");

	V<TypeParam> validView(pValidInt.Source());
	V<TypeParam> invalidView(pInvalidInt.Source());

	_s32 result = 0;
	EXPECT_TRUE(validView.TryToInt32(result));
	EXPECT_EQ(result, 12345);

	_s32 result2 = 999;
	EXPECT_FALSE(invalidView.TryToInt32(result2));
	EXPECT_EQ(result2, 999);  // Should remain unchanged

	// TryToFloat
	auto pFloatStr = C<TypeParam>("3.14");
	V<TypeParam> floatView(pFloatStr.Source());
	_f32 floatResult = 0.0f;
	EXPECT_TRUE(floatView.TryToFloat(floatResult));
	EXPECT_TRUE(floatResult > 3.1f && floatResult < 3.2f);

	// TryToDouble
	_f64 doubleResult = 0.0;
	EXPECT_TRUE(floatView.TryToDouble(doubleResult));
	EXPECT_TRUE(doubleResult > 3.1 && doubleResult < 3.2);

	// TryToInt8
	_s8 int8Result = 0;
	auto pInt8Str = C<TypeParam>("100");
	V<TypeParam> int8View(pInt8Str.Source());
	EXPECT_TRUE(int8View.TryToInt8(int8Result));
	EXPECT_EQ(int8Result, 100);

	// TryToUInt32
	_u32 uint32Result = 0;
	EXPECT_TRUE(validView.TryToUInt32(uint32Result));
	EXPECT_EQ(uint32Result, 12345);

	// TryToInt64
	_s64 int64Result = 0;
	auto pInt64Str = C<TypeParam>("9223372036854775807");
	V<TypeParam> int64View(pInt64Str.Source());
	EXPECT_TRUE(int64View.TryToInt64(int64Result));
	EXPECT_EQ(int64Result, 9223372036854775807LL);
}

// StringView 생성자에서 String 객체 사용 테스트
TYPED_TEST(StringViewTypedTest, StringInteroperability) {
	TStr<TypeParam> szStr = C<TypeParam>("hello world");
	V<TypeParam> view(szStr);

	EXPECT_EQ(view.Length(), 11);
	EXPECT_EQ(view.Source(), szStr.Source());
	EXPECT_TRUE(view == C<TypeParam>("hello world"));
	EXPECT_TRUE(view == szStr);

	// 문자열 길이 1 테스트
	TStr<TypeParam> szStr2 = C<TypeParam>("a");
	V<TypeParam> view2(szStr2);
	EXPECT_EQ(view2.Length(), 1);
	EXPECT_EQ(view2[0], TypeParam('a'));
}

// StringView 엣지 케이스 테스트
TYPED_TEST(StringViewTypedTest, EdgeCases) {
	// Null StringView with operations
	V<TypeParam> nullView;
	EXPECT_EQ(nullView.Find(C<TypeParam>("test").Source()), -1);
	EXPECT_EQ(nullView.Count(C<TypeParam>("test").Source()), 0);
	EXPECT_FALSE(nullView.Contain(C<TypeParam>("test").Source()));
	EXPECT_TRUE(nullView.IsNull());
	EXPECT_TRUE(nullView.IsEmpty());

	// Empty but non-null StringView
	auto sEmptyStr = C<TypeParam>("");
	V<TypeParam> emptyView(sEmptyStr.Source(), 0);
	EXPECT_FALSE(emptyView.IsNull());
	EXPECT_TRUE(emptyView.IsEmpty());
	EXPECT_EQ(emptyView.First(), TypeParam('\0'));
	EXPECT_EQ(emptyView.Last(), TypeParam('\0'));

	// Single character
	auto pSingleChar = C<TypeParam>("x");
	V<TypeParam> singleView(pSingleChar.Source());
	EXPECT_EQ(singleView.Length(), 1);
	EXPECT_EQ(singleView.First(), TypeParam('x'));
	EXPECT_EQ(singleView.Last(), TypeParam('x'));
	EXPECT_TRUE(singleView == C<TypeParam>("x"));
}

// StringView 범위 검색 테스트
TYPED_TEST(StringViewTypedTest, RangeOperations) {
	auto pStr = C<TypeParam>("0123456789");
	V<TypeParam> view(pStr.Source());

	// Count with range
	EXPECT_EQ(view.Count(0, 4, C<TypeParam>("1").Source()), 1);
	EXPECT_EQ(view.Count(5, 9, C<TypeParam>("1").Source()), 0);

	// Find with range
	EXPECT_EQ(view.Find(2, 8, C<TypeParam>("456").Source()), 4);
	EXPECT_EQ(view.Find(0, 2, C<TypeParam>("456").Source()), -1);

	// FindReverse with range
	EXPECT_EQ(view.FindReverse(0, 8, C<TypeParam>("7").Source()), 7);
}

// StringView caseSensitive 검색 테스트
TYPED_TEST(StringViewTypedTest, FindCaseSensitive) {
	auto pStr = C<TypeParam>("Hello World ABCDEFG");
	V<TypeParam> view(pStr.Source());

	// caseSensitive = true (기본값)
	EXPECT_EQ(view.Find(C<TypeParam>("Hello").Source()), 0);
	EXPECT_EQ(view.Find(C<TypeParam>("hello").Source()), -1);
	EXPECT_EQ(view.Find(C<TypeParam>("WORLD").Source()), -1);
	EXPECT_EQ(view.Find(C<TypeParam>("ABCDEFG").Source()), 12);

	// caseSensitive = false
	EXPECT_EQ(view.Find(C<TypeParam>("hello").Source(), false), 0);
	EXPECT_EQ(view.Find(C<TypeParam>("HELLO").Source(), false), 0);
	EXPECT_EQ(view.Find(C<TypeParam>("world").Source(), false), 6);
	EXPECT_EQ(view.Find(C<TypeParam>("WORLD").Source(), false), 6);
	EXPECT_EQ(view.Find(C<TypeParam>("abcdefg").Source(), false), 12);

	// 범위 지정 + caseSensitive = false
	EXPECT_EQ(view.Find(0, 4, C<TypeParam>("hello").Source(), false), 0);
	EXPECT_EQ(view.Find(1, 4, C<TypeParam>("hello").Source(), false), -1);
	EXPECT_EQ(view.Find(6, 10, C<TypeParam>("world").Source(), false), 6);
}

// StringView::FindReverse caseSensitive 테스트
TYPED_TEST(StringViewTypedTest, FindReverseCaseSensitive) {
	auto pStr = C<TypeParam>("AbcABCAbc");
	V<TypeParam> view(pStr.Source());

	// caseSensitive = true (기본값)
	EXPECT_EQ(view.FindReverse(C<TypeParam>("Abc").Source()), 6);
	EXPECT_EQ(view.FindReverse(C<TypeParam>("abc").Source()), -1);
	EXPECT_EQ(view.FindReverse(C<TypeParam>("ABC").Source()), 3);

	// caseSensitive = false
	EXPECT_EQ(view.FindReverse(C<TypeParam>("abc").Source(), false), 6);
	EXPECT_EQ(view.FindReverse(C<TypeParam>("ABC").Source(), false), 6);

	// 범위 지정 + caseSensitive = false
	EXPECT_EQ(view.FindReverse(0, 5, C<TypeParam>("abc").Source(), false), 3);
	EXPECT_EQ(view.FindReverse(0, 2, C<TypeParam>("abc").Source(), false), 0);
}

// StringView::Count caseSensitive 테스트
TYPED_TEST(StringViewTypedTest, CountCaseSensitive) {
	auto pStr = C<TypeParam>("HellohelloHELLO");
	V<TypeParam> view(pStr.Source());

	// caseSensitive = true (기본값)
	EXPECT_EQ(view.Count(C<TypeParam>("hello").Source()), 1);
	EXPECT_EQ(view.Count(C<TypeParam>("Hello").Source()), 1);
	EXPECT_EQ(view.Count(C<TypeParam>("HELLO").Source()), 1);

	// caseSensitive = false
	EXPECT_EQ(view.Count(C<TypeParam>("hello").Source(), false), 3);
	EXPECT_EQ(view.Count(C<TypeParam>("HELLO").Source(), false), 3);

	// 범위 지정 + caseSensitive = false
	EXPECT_EQ(view.Count(0, 9, C<TypeParam>("hello").Source(), false), 2);
	EXPECT_EQ(view.Count(5, 14, C<TypeParam>("hello").Source(), false), 2);
}

// StringView::StartWith caseSensitive 테스트
TYPED_TEST(StringViewTypedTest, StartWithCaseSensitive) {
	auto pStr = C<TypeParam>("HelloWorld");
	V<TypeParam> view(pStr.Source());

	// caseSensitive = true (기본값)
	EXPECT_TRUE(view.StartWith(V<TypeParam>(C<TypeParam>("Hello").Source())));
	EXPECT_FALSE(view.StartWith(V<TypeParam>(C<TypeParam>("hello").Source())));

	// caseSensitive = false
	EXPECT_TRUE(view.StartWith(V<TypeParam>(C<TypeParam>("hello").Source()), false));
	EXPECT_TRUE(view.StartWith(V<TypeParam>(C<TypeParam>("HELLO").Source()), false));
	EXPECT_FALSE(view.StartWith(V<TypeParam>(C<TypeParam>("world").Source()), false));

	// String 타입 + caseSensitive = false
	EXPECT_TRUE(view.StartWith(C<TypeParam>("hello"), false));
	EXPECT_TRUE(view.StartWith(C<TypeParam>("HELLO"), false));
}

// StringView::EndWith caseSensitive 테스트
TYPED_TEST(StringViewTypedTest, EndWithCaseSensitive) {
	auto pStr = C<TypeParam>("HelloWorld");
	V<TypeParam> view(pStr.Source());

	// caseSensitive = true (기본값)
	EXPECT_TRUE(view.EndWith(V<TypeParam>(C<TypeParam>("World").Source())));
	EXPECT_FALSE(view.EndWith(V<TypeParam>(C<TypeParam>("world").Source())));

	// caseSensitive = false
	EXPECT_TRUE(view.EndWith(V<TypeParam>(C<TypeParam>("world").Source()), false));
	EXPECT_TRUE(view.EndWith(V<TypeParam>(C<TypeParam>("WORLD").Source()), false));
	EXPECT_FALSE(view.EndWith(V<TypeParam>(C<TypeParam>("hello").Source()), false));

	// String 타입 + caseSensitive = false
	EXPECT_TRUE(view.EndWith(C<TypeParam>("world"), false));
	EXPECT_TRUE(view.EndWith(C<TypeParam>("WORLD"), false));
}

// StringView::Contain caseSensitive 테스트
TYPED_TEST(StringViewTypedTest, ContainCaseSensitive) {
	auto pStr = C<TypeParam>("Hello World");
	V<TypeParam> view(pStr.Source());

	// caseSensitive = true (기본값)
	EXPECT_TRUE(view.Contain(C<TypeParam>("World").Source()));
	EXPECT_FALSE(view.Contain(C<TypeParam>("world").Source()));
	EXPECT_FALSE(view.Contain(C<TypeParam>("HELLO").Source()));

	// caseSensitive = false
	EXPECT_TRUE(view.Contain(C<TypeParam>("world").Source(), false));
	EXPECT_TRUE(view.Contain(C<TypeParam>("HELLO").Source(), false));
	EXPECT_TRUE(view.Contain(C<TypeParam>("hello world").Source(), false));
	EXPECT_FALSE(view.Contain(C<TypeParam>("xyz").Source(), false));

	// StringView 타입 + caseSensitive = false
	EXPECT_TRUE(view.Contain(V<TypeParam>(C<TypeParam>("WORLD").Source()), false));

	// String 타입 + caseSensitive = false
	EXPECT_TRUE(view.Contain(C<TypeParam>("hello"), false));
}

#endif // TEST_StringViewTest == ON
