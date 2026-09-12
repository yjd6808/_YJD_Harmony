/*
	작성자 : 윤정도
	String 클래스 테스트입니다. (A/W 양쪽)
*/


#include <type_traits>

#include "jc/Primitives/String.h"
#include "jc/Primitives/StringConvert.h"
#include "jc/Container/Vector.h"


using namespace std;

static Vector<int> gec;

#if TEST_StringTest == ON

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
class StringTypedTest : public ::testing::Test {};

// googletest 1.8.1이라 TYPED_TEST_SUITE 대신 TYPED_TEST_CASE를 쓴다.
// 1.8.1 TYPED_TEST_CASE는 가변 인자라 콤마가 있으면 쪼개지므로 typedef로 묶어야 한다.
typedef ::testing::Types<char, wchar_t> StringTestTypes;
TYPED_TEST_CASE(StringTypedTest, StringTestTypes);

// String::Compare 함수 테스트
TYPED_TEST(StringTypedTest, Compare) {
	TStr<TypeParam> szStr1 = C<TypeParam>("abcd");
	TStr<TypeParam> szStr2 = TStr<TypeParam>::Empty;
	TStr<TypeParam> szStr3 = C<TypeParam>("a");

	string szStr4 = "abcd";
	string szStr5 = "";
	string szStr6 = "a";

	// String 결과
	EXPECT_TRUE(szStr1 == C<TypeParam>("abcd"));
	EXPECT_TRUE(szStr1 != C<TypeParam>("abc"));
	EXPECT_TRUE(szStr2 == TStr<TypeParam>::Empty);
	EXPECT_TRUE(szStr2 != C<TypeParam>("a"));
	EXPECT_TRUE(szStr3 == C<TypeParam>("a"));
	EXPECT_TRUE(szStr3 != TStr<TypeParam>::Empty);
	EXPECT_TRUE(szStr1.Compare(szStr1) == 0);	// abcd와 abcd 자기 자신 비교
	EXPECT_TRUE(szStr1.Compare(szStr2) == 1);	// abcd와 "" 빈 문자열 비교는 abcd가 더 크다.
	EXPECT_TRUE(szStr1.Compare(szStr3) == 1);	// abcd와 a를 비교하면 abcd가 더 크다.

	// std::string 결과 (표준 동작 대조용, CharT와 무관)
	EXPECT_TRUE(szStr4 == "abcd");
	EXPECT_TRUE(szStr4 != "abc");
	EXPECT_TRUE(szStr5.empty());
	EXPECT_TRUE(szStr5 != "a");
	EXPECT_TRUE(szStr6 == "a");
	EXPECT_TRUE(!szStr6.empty());
	EXPECT_TRUE(szStr4.compare(szStr4) == 0);	// abcd와 abcd 자기 자신 비교
	EXPECT_TRUE(szStr4.compare(szStr5) == 1);	// abcd와 "" 빈 문자열 비교는 abcd가 더 크다.
	EXPECT_TRUE(szStr4.compare(szStr6) == 1);	// abcd와 a를 비교하면 abcd가 더 크다.
}

// String::operator> < >= <= 함수 테스트
TYPED_TEST(StringTypedTest, ComparisonOperator)
{
	const TStr<TypeParam> str1 = C<TypeParam>("abcd");
	const TStr<TypeParam> str2 = C<TypeParam>("abc");
	const TStr<TypeParam> str3 = C<TypeParam>("ab");

	EXPECT_FALSE(str2 > str2);
	EXPECT_TRUE(str2 >= str2);
	EXPECT_TRUE(str1 >= str2);
	EXPECT_TRUE(str1 > str2);
	EXPECT_TRUE(str3 < str2);
	EXPECT_TRUE(str3 <= str2);
}


