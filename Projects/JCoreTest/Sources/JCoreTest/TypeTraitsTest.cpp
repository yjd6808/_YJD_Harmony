/*
	작성자 : 윤정도
	PCH 겸.. JCore 테스트입니다.
*/

#include <JCoreTest/CoreTest.h>

#include <JCore/TypeCast.h>
#include <JCore/TypeTraits.h>
#include <JCore/Primitives/String.h>
#include <JCore/Allocator/DefaultArrayAllocator.h>
#include <JCore/Core.h>


using namespace std;

#if TEST_TypeTraitsTest == ON

TEST(TypeTraitsTest, IsSameType) {
	EXPECT_TRUE((IsSameType_v<int, int>));	// 일치
	EXPECT_TRUE((IsSameType_v<int, int*>) == false);
	EXPECT_TRUE((IsSameType_v<int*, int>) == false);
	EXPECT_TRUE((IsSameType_v<int, char>) == false);
	EXPECT_TRUE((IsSameType_v<char*, char*>));	// 일치
	EXPECT_TRUE((IsSameType_v<char*, char**>) == false);
	EXPECT_TRUE((IsSameType_v<const char&, const char>) == false);
}

TEST(TypeTraitsTest, IsStringType) {
	char a[30] {};
	const wchar_t b[40] {};
	EXPECT_TRUE((IsStringType_v<String>));
	EXPECT_TRUE((IsStringType_v<char*&>));
	EXPECT_TRUE((IsStringType_v<const char*&>));
	EXPECT_TRUE((IsStringType_v<const wchar_t*>));
	EXPECT_TRUE((IsStringType_v<wchar_t*>));
	EXPECT_TRUE((IsStringType_v<decltype(a)>));
	EXPECT_TRUE((IsStringType_v<decltype(b)>));
}

TEST(TypeTraitsTest, IsFloatType) {
	EXPECT_TRUE((IsFloatType_v<float>));
	EXPECT_TRUE((IsFloatType_v<double>));
	EXPECT_TRUE((IsFloatType_v<double&>));
	EXPECT_TRUE((IsFloatType_v<double&&>));
	EXPECT_TRUE((IsFloatType_v<const volatile double&>));
	EXPECT_TRUE((IsFloatType_v<long double&>));
	EXPECT_TRUE((IsFloatType_v<long double>));
}

TEST(TypeTraitsTest, IsIntegerType) {
	EXPECT_TRUE((IsIntegerType_v<Int64>));
	EXPECT_TRUE((IsIntegerType_v<Int64U>));
	EXPECT_TRUE((IsIntegerType_v<Int32>));
	EXPECT_TRUE((IsIntegerType_v<Int32U>));
	EXPECT_TRUE((IsIntegerType_v<Int32L>));
	EXPECT_TRUE((IsIntegerType_v<Int32UL>));
	EXPECT_TRUE((IsIntegerType_v<Int16>));
	EXPECT_TRUE((IsIntegerType_v<Int16U>));


	EXPECT_TRUE((IsIntegerType_v<const Int32U&&>));
	EXPECT_TRUE((IsIntegerType_v<const volatile Int32U&>));
}

TEST(TypeTraitsTest, Move) {
	int a = 10;
	int& b = a;

	int&& z = 30;
	int&& c = JCore::Move(z);
	int&& g = JCore::Forward<int&&>(b);
}


TEST(TypeTraitsTest, DynamicCastable) {
	

	struct NoobModel {};

	EXPECT_TRUE((IsDynamicCastable_v<Model*, SuperModel*>));
	EXPECT_TRUE((IsDynamicCastable_v<Model*, Model*>));
	EXPECT_TRUE((IsDynamicCastable_v<SuperModel*, SuperModel*>));
	EXPECT_TRUE((IsDynamicCastable_v<SuperModel*, Model*>));

	EXPECT_TRUE((IsDynamicCastable_v<Model&, SuperModel&>));
	EXPECT_TRUE((IsDynamicCastable_v<Model&, Model&>));
	EXPECT_TRUE((IsDynamicCastable_v<SuperModel&, SuperModel&>));
	EXPECT_TRUE((IsDynamicCastable_v<SuperModel&, Model&>));

	// 부모 자식관계가 아닌 경우에는 안됨
	EXPECT_FALSE((IsDynamicCastable_v<NoobModel*, SuperModel*>));
	EXPECT_FALSE((IsDynamicCastable_v<Model*, NoobModel*>));
	EXPECT_FALSE((IsDynamicCastable_v<NoobModel*, Model*>));
	EXPECT_TRUE((IsDynamicCastable_v<NoobModel*, NoobModel*>));	// 서로 같은 타입이므로 OK
	

	// 값 타입은 무조건 실패
	EXPECT_FALSE((IsDynamicCastable_v<Model, SuperModel>));
	EXPECT_FALSE((IsDynamicCastable_v<Model, Model>));
	EXPECT_FALSE((IsDynamicCastable_v<SuperModel, SuperModel>));
	EXPECT_FALSE((IsDynamicCastable_v<SuperModel, Model>));

	// 원시타입은 동일한 타입끼리만 허용 / 그래도 값타입은 안댐
	EXPECT_FALSE((IsDynamicCastable_v<int, int>));
	EXPECT_TRUE((IsDynamicCastable_v<int*, int*>));
	EXPECT_TRUE((IsDynamicCastable_v<int&, int&>));
}

TEST(TypeTraitsTest, IsValidAllocator) {
	bool a = IsValidAllocator_v<int, DefaultAllocator>;
	bool b = IsValidAllocator_v<int, DefaultArrayAllocator>;

	EXPECT_TRUE(a);
	EXPECT_TRUE(b);
}

#endif // TEST_TypeTraitsTest == ON