/*
	작성자 : 윤정도
	StringView 클래스 테스트입니다.
*/


#include <jc_gtest/Core.h>
#include <jc/Core.h>
#include <jc/Primitives/StringView.h>
#include <jc/Primitives/String.h>
#include <jc/Container/Vector.h>


using namespace std;

#if TEST_StringViewTest == ON

// StringView 생성자 테스트
TEST(StringViewTest, Constructor) {
	// 기본 생성자
	StringView view1;
	EXPECT_TRUE(view1.IsNull());
	EXPECT_TRUE(view1.IsEmpty());
	EXPECT_EQ(view1.Length(), 0);

	// char* 생성자
	const char* pStr = "abcd";
	StringView view2(const_cast<char*>(pStr));
	EXPECT_FALSE(view2.IsNull());
	EXPECT_FALSE(view2.IsEmpty());
	EXPECT_EQ(view2.Length(), 4);
	EXPECT_EQ(view2.Source(), pStr);

	// char*, length 생성자
	StringView view3(const_cast<char*>(pStr), 2);
	EXPECT_EQ(view3.Length(), 2);

	// String 생성자
	String szStr = "hello";
	StringView view4(szStr);
	EXPECT_EQ(view4.Length(), 5);
	EXPECT_EQ(view4.Source(), szStr.Source());
}

// StringView 기본 정보 조회 테스트
TEST(StringViewTest, BasicInformation) {
	const char* pStr = "test";
	StringView view(const_cast<char*>(pStr));

	EXPECT_EQ(view.Length(), 4);
	EXPECT_EQ(view.LengthWithNull(), 5);
	EXPECT_FALSE(view.IsEmpty());
	EXPECT_FALSE(view.IsNull());
	EXPECT_EQ(view.Source(), pStr);
	EXPECT_STREQ(view.SafeSource(), "test");

	// Null StringView
	StringView nullView;
	EXPECT_TRUE(nullView.IsNull());
	EXPECT_STREQ(nullView.SafeSource(), "");

	// Empty StringView
	StringView emptyView(const_cast<char*>(""), 0);
	EXPECT_TRUE(emptyView.IsEmpty());
	EXPECT_FALSE(emptyView.IsNull());
}