// String::Split 함수 테스트
TYPED_TEST(StringTypedTest, Split) {
	TStr<TypeParam> szStr1 = C<TypeParam>("abcd_cd_efg");
	TStr<TypeParam> szStr2 = C<TypeParam>("___");
	TStr<TypeParam> szStr3 = C<TypeParam>("_bbbbb_");
	TStr<TypeParam> szStr4 = C<TypeParam>("_bbbbb");
	TStr<TypeParam> szStr5 = C<TypeParam>("_");

	{
		LeakCheck;
		Vector<TStr<TypeParam>> tokens1_1 = szStr1.Split(C<TypeParam>("_").Source(), true);
		EXPECT_TRUE(tokens1_1.Size() == 3);
		if (tokens1_1.Size() == 3) {
			EXPECT_TRUE(tokens1_1[0] == C<TypeParam>("abcd"));
			EXPECT_TRUE(tokens1_1[1] == C<TypeParam>("cd"));
			EXPECT_TRUE(tokens1_1[2] == C<TypeParam>("efg"));
		}
	}

	{
		LeakCheck;
		Vector<TStr<TypeParam>> tokens1_2 = szStr1.Split(C<TypeParam>("_").Source(), false);
		EXPECT_TRUE(tokens1_2.Size() == 3);
		if (tokens1_2.Size() == 3) {
			EXPECT_TRUE(tokens1_2[0] == C<TypeParam>("abcd"));
			EXPECT_TRUE(tokens1_2[1] == C<TypeParam>("cd"));
			EXPECT_TRUE(tokens1_2[2] == C<TypeParam>("efg"));
		}
	}


	{
		LeakCheck;
		Vector<TStr<TypeParam>> tokens2_1 = szStr2.Split(C<TypeParam>("_").Source(), true);
		EXPECT_TRUE(tokens2_1.Size() == 4);
		if (tokens2_1.Size() == 4) {
			EXPECT_TRUE(tokens2_1[0] == C<TypeParam>(""));
			EXPECT_TRUE(tokens2_1[1] == C<TypeParam>(""));
			EXPECT_TRUE(tokens2_1[2] == C<TypeParam>(""));
			EXPECT_TRUE(tokens2_1[3] == C<TypeParam>(""));
		}
	}


	{
		LeakCheck;
		Vector<TStr<TypeParam>> tokens3_1 = szStr3.Split(C<TypeParam>("_").Source(), true);
		EXPECT_TRUE(tokens3_1.Size() == 3);
		if (tokens3_1.Size() == 3) {
			EXPECT_TRUE(tokens3_1[0] == C<TypeParam>(""));
			EXPECT_TRUE(tokens3_1[1] == C<TypeParam>("bbbbb"));
			EXPECT_TRUE(tokens3_1[2] == C<TypeParam>(""));
		}
	}

	{
		LeakCheck;
		Vector<TStr<TypeParam>> tokens3_2 = szStr3.Split(C<TypeParam>("_").Source(), false);
	}

	{
		LeakCheck;
		Vector<TStr<TypeParam>> tokens4_1 = szStr4.Split(C<TypeParam>("_").Source(), true);

		EXPECT_TRUE(tokens4_1.Size() == 2);
		if (tokens4_1.Size() == 2) {
			EXPECT_TRUE(tokens4_1[0] == C<TypeParam>(""));
			EXPECT_TRUE(tokens4_1[1] == C<TypeParam>("bbbbb"));
		}
	}

	{
		LeakCheck;
		Vector<TStr<TypeParam>> tokens5_1 = szStr5.Split(C<TypeParam>("_").Source(), true);
		EXPECT_TRUE(tokens5_1.Size() == 2);
		if (tokens5_1.Size() == 2) {
			EXPECT_TRUE(tokens5_1[0] == C<TypeParam>(""));
			EXPECT_TRUE(tokens5_1[1] == C<TypeParam>(""));
		}

	}

	{
		LeakCheck;
		Vector<TStr<TypeParam>> tokens5_2 = szStr5.Split(C<TypeParam>("_").Source(), false);
		EXPECT_TRUE(tokens5_2.Size() == 0);
	}
}


