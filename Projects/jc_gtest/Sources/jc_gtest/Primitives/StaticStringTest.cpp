/*
	작성자 : 윤정도
	StaticStringTest 입니다. (A/W 양쪽)
*/


#include <type_traits>

#include "jc/Primitives/String.h"
#include "jc/Primitives/StaticString.h"


using namespace std;

#if TEST_StaticStringTest == ON

// narrow 리터럴에서 CharT StaticString을 만든다 (집합체 초기화와 동일 효과, constexpr).
template <typename CharT, _u32 M>
constexpr StaticString<M, CharT> SS(const char(&lit)[M])
{
	StaticString<M, CharT> s{};
	for (_u32 i = 0; i < M - 1; ++i)
		s.Source[i] = static_cast<CharT>(lit[i]);
	s.Source[M - 1] = CharT(0);
	return s;
}

template <typename CharT>
class StaticStringTypedTest : public ::testing::Test {};

// googletest 1.8.1이라 TYPED_TEST_SUITE 대신 TYPED_TEST_CASE를 쓴다.
// 1.8.1 TYPED_TEST_CASE는 가변 인자라 콤마가 있으면 쪼개지므로 typedef로 묶어야 한다.
typedef ::testing::Types<char, wchar_t> StaticStringTestTypes;
TYPED_TEST_CASE(StaticStringTypedTest, StaticStringTestTypes);

TYPED_TEST(StaticStringTypedTest, Operator) {
	auto szStr1 = SS<TypeParam>("abcd");
	auto szStr2 = SS<TypeParam>("");
	auto szStr3 = SS<TypeParam>("a");

	// String 결과
	EXPECT_TRUE(szStr1 == SS<TypeParam>("abcd"));
	EXPECT_TRUE(szStr1 != SS<TypeParam>("abc"));
	EXPECT_TRUE(szStr2 == SS<TypeParam>(""));
	EXPECT_TRUE(szStr2 != SS<TypeParam>("a"));
	EXPECT_TRUE(szStr3 == SS<TypeParam>("a"));
	EXPECT_TRUE(szStr3 != SS<TypeParam>(""));
	EXPECT_TRUE(szStr1.Compare(szStr1) == 0);	// abcd와 abcd 자기 자신 비교
	EXPECT_TRUE(szStr1.Compare(szStr2) == 1);	// abcd와 "" 빈 문자열 비교는 abcd가 더 크다.
	EXPECT_TRUE(szStr1.Compare(szStr3) == 1);	// abcd와 a를 비교하면 abcd가 더 크다.

	StaticString<6, TypeParam> mut;
	mut = SS<TypeParam>("abcd");
	mut[0] = TypeParam('b');
	EXPECT_TRUE(mut[0] == TypeParam('b'));
	EXPECT_TRUE(mut[1] == TypeParam('b'));
	EXPECT_TRUE(mut[2] == TypeParam('c'));
	EXPECT_TRUE(mut[3] == TypeParam('d'));
	EXPECT_TRUE(mut[4] == TypeParam('\0'));

}

TYPED_TEST(StaticStringTypedTest, CopyFrom) {
	StaticString<7, TypeParam> szStr1;
	szStr1 = SS<TypeParam>("abcdef");

	auto e0 = SS<TypeParam>("");
	szStr1.CopyFrom(e0.Source);
	EXPECT_TRUE(szStr1 == SS<TypeParam>(""));

	auto e1 = SS<TypeParam>("k");
	szStr1.CopyFrom(e1.Source);
	EXPECT_TRUE(szStr1 == SS<TypeParam>("k"));

	auto e2 = SS<TypeParam>("bb");
	szStr1.CopyFrom(e2.Source);
	EXPECT_TRUE(szStr1 == SS<TypeParam>("bb"));

	auto e3 = SS<TypeParam>("ccc");
	szStr1.CopyFrom(e3.Source);
	EXPECT_TRUE(szStr1 == SS<TypeParam>("ccc"));

	auto e4 = SS<TypeParam>("dddd");
	szStr1.CopyFrom(e4.Source);
	EXPECT_TRUE(szStr1 == SS<TypeParam>("dddd"));

	auto e5 = SS<TypeParam>("eeeee");
	szStr1.CopyFrom(e5.Source);
	EXPECT_TRUE(szStr1 == SS<TypeParam>("eeeee"));

	auto e6 = SS<TypeParam>("ffffff");
	szStr1.CopyFrom(e6.Source);
	EXPECT_TRUE(szStr1 == SS<TypeParam>("ffffff"));

	auto e7 = SS<TypeParam>("qqqqqqq");
	szStr1.CopyFrom(e7.Source);
	EXPECT_TRUE(szStr1 == SS<TypeParam>("qqqqqq"));	// 크기가 7이기 때문에 6개만 복사됨

	auto e8 = SS<TypeParam>("rrrrrrrr");
	szStr1.CopyFrom(e8.Source);
	EXPECT_TRUE(szStr1 == SS<TypeParam>("rrrrrr"));	// 크기가 7이기 때문에 6개만 복사됨
}