// StringView 인덱스 검증 테스트
TEST(StringViewTest, IndexValidation) {
	const char* pStr = "abcde";
	StringView view(const_cast<char*>(pStr));

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
TEST(StringViewTest, CharacterAccess) {
	const char* pStr = "abcde";
	StringView view(const_cast<char*>(pStr));

	EXPECT_EQ(view.GetAt(0), 'a');
	EXPECT_EQ(view.GetAt(4), 'e');
	EXPECT_EQ(view.GetAt(5), '\0');
	EXPECT_EQ(view.GetAt(-1), '\0');

	EXPECT_EQ(view.First(), 'a');
	EXPECT_EQ(view.Last(), 'e');

	EXPECT_EQ(view[0], 'a');
	EXPECT_EQ(view[4], 'e');
	EXPECT_EQ(view[5], '\0');

	// Empty StringView
	StringView emptyView;
	EXPECT_EQ(emptyView.First(), '\0');
	EXPECT_EQ(emptyView.Last(), '\0');
}

// StringView::Find 테스트
TEST(StringViewTest, Find) {
	const char* pStr = "abcdefg";
	StringView view(const_cast<char*>(pStr));

	// 기본 Find
	EXPECT_EQ(view.Find("g"), 6);
	EXPECT_EQ(view.Find("fg"), 5);
	EXPECT_EQ(view.Find("efg"), 4);
	EXPECT_EQ(view.Find("defg"), 3);
	EXPECT_EQ(view.Find("cdefg"), 2);
	EXPECT_EQ(view.Find("bcdefg"), 1);
	EXPECT_EQ(view.Find("abcdefg"), 0);
	EXPECT_EQ(view.Find("-abcdefg"), -1);
	EXPECT_EQ(view.Find("abcdefg-"), -1);

	// 왼쪽에서 검사
	EXPECT_EQ(view.Find("a"), 0);
	EXPECT_EQ(view.Find("ab"), 0);
	EXPECT_EQ(view.Find("abc"), 0);

	// 시작 위치 지정
	EXPECT_EQ(view.Find(0, "a"), 0);
	EXPECT_EQ(view.Find(1, "a"), -1);
	EXPECT_EQ(view.Find(4, "e"), 4);

	// 범위 지정
	EXPECT_EQ(view.Find(0, 2, "abc"), 0);
	EXPECT_EQ(view.Find(1, 2, "abc"), -1);
	EXPECT_EQ(view.Find(4, 6, "efg"), 4);
	EXPECT_EQ(view.Find(0, 5, "abcdefg"), -1);

	// StringView와 String 타입
	StringView searchView(const_cast<char*>("cd"));
	String searchStr("cd");
	EXPECT_EQ(view.Find(searchView), 2);
	EXPECT_EQ(view.Find(searchStr), 2);
}

// StringView::FindReverse 테스트
TEST(StringViewTest, FindReverse) {
	const char* pStr = "abcdefg";
	StringView view(const_cast<char*>(pStr));

	// 기본 FindReverse
	EXPECT_EQ(view.FindReverse("g"), 6);
	EXPECT_EQ(view.FindReverse("fg"), 5);
	EXPECT_EQ(view.FindReverse("efg"), 4);
	EXPECT_EQ(view.FindReverse("abcdefg"), 0);
	EXPECT_EQ(view.FindReverse("-abcdefg"), -1);

	// 범위 지정
	EXPECT_EQ(view.FindReverse(0, 6, "abcdefg"), 0);
	EXPECT_EQ(view.FindReverse(1, 6, "abcdefg"), -1);
	EXPECT_EQ(view.FindReverse(0, 5, "abcdefg"), -1);

	// StringView와 String 타입
	StringView searchView(const_cast<char*>("cd"));
	String searchStr("cd");
	EXPECT_EQ(view.FindReverse(searchView), 2);
	EXPECT_EQ(view.FindReverse(searchStr), 2);
}

// StringView::StartWith 테스트
TEST(StringViewTest, StartWith) {
	const char* pStr = "abcdefg";
	StringView view(const_cast<char*>(pStr));

	EXPECT_TRUE(view.StartWith(StringView(const_cast<char*>("a"))));
	EXPECT_TRUE(view.StartWith(StringView(const_cast<char*>("abc"))));
	EXPECT_TRUE(view.StartWith(StringView(const_cast<char*>("abcdefg"))));
	EXPECT_FALSE(view.StartWith(StringView(const_cast<char*>("b"))));
	EXPECT_FALSE(view.StartWith(StringView(const_cast<char*>("abcdefgh"))));

	// String 타입
	String prefixStr("abc");
	EXPECT_TRUE(view.StartWith(prefixStr));
	EXPECT_FALSE(view.StartWith(String("xyz")));
}

// StringView::EndWith 테스트
TEST(StringViewTest, EndWith) {
	const char* pStr = "abcdefg";
	StringView view(const_cast<char*>(pStr));

	EXPECT_TRUE(view.EndWith(StringView(const_cast<char*>("g"))));
	EXPECT_TRUE(view.EndWith(StringView(const_cast<char*>("efg"))));
	EXPECT_TRUE(view.EndWith(StringView(const_cast<char*>("abcdefg"))));
	EXPECT_FALSE(view.EndWith(StringView(const_cast<char*>("f"))));
	EXPECT_FALSE(view.EndWith(StringView(const_cast<char*>("xabcdefg"))));

	// String 타입
	String suffixStr("efg");
	EXPECT_TRUE(view.EndWith(suffixStr));
	EXPECT_FALSE(view.EndWith(String("xyz")));
}

// StringView::Contain 테스트
TEST(StringViewTest, Contain) {
	const char* pStr = "abcdefg";
	StringView view(const_cast<char*>(pStr));

	EXPECT_TRUE(view.Contain("a"));
	EXPECT_TRUE(view.Contain("bcd"));
	EXPECT_TRUE(view.Contain("g"));
	EXPECT_FALSE(view.Contain("xyz"));
	EXPECT_FALSE(view.Contain("abcdefgh"));

	// StringView와 String 타입
	StringView searchView(const_cast<char*>("cd"));
	String searchStr("cd");
	EXPECT_TRUE(view.Contain(searchView));
	EXPECT_TRUE(view.Contain(searchStr));
}

// StringView::Count 테스트
TEST(StringViewTest, Count) {
	const char* pStr = "aabbccaa";
	StringView view(const_cast<char*>(pStr));

	EXPECT_EQ(view.Count("a"), 4);
	EXPECT_EQ(view.Count("aa"), 2);
	EXPECT_EQ(view.Count("b"), 2);
	EXPECT_EQ(view.Count("c"), 2);
	EXPECT_EQ(view.Count("cc"), 1);
	EXPECT_EQ(view.Count("xyz"), 0);

	// StringView와 String 타입
	StringView searchView(const_cast<char*>("aa"));
	String searchStr("aa");
	EXPECT_EQ(view.Count(searchView), 2);
	EXPECT_EQ(view.Count(searchStr), 2);

	// 범위 지정
	EXPECT_EQ(view.Count(0, 3, "a"), 2);
	EXPECT_EQ(view.Count(4, 7, "a"), 2);
}

// StringView::Compare 테스트
TEST(StringViewTest, Compare) {
	const char* pStr1 = "abcd";
	const char* pStr2 = "abc";
	const char* pStr3 = "abcd";
	const char* pStr4 = "abce";

	StringView view1(const_cast<char*>(pStr1));
	StringView view2(const_cast<char*>(pStr2));
	StringView view3(const_cast<char*>(pStr3));
	StringView view4(const_cast<char*>(pStr4));

	EXPECT_EQ(view1.Compare(view3), 0);  // abcd == abcd
	EXPECT_GT(view1.Compare(view2), 0);  // abcd > abc
	EXPECT_LT(view2.Compare(view1), 0);  // abc < abcd
	EXPECT_LT(view1.Compare(view4), 0);  // abcd < abce

	// Compare with char*
	EXPECT_EQ(view1.Compare("abcd"), 0);
	EXPECT_GT(view1.Compare("abc"), 0);

	// Compare with String
	String str2("abc");
	EXPECT_GT(view1.Compare(str2), 0);

	String a = "abcdefg";
	String b = "";
	String c(0);
	std::vector<StringView> vec;

	vec.push_back(StringView(a));
	vec.push_back(StringView(b));
	vec.push_back(StringView(c));

	EXPECT_TRUE(vec[0] == StringView(a));
	EXPECT_TRUE(vec[0] == a);

	EXPECT_TRUE(vec[1] == StringView(b));
	EXPECT_TRUE(vec[1] == b);
	EXPECT_TRUE(vec[1] == "");
	EXPECT_TRUE(vec[1].Length() == 0);

	EXPECT_TRUE(vec[2] == StringView(c));
	EXPECT_TRUE(vec[2] == c);
	EXPECT_TRUE(vec[2] == "");
	EXPECT_TRUE(vec[2].Length() == 0);
}

// StringView::Comparison Operators 테스트
TEST(StringViewTest, ComparisonOperators) {
	const char* pStr1 = "abcd";
	const char* pStr2 = "abc";
	const char* pStr3 = "abcd";

	StringView view1(const_cast<char*>(pStr1));
	StringView view2(const_cast<char*>(pStr2));
	StringView view3(const_cast<char*>(pStr3));

	// Equality
	EXPECT_TRUE(view1 == view3);
	EXPECT_TRUE(view1 == "abcd");
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
	String str2("abc");
	EXPECT_TRUE(view1 > str2);
	EXPECT_TRUE(view1 >= str2);
	EXPECT_FALSE(view1 < str2);
}

// StringView::Split 테스트
TEST(StringViewTest, Split) {
	const char* pStr1 = "abcd_cd_efg";
	const char* pStr2 = "___";
	const char* pStr3 = "_bbbbb_";
	const char* pStr4 = "a,b,c";

	StringView view1(const_cast<char*>(pStr1));
	StringView view2(const_cast<char*>(pStr2));
	StringView view3(const_cast<char*>(pStr3));
	StringView view4(const_cast<char*>(pStr4));

	// char* delimiter with includeEmpty = true
	{
		LeakCheck;
		auto tokens = view1.Split("_", true);
		EXPECT_EQ(tokens.Size(), 3);
		if (tokens.Size() == 3) {
			EXPECT_TRUE(tokens[0] == StringView(const_cast<char*>("abcd"), 4));
			EXPECT_TRUE(tokens[1] == StringView(const_cast<char*>("cd"), 2));
			EXPECT_TRUE(tokens[2] == StringView(const_cast<char*>("efg"), 3));
		}
	}

	// char delimiter
	{
		LeakCheck;
		auto tokens = view4.Split(',', true);
		EXPECT_EQ(tokens.Size(), 3);
		if (tokens.Size() == 3) {
			EXPECT_TRUE(tokens[0] == StringView(const_cast<char*>("a"), 1));
			EXPECT_TRUE(tokens[1] == StringView(const_cast<char*>("b"), 1));
			EXPECT_TRUE(tokens[2] == StringView(const_cast<char*>("c"), 1));
		}
	}

	// Multiple delimiters
	{
		LeakCheck;
		auto tokens = view2.Split("_", true);
		EXPECT_EQ(tokens.Size(), 4);
	}

	// With includeEmpty = false
	{
		LeakCheck;
		auto tokens = view3.Split("_", false);
		EXPECT_EQ(tokens.Size(), 1);
	}
}

// StringView::SubStr 테스트
TEST(StringViewTest, SubStr) {
	const char* pStr = "abcdefg";
	StringView view(const_cast<char*>(pStr));

	StringView sub1 = view.SubStr(0, 3);
	EXPECT_EQ(sub1.Length(), 3);
	EXPECT_TRUE(sub1 == StringView(const_cast<char*>("abc"), 3));

	StringView sub2 = view.SubStr(3, 2);
	EXPECT_EQ(sub2.Length(), 2);
	EXPECT_TRUE(sub2 == StringView(const_cast<char*>("de"), 2));

	// Count exceeds remaining length
	StringView sub3 = view.SubStr(5, 10);
	EXPECT_EQ(sub3.Length(), 2);

	// Invalid start index
	StringView sub4 = view.SubStr(10, 5);
	EXPECT_TRUE(sub4.IsEmpty());
}

// StringView::GetRange 테스트
TEST(StringViewTest, GetRange) {
	const char* pStr = "abcdefg";
	StringView view(const_cast<char*>(pStr));

	StringView range1 = view.GetRange(0, 2);
	EXPECT_EQ(range1.Length(), 3);
	EXPECT_TRUE(range1 == StringView(const_cast<char*>("abc"), 3));

	StringView range2 = view.GetRange(3, 5);
	EXPECT_EQ(range2.Length(), 3);
	EXPECT_TRUE(range2 == StringView(const_cast<char*>("def"), 3));

	StringView range3 = view.GetRange(6, 6);
	EXPECT_EQ(range3.Length(), 1);
	EXPECT_TRUE(range3 == StringView(const_cast<char*>("g"), 1));

	// Invalid range
	StringView range4 = view.GetRange(5, 3);
	EXPECT_TRUE(range4.IsEmpty());
}

// StringView::ToLowerCase 테스트
TEST(StringViewTest, ToLowerCase) {
	const char* pStr = "AbCdEfG";
	StringView view(const_cast<char*>(pStr));

	String lower = view.ToLowerCase();
	EXPECT_EQ(lower.Length(), 7);
	EXPECT_EQ(lower[0], 'a');
	EXPECT_EQ(lower[1], 'b');
	EXPECT_EQ(lower[2], 'c');

	// Empty StringView
	StringView emptyView;
	String emptyLower = emptyView.ToLowerCase();
	EXPECT_TRUE(emptyLower.IsEmpty());
}

// StringView::ToUpperCase 테스트
TEST(StringViewTest, ToUpperCase) {
	const char* pStr = "AbCdEfG";
	StringView view(const_cast<char*>(pStr));

	String upper = view.ToUpperCase();
	EXPECT_EQ(upper.Length(), 7);
	EXPECT_EQ(upper[0], 'A');
	EXPECT_EQ(upper[1], 'B');
	EXPECT_EQ(upper[2], 'C');

	// Empty StringView
	StringView emptyView;
	String emptyUpper = emptyView.ToUpperCase();
	EXPECT_TRUE(emptyUpper.IsEmpty());
}

// StringView::Type Conversion 테스트 - ToInt32, ToUInt32 등
TEST(StringViewTest, TypeConversion) {
	const char* pStr1 = "12345";
	const char* pStr2 = "3.14";
	const char* pStr3 = "-100";

	StringView view1(const_cast<char*>(pStr1));
	StringView view2(const_cast<char*>(pStr2));
	StringView view3(const_cast<char*>(pStr3));

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
	const char* pInt8Str = "127";
	StringView int8View(const_cast<char*>(pInt8Str));
	EXPECT_EQ(int8View.ToInt8(), 127);

	// ToInt16
	const char* pInt16Str = "32000";
	StringView int16View(const_cast<char*>(pInt16Str));
	EXPECT_EQ(int16View.ToInt16(), 32000);

	// ToInt64
	const char* pInt64Str = "9223372036854775807";
	StringView int64View(const_cast<char*>(pInt64Str));
	EXPECT_EQ(int64View.ToInt64(), 9223372036854775807LL);
}

// StringView::TryToXXX 테스트
TEST(StringViewTest, TryTypeConversion) {
	const char* pValidInt = "12345";
	const char* pInvalidInt = "abc";

	StringView validView(const_cast<char*>(pValidInt));
	StringView invalidView(const_cast<char*>(pInvalidInt));

	_s32 result = 0;
	EXPECT_TRUE(validView.TryToInt32(result));
	EXPECT_EQ(result, 12345);

	_s32 result2 = 999;
	EXPECT_FALSE(invalidView.TryToInt32(result2));
	EXPECT_EQ(result2, 999);  // Should remain unchanged

	// TryToFloat
	const char* pFloatStr = "3.14";
	StringView floatView(const_cast<char*>(pFloatStr));
	_f32 floatResult = 0.0f;
	EXPECT_TRUE(floatView.TryToFloat(floatResult));
	EXPECT_TRUE(floatResult > 3.1f && floatResult < 3.2f);

	// TryToDouble
	_f64 doubleResult = 0.0;
	EXPECT_TRUE(floatView.TryToDouble(doubleResult));
	EXPECT_TRUE(doubleResult > 3.1 && doubleResult < 3.2);

	// TryToInt8
	_s8 int8Result = 0;
	const char* pInt8Str = "100";
	StringView int8View(const_cast<char*>(pInt8Str));
	EXPECT_TRUE(int8View.TryToInt8(int8Result));
	EXPECT_EQ(int8Result, 100);

	// TryToUInt32
	_u32 uint32Result = 0;
	EXPECT_TRUE(validView.TryToUInt32(uint32Result));
	EXPECT_EQ(uint32Result, 12345);

	// TryToInt64
	_s64 int64Result = 0;
	const char* pInt64Str = "9223372036854775807";
	StringView int64View(const_cast<char*>(pInt64Str));
	EXPECT_TRUE(int64View.TryToInt64(int64Result));
	EXPECT_EQ(int64Result, 9223372036854775807LL);
}

// StringView 생성자에서 String 객체 사용 테스트
TEST(StringViewTest, StringInteroperability) {
	String szStr = "hello world";
	StringView view(szStr);

	EXPECT_EQ(view.Length(), 11);
	EXPECT_EQ(view.Source(), szStr.Source());
	EXPECT_TRUE(view == "hello world");
	EXPECT_TRUE(view == szStr);

	// 문자열 길이 변경 테스트
	String szStr2 = "a";
	StringView view2(szStr2);
	EXPECT_EQ(view2.Length(), 1);
	EXPECT_EQ(view2[0], 'a');
}

// StringView 엣지 케이스 테스트
TEST(StringViewTest, EdgeCases) {
	// Null StringView with operations
	StringView nullView;
	EXPECT_EQ(nullView.Find("test"), -1);
	EXPECT_EQ(nullView.Count("test"), 0);
	EXPECT_FALSE(nullView.Contain("test"));
	EXPECT_TRUE(nullView.IsNull());
	EXPECT_TRUE(nullView.IsEmpty());

	// Empty but non-null StringView
	const char* pEmptyStr = "";
	StringView emptyView(const_cast<char*>(pEmptyStr), 0);
	EXPECT_FALSE(emptyView.IsNull());
	EXPECT_TRUE(emptyView.IsEmpty());
	EXPECT_EQ(emptyView.First(), '\0');
	EXPECT_EQ(emptyView.Last(), '\0');

	// Single character
	const char* pSingleChar = "x";
	StringView singleView(const_cast<char*>(pSingleChar));
	EXPECT_EQ(singleView.Length(), 1);
	EXPECT_EQ(singleView.First(), 'x');
	EXPECT_EQ(singleView.Last(), 'x');
	EXPECT_TRUE(singleView == "x");
}

// StringView 범위 검사 테스트
TEST(StringViewTest, RangeOperations) {
	const char* pStr = "0123456789";
	StringView view(const_cast<char*>(pStr));

	// Count with range
	EXPECT_EQ(view.Count(0, 4, "1"), 1);
	EXPECT_EQ(view.Count(5, 9, "1"), 0);

	// Find with range
	EXPECT_EQ(view.Find(2, 8, "456"), 4);
	EXPECT_EQ(view.Find(0, 2, "456"), -1);

	// FindReverse with range
	EXPECT_EQ(view.FindReverse(0, 8, "7"), 7);
}

#endif // TEST_StringViewTest == ON