// String::ReplaceAll 함수 테스트
TYPED_TEST(StringTypedTest, ReplaceAll) {

	// ReplaceAllDifferentLen 테스트
	TStr<TypeParam> szStr1 = C<TypeParam>("abcd_cd_efg");
	TStr<TypeParam> szStr2 = C<TypeParam>("___");
	TStr<TypeParam> szStr3 = C<TypeParam>("_bbbbb_");
	TStr<TypeParam> szStr4 = C<TypeParam>("_");
	TStr<TypeParam> szStr5 = C<TypeParam>("");

	szStr1.ReplaceAll(C<TypeParam>("_").Source(), C<TypeParam>("@@").Source());
	szStr2.ReplaceAll(C<TypeParam>("_").Source(), C<TypeParam>("@@").Source());
	szStr3.ReplaceAll(C<TypeParam>("_").Source(), C<TypeParam>("@@").Source());
	szStr4.ReplaceAll(C<TypeParam>("_").Source(), C<TypeParam>("@@").Source());
	szStr5.ReplaceAll(C<TypeParam>("").Source(), C<TypeParam>("abcdef").Source());

	EXPECT_TRUE(szStr1 == C<TypeParam>("abcd@@cd@@efg"));
	EXPECT_TRUE(szStr2 == C<TypeParam>("@@@@@@"));
	EXPECT_TRUE(szStr3 == C<TypeParam>("@@bbbbb@@"));
	EXPECT_TRUE(szStr4 == C<TypeParam>("@@"));
	EXPECT_TRUE(szStr5 == C<TypeParam>("abcdef"));


	// ReplaceAllEqualLen 테스트
	TStr<TypeParam> szStr6 = C<TypeParam>("한글__사랑__한글__동물");
	TStr<TypeParam> szStr7 = C<TypeParam>("a_a_b_c");
	TStr<TypeParam> szStr8 = C<TypeParam>("aa_aaa");

	szStr6.ReplaceAll(C<TypeParam>("한글").Source(), C<TypeParam>("국가").Source());
	szStr7.ReplaceAll(C<TypeParam>("a").Source(), C<TypeParam>("z").Source());
	szStr8.ReplaceAll(C<TypeParam>("aa").Source(), C<TypeParam>("kk").Source());


	EXPECT_TRUE(szStr6 == C<TypeParam>("국가__사랑__국가__동물"));
	EXPECT_TRUE(szStr7 == C<TypeParam>("z_z_b_c"));
	EXPECT_TRUE(szStr8 == C<TypeParam>("kk_kka"));
}


// String::Format 함수 테스트
TYPED_TEST(StringTypedTest, Format) {
	TStr<TypeParam> szStr1;
	szStr1.Format(C<TypeParam>("%d").Source(), 5123);
	EXPECT_TRUE(szStr1 == C<TypeParam>("5123"));

	szStr1.Format(C<TypeParam>("%d %d %d").Source(), 2000000000, 2000000000, 2000000000);
	EXPECT_TRUE(szStr1 == C<TypeParam>("2000000000 2000000000 2000000000"));

	szStr1.Format(C<TypeParam>("%.2f").Source(), 1.52f);
	EXPECT_TRUE(szStr1 == C<TypeParam>("1.52"));
}


// String::operator 연산자 테스트
TYPED_TEST(StringTypedTest, AssignmentOperator) {
	TStr<TypeParam> szStr1 = C<TypeParam>("abcd");
	TStr<TypeParam>&& szStr2 = C<TypeParam>("ccc");

	// operator=(const CharT*) 호출
	szStr1 = C<TypeParam>("fredsgersgergesrgersfesvdfscdfscdssdfgds").Source();
	EXPECT_TRUE(szStr1 == C<TypeParam>("fredsgersgergesrgersfesvdfscdfscdssdfgds"));

	// operator=(const String&) 호출
	szStr1 = szStr2;
	EXPECT_TRUE(szStr1 == C<TypeParam>("ccc"));

	// operator=(String&&) 호출
	szStr1 = std::move(szStr2);
	EXPECT_TRUE(szStr1 == C<TypeParam>("ccc"));
	EXPECT_TRUE(szStr2.Length() == 0);
}