TYPED_TEST(StaticStringTypedTest, StartWith) {
	auto szStr1 = SS<TypeParam>("abcdef");

	EXPECT_TRUE(szStr1.StartWith(SS<TypeParam>("").Source));
	EXPECT_TRUE(szStr1.StartWith(SS<TypeParam>("a").Source));
	EXPECT_TRUE(szStr1.StartWith(SS<TypeParam>("ab").Source));
	EXPECT_TRUE(szStr1.StartWith(SS<TypeParam>("abc").Source));
	EXPECT_TRUE(szStr1.StartWith(SS<TypeParam>("abcd").Source));
	EXPECT_TRUE(szStr1.StartWith(SS<TypeParam>("abcde").Source));
	EXPECT_TRUE(szStr1.StartWith(SS<TypeParam>("abcdef").Source));

	// 못찾는 경우
	EXPECT_FALSE(szStr1.StartWith(SS<TypeParam>("c").Source));
	EXPECT_FALSE(szStr1.StartWith(SS<TypeParam>("cab").Source));
	EXPECT_FALSE(szStr1.StartWith(SS<TypeParam>("abcdefg").Source));
}

TYPED_TEST(StaticStringTypedTest, Contain) {
	auto szStr1 = SS<TypeParam>("abcd");
	EXPECT_TRUE(szStr1.Contain(SS<TypeParam>("a").Source));
	EXPECT_TRUE(szStr1.Contain(SS<TypeParam>("b").Source));
	EXPECT_TRUE(szStr1.Contain(SS<TypeParam>("c").Source));
	EXPECT_TRUE(szStr1.Contain(SS<TypeParam>("d").Source));
	EXPECT_TRUE(szStr1.Contain(SS<TypeParam>("").Source));

	EXPECT_TRUE(szStr1.Contain(SS<TypeParam>("ab").Source));
	EXPECT_TRUE(szStr1.Contain(SS<TypeParam>("bc").Source));
	EXPECT_TRUE(szStr1.Contain(SS<TypeParam>("cd").Source));

	EXPECT_TRUE(szStr1.Contain(SS<TypeParam>("abc").Source));
	EXPECT_TRUE(szStr1.Contain(SS<TypeParam>("bcd").Source));

	EXPECT_TRUE(szStr1.Contain(SS<TypeParam>("abcd").Source));

	// 못찾는 경우
	EXPECT_FALSE(szStr1.Contain(SS<TypeParam>(" abcd").Source));
	EXPECT_FALSE(szStr1.Contain(SS<TypeParam>("abcd ").Source));
	EXPECT_FALSE(szStr1.Contain(SS<TypeParam>("ab ").Source));
	EXPECT_FALSE(szStr1.Contain(SS<TypeParam>(" ab").Source));
}

TYPED_TEST(StaticStringTypedTest, EndWith) {
	auto szStr1 = SS<TypeParam>("abcdef");

	EXPECT_TRUE(szStr1.EndWith(SS<TypeParam>("").Source));
	EXPECT_TRUE(szStr1.EndWith(SS<TypeParam>("f").Source));
	EXPECT_TRUE(szStr1.EndWith(SS<TypeParam>("ef").Source));
	EXPECT_TRUE(szStr1.EndWith(SS<TypeParam>("def").Source));
	EXPECT_TRUE(szStr1.EndWith(SS<TypeParam>("cdef").Source));
	EXPECT_TRUE(szStr1.EndWith(SS<TypeParam>("bcdef").Source));
	EXPECT_TRUE(szStr1.EndWith(SS<TypeParam>("abcdef").Source));

	// 못찾는 경우
	EXPECT_FALSE(szStr1.EndWith(SS<TypeParam>("g").Source));
	EXPECT_FALSE(szStr1.EndWith(SS<TypeParam>("eef").Source));
	EXPECT_FALSE(szStr1.EndWith(SS<TypeParam>("0abcdef").Source));
}

TYPED_TEST(StaticStringTypedTest, Remove) {
	auto szStr1 = SS<TypeParam>("abcd");

	EXPECT_TRUE(szStr1.Remove(SS<TypeParam>("").Source) == SS<TypeParam>("abcd"));

	EXPECT_TRUE(szStr1.Remove(SS<TypeParam>("a").Source) == SS<TypeParam>("bcd"));
	EXPECT_TRUE(szStr1.Remove(SS<TypeParam>("b").Source) == SS<TypeParam>("acd"));
	EXPECT_TRUE(szStr1.Remove(SS<TypeParam>("c").Source) == SS<TypeParam>("abd"));
	EXPECT_TRUE(szStr1.Remove(SS<TypeParam>("d").Source) == SS<TypeParam>("abc"));

	EXPECT_TRUE(szStr1.Remove(SS<TypeParam>("ab").Source) == SS<TypeParam>("cd"));
	EXPECT_TRUE(szStr1.Remove(SS<TypeParam>("bc").Source) == SS<TypeParam>("ad"));
	EXPECT_TRUE(szStr1.Remove(SS<TypeParam>("cd").Source) == SS<TypeParam>("ab"));

	EXPECT_TRUE(szStr1.Remove(SS<TypeParam>("abc").Source) == SS<TypeParam>("d"));
	EXPECT_TRUE(szStr1.Remove(SS<TypeParam>("bcd").Source) == SS<TypeParam>("a"));

	EXPECT_TRUE(szStr1.Remove(SS<TypeParam>("abcd").Source) == SS<TypeParam>(""));
}

#endif