// String::Find 테스트
TYPED_TEST(StringTypedTest, Find) {
	TStr<TypeParam> szSource = C<TypeParam>("abcdefg"); // 0 ~ 6

	EXPECT_TRUE(szSource.Find(C<TypeParam>("g").Source()) == 6);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("fg").Source()) == 5);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("efg").Source()) == 4);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("defg").Source()) == 3);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("cdefg").Source()) == 2);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("bcdefg").Source()) == 1);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("abcdefg").Source()) == 0);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("-abcdefg").Source()) == -1);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("abcdefg-").Source()) == -1);

	// 왼쪽에서 검사 테스트
	EXPECT_TRUE(szSource.Find(C<TypeParam>("a").Source()) == 0);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("ab").Source()) == 0);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("abc").Source()) == 0);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("abcd").Source()) == 0);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("abcde").Source()) == 0);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("abcdef").Source()) == 0);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("abcdefg").Source()) == 0);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("abcdefg-").Source()) == -1);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("-abcdefg").Source()) == -1);

	// 범위 검사 테스트
	EXPECT_TRUE(szSource.Find(0, 0, C<TypeParam>("a").Source()) == 0);
	EXPECT_TRUE(szSource.Find(0, 1, C<TypeParam>("a").Source()) == 0);
	EXPECT_TRUE(szSource.Find(1, 1, C<TypeParam>("a").Source()) == -1);

	EXPECT_TRUE(szSource.Find(0, 2, C<TypeParam>("abc").Source()) == 0);
	EXPECT_TRUE(szSource.Find(1, 2, C<TypeParam>("abc").Source()) == -1);
	EXPECT_TRUE(szSource.Find(2, 2, C<TypeParam>("abc").Source()) == -1);

	EXPECT_TRUE(szSource.Find(4, 4, C<TypeParam>("e").Source()) == 4);
	EXPECT_TRUE(szSource.Find(4, 5, C<TypeParam>("ef").Source()) == 4);
	EXPECT_TRUE(szSource.Find(4, 6, C<TypeParam>("efg").Source()) == 4);

	EXPECT_TRUE(szSource.Find(0, 6, C<TypeParam>("abcdefg").Source()) == 0);
	EXPECT_TRUE(szSource.Find(1, 6, C<TypeParam>("abcdefg").Source()) == -1);
	EXPECT_TRUE(szSource.Find(0, 5, C<TypeParam>("abcdefg").Source()) == -1);
}



// String::FindReverse 테스트
TYPED_TEST(StringTypedTest, FindReverse) {
	TStr<TypeParam> szSource = C<TypeParam>("abcdefg"); // 0 ~ 6

	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("g").Source()) == 6);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("fg").Source()) == 5);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("efg").Source()) == 4);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("defg").Source()) == 3);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("cdefg").Source()) == 2);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("bcdefg").Source()) == 1);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("abcdefg").Source()) == 0);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("-abcdefg").Source()) == -1);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("abcdefg-").Source()) == -1);

	// 왼쪽에서 검사 테스트
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("a").Source()) == 0);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("ab").Source()) == 0);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("abc").Source()) == 0);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("abcd").Source()) == 0);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("abcde").Source()) == 0);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("abcdef").Source()) == 0);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("abcdefg").Source()) == 0);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("abcdefg-").Source()) == -1);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("-abcdefg").Source()) == -1);

	// 범위 검사 테스트
	EXPECT_TRUE(szSource.FindReverse(0, 0, C<TypeParam>("a").Source()) == 0);
	EXPECT_TRUE(szSource.FindReverse(0, 1, C<TypeParam>("a").Source()) == 0);
	EXPECT_TRUE(szSource.FindReverse(1, 1, C<TypeParam>("a").Source()) == -1);

	EXPECT_TRUE(szSource.FindReverse(0, 2, C<TypeParam>("abc").Source()) == 0);
	EXPECT_TRUE(szSource.FindReverse(1, 2, C<TypeParam>("abc").Source()) == -1);
	EXPECT_TRUE(szSource.FindReverse(2, 2, C<TypeParam>("abc").Source()) == -1);

	EXPECT_TRUE(szSource.FindReverse(4, 4, C<TypeParam>("e").Source()) == 4);
	EXPECT_TRUE(szSource.FindReverse(4, 5, C<TypeParam>("ef").Source()) == 4);
	EXPECT_TRUE(szSource.FindReverse(4, 6, C<TypeParam>("efg").Source()) == 4);

	EXPECT_TRUE(szSource.FindReverse(0, 6, C<TypeParam>("abcdefg").Source()) == 0);
	EXPECT_TRUE(szSource.FindReverse(1, 6, C<TypeParam>("abcdefg").Source()) == -1);
	EXPECT_TRUE(szSource.FindReverse(0, 5, C<TypeParam>("abcdefg").Source()) == -1);

}

TYPED_TEST(StringTypedTest, Count) {
	TStr<TypeParam> szSource = C<TypeParam>(" ");
	EXPECT_TRUE(szSource.Count(C<TypeParam>(" ").Source()) == 1);
	EXPECT_TRUE(szSource.Count(C<TypeParam>("a").Source()) == 0);

	// 은근슬쩍 operator=T 테스트
	szSource = 123454321;
	EXPECT_TRUE(szSource.Count(C<TypeParam>("1").Source()) == 2);
	EXPECT_TRUE(szSource.Count(C<TypeParam>("2").Source()) == 2);
	EXPECT_TRUE(szSource.Count(C<TypeParam>("3").Source()) == 2);
	EXPECT_TRUE(szSource.Count(C<TypeParam>("4").Source()) == 2);
	EXPECT_TRUE(szSource.Count(C<TypeParam>("5").Source()) == 1);

	szSource = C<TypeParam>("aabbcc");
	EXPECT_TRUE(szSource.Count(C<TypeParam>("a").Source()) == 2);
	EXPECT_TRUE(szSource.Count(C<TypeParam>("b").Source()) == 2);
	EXPECT_TRUE(szSource.Count(C<TypeParam>("c").Source()) == 2);

	szSource = C<TypeParam>("a");
	EXPECT_TRUE(szSource.Count(C<TypeParam>("a").Source()) == 1);
}

TYPED_TEST(StringTypedTest, Insert) {
	TStr<TypeParam> szSource(C<TypeParam>("").Source(), 1);
	szSource.Insert(0, C<TypeParam>("a").Source());
	EXPECT_TRUE(szSource == C<TypeParam>("a"));

	szSource.Insert(0, C<TypeParam>("b").Source());
	EXPECT_TRUE(szSource == C<TypeParam>("ba"));

	szSource.Insert(0, C<TypeParam>("c").Source());
	EXPECT_TRUE(szSource == C<TypeParam>("cba"));

	szSource.Insert(3, C<TypeParam>("b").Source());
	EXPECT_TRUE(szSource == C<TypeParam>("cbab"));
	szSource.Insert(4, C<TypeParam>("c").Source());
	EXPECT_TRUE(szSource == C<TypeParam>("cbabc"));

	szSource.Insert(1, C<TypeParam>("zz").Source());
	EXPECT_TRUE(szSource == C<TypeParam>("czzbabc"));
}


TYPED_TEST(StringTypedTest, Replace) {
	LeakCheck;
	TStr<TypeParam> szSource = C<TypeParam>("a");
	// 빈 문자열로 바꾸는 경우
	int ret = szSource.Replace(C<TypeParam>("a").Source(), C<TypeParam>(""));
	EXPECT_TRUE(szSource == C<TypeParam>(""));
	EXPECT_TRUE(ret == -1);	// 마지막 위치에 도달해서 -1을 반환


	// 맨 앞에서 긴 문자열로 바꾸는 경우
	szSource = TStr<TypeParam>(C<TypeParam>("aabcdd").Source(), 7);
	ret = szSource.Replace(C<TypeParam>("aa").Source(), C<TypeParam>("zzzz"));
	EXPECT_TRUE(szSource == C<TypeParam>("zzzzbcdd"));
	EXPECT_TRUE(ret == 4);

	// 맨뒤에서 긴 문자열로 바꾸는 경우
	ret = szSource.Replace(C<TypeParam>("dd").Source(), C<TypeParam>("zzzz"));
	EXPECT_TRUE(szSource == C<TypeParam>("zzzzbczzzz"));
	EXPECT_TRUE(ret == -1);	// 마지막 위치에 도달해서 -1 반환

	// 중간에서 긴 문자열로 바꾸는 경우
	ret = szSource.Replace(C<TypeParam>("bc").Source(), C<TypeParam>("zzzz"));
	EXPECT_TRUE(szSource == C<TypeParam>("zzzzzzzzzzzz"));
	EXPECT_TRUE(ret == 8);

	// 확장 잘 되는지 확인
	szSource = TStr<TypeParam>(C<TypeParam>("a").Source(), 2);
	szSource.Replace(C<TypeParam>("a").Source(), C<TypeParam>("zzzzzz"));
	EXPECT_TRUE(szSource == C<TypeParam>("zzzzzz"));

	// 맨 앞에서 같은 길이로 치환
	szSource = C<TypeParam>("aabbcc");
	szSource.Replace(C<TypeParam>("aa").Source(), C<TypeParam>("gg"));
	EXPECT_TRUE(szSource == C<TypeParam>("ggbbcc"));

	// 중간에서 같은 길이로 치환
	szSource.Replace(C<TypeParam>("bb").Source(), C<TypeParam>("gg"));
	EXPECT_TRUE(szSource == C<TypeParam>("ggggcc"));

	// 뒤에서 같은 길이로 치환
	szSource.Replace(C<TypeParam>("cc").Source(), C<TypeParam>("gg"));
	EXPECT_TRUE(szSource == C<TypeParam>("gggggg"));

	// 맨 앞에서 짧은 길이로 치환
	szSource = C<TypeParam>("aabbcc");
	szSource.Replace(C<TypeParam>("aa").Source(), C<TypeParam>("a"));
	EXPECT_TRUE(szSource == C<TypeParam>("abbcc"));

	// 중간에서 짧은 길이로 치환
	szSource.Replace(C<TypeParam>("bb").Source(), C<TypeParam>("b"));
	EXPECT_TRUE(szSource == C<TypeParam>("abcc"));

	// 맨뒤에서 짧은 길이로 치환
	szSource.Replace(C<TypeParam>("cc").Source(), C<TypeParam>("c"));
	EXPECT_TRUE(szSource == C<TypeParam>("abc"));


	// 반복 Replace 테스트
	int iReplaceOffset = 0;
	int iReplaceCount = 0;
	szSource = C<TypeParam>("aaaaaaaa");

	while ((iReplaceOffset = szSource.Replace(iReplaceOffset, C<TypeParam>("a").Source(), C<TypeParam>("b"))) != -1) {
		iReplaceCount++;
	}
	EXPECT_TRUE(szSource == C<TypeParam>("bbbbbbbb"));
	EXPECT_TRUE(iReplaceCount == 7);	// 한번은 카운트 안되서 7로 뜸


	szSource = C<TypeParam>("aaaaaaaa");
	iReplaceOffset = 0;
	iReplaceCount = 0;

	// 길게 반복 Replace
	while ((iReplaceOffset = szSource.Replace(iReplaceOffset, C<TypeParam>("a").Source(), C<TypeParam>("cc"))) != -1) {
		iReplaceCount++;
	}

	EXPECT_TRUE(szSource == C<TypeParam>("cccccccccccccccc"));
	EXPECT_TRUE(iReplaceCount == 7);	// 한번은 카운트 안되서 7로 뜸

	iReplaceOffset = 0;
	iReplaceCount = 0;

	// 짧게 반복 Replace
	while ((iReplaceOffset = szSource.Replace(iReplaceOffset, C<TypeParam>("cc").Source(), C<TypeParam>("a"))) != -1) {
		iReplaceCount++;
	}

	EXPECT_TRUE(szSource == C<TypeParam>("aaaaaaaa"));
	EXPECT_TRUE(iReplaceCount == 7);	// 한번은 카운트 안되서 7로 뜸
}

TYPED_TEST(StringTypedTest, Clear) {
	TStr<TypeParam> szSource = C<TypeParam>("abcd");
	szSource.Clear();
	EXPECT_TRUE(szSource == C<TypeParam>(""));

	szSource = C<TypeParam>("abcd");
	szSource.Clear(1, 2);
	EXPECT_TRUE(szSource == C<TypeParam>("ad"));

	szSource = C<TypeParam>("abcd");
	szSource.Clear(1, 3);
	EXPECT_TRUE(szSource == C<TypeParam>("a"));

	szSource = C<TypeParam>("abcd");
	szSource.Clear(1, 4);
	EXPECT_TRUE(szSource == C<TypeParam>("a"));
}

TYPED_TEST(StringTypedTest, FindCaseSensitive)
{
	TStr<TypeParam> szSource = C<TypeParam>("Hello World ABCDEFG");

	// caseSensitive = true (기본값)
	EXPECT_TRUE(szSource.Find(C<TypeParam>("Hello").Source()) == 0);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("hello").Source()) == -1);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("WORLD").Source()) == -1);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("ABCDEFG").Source()) == 12);

	// caseSensitive = false
	EXPECT_TRUE(szSource.Find(C<TypeParam>("hello").Source(), false) == 0);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("HELLO").Source(), false) == 0);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("world").Source(), false) == 6);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("WORLD").Source(), false) == 6);
	EXPECT_TRUE(szSource.Find(C<TypeParam>("abcdefg").Source(), false) == 12);

	// 범위 지정 + caseSensitive = false
	EXPECT_TRUE(szSource.Find(0, 4, C<TypeParam>("hello").Source(), false) == 0);
	EXPECT_TRUE(szSource.Find(1, 4, C<TypeParam>("hello").Source(), false) == -1);
	EXPECT_TRUE(szSource.Find(6, 10, C<TypeParam>("world").Source(), false) == 6);
}

TYPED_TEST(StringTypedTest, FindReverseCaseSensitive)
{
	TStr<TypeParam> szSource = C<TypeParam>("AbcABCAbc");

	// caseSensitive = true (기본값)
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("Abc").Source()) == 6);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("abc").Source()) == -1);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("ABC").Source()) == 3);

	// caseSensitive = false
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("abc").Source(), false) == 6);
	EXPECT_TRUE(szSource.FindReverse(C<TypeParam>("ABC").Source(), false) == 6);

	// 범위 지정 + caseSensitive = false
	EXPECT_TRUE(szSource.FindReverse(0, 5, C<TypeParam>("abc").Source(), false) == 3);
	EXPECT_TRUE(szSource.FindReverse(0, 2, C<TypeParam>("abc").Source(), false) == 0);
}

TYPED_TEST(StringTypedTest, FindAllCaseSensitive) {
	TStr<TypeParam> szSource = C<TypeParam>("AbcABCabc");

	// caseSensitive = true (기본값)
	Vector<int> result1 = szSource.FindAll(C<TypeParam>("abc").Source());
	EXPECT_TRUE(result1.Size() == 1);
	EXPECT_TRUE(result1[0] == 6);

	// caseSensitive = false
	Vector<int> result2 = szSource.FindAll(C<TypeParam>("abc").Source(), false);
	EXPECT_TRUE(result2.Size() == 3);
	if (result2.Size() == 3) {
		EXPECT_TRUE(result2[0] == 0);
		EXPECT_TRUE(result2[1] == 3);
		EXPECT_TRUE(result2[2] == 6);
	}
}

TYPED_TEST(StringTypedTest, CountCaseSensitive) {
	TStr<TypeParam> szSource = C<TypeParam>("HellohelloHELLO");

	// caseSensitive = true (기본값)
	EXPECT_TRUE(szSource.Count(C<TypeParam>("hello").Source()) == 1);
	EXPECT_TRUE(szSource.Count(C<TypeParam>("Hello").Source()) == 1);
	EXPECT_TRUE(szSource.Count(C<TypeParam>("HELLO").Source()) == 1);

	// caseSensitive = false
	EXPECT_TRUE(szSource.Count(C<TypeParam>("hello").Source(), false) == 3);
	EXPECT_TRUE(szSource.Count(C<TypeParam>("HELLO").Source(), false) == 3);

	// 범위 지정 + caseSensitive = false
	EXPECT_TRUE(szSource.Count(0, 9, C<TypeParam>("hello").Source(), false) == 2);
	EXPECT_TRUE(szSource.Count(5, 14, C<TypeParam>("hello").Source(), false) == 2);
}

TYPED_TEST(StringTypedTest, ReplaceCaseSensitive) {
	// caseSensitive = true (기본값)
	TStr<TypeParam> szSource = C<TypeParam>("HellohelloHELLO");
	szSource.Replace(C<TypeParam>("hello").Source(), C<TypeParam>("world"));
	EXPECT_TRUE(szSource == C<TypeParam>("HelloworldHELLO"));

	// caseSensitive = false
	szSource = C<TypeParam>("HellohelloHELLO");
	szSource.Replace(C<TypeParam>("hello").Source(), C<TypeParam>("world"), false);
	EXPECT_TRUE(szSource == C<TypeParam>("worldhelloHELLO"));
}

TYPED_TEST(StringTypedTest, ReplaceAllCaseSensitive) {
	// caseSensitive = true (기본값)
	TStr<TypeParam> szSource = C<TypeParam>("HellohelloHELLO");
	szSource.ReplaceAll(C<TypeParam>("hello").Source(), C<TypeParam>("world").Source());
	EXPECT_TRUE(szSource == C<TypeParam>("HelloworldHELLO"));

	// caseSensitive = false
	szSource = C<TypeParam>("HellohelloHELLO");
	szSource.ReplaceAll(C<TypeParam>("hello").Source(), C<TypeParam>("world").Source(), false);
	EXPECT_TRUE(szSource == C<TypeParam>("worldworldworld"));
}

TYPED_TEST(StringTypedTest, StartWithEndWithCaseSensitive) {
	TStr<TypeParam> szSource = C<TypeParam>("HelloWorld");

	// caseSensitive = true (기본값)
	EXPECT_TRUE(szSource.StartWith(C<TypeParam>("Hello")));
	EXPECT_FALSE(szSource.StartWith(C<TypeParam>("hello")));
	EXPECT_TRUE(szSource.EndWith(C<TypeParam>("World")));
	EXPECT_FALSE(szSource.EndWith(C<TypeParam>("world")));

	// caseSensitive = false
	EXPECT_TRUE(szSource.StartWith(C<TypeParam>("hello"), false));
	EXPECT_TRUE(szSource.StartWith(C<TypeParam>("HELLO"), false));
	EXPECT_TRUE(szSource.EndWith(C<TypeParam>("world"), false));
	EXPECT_TRUE(szSource.EndWith(C<TypeParam>("WORLD"), false));
}

#endif // TEST_StringTest == ON

