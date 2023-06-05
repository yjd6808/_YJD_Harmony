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
	constexpr bool a = IsValidAllocator_v<int, DefaultAllocator>;
	constexpr bool b = IsValidAllocator_v<int, DefaultArrayAllocator>;

	EXPECT_TRUE(a);
	EXPECT_TRUE(b);
}

static int* globalFn(char, int*, double&) {}
struct op
{
	void operator()() {}
	void memberFn() {}
	static void staticFn() {}
};
TEST(TypeTraitsTest, IsCallable) {
	EXPECT_TRUE(IsCallable_v<decltype([] {})>);
	EXPECT_TRUE(IsCallable_v<op>);
	EXPECT_TRUE(IsCallable_v<decltype(&op::memberFn)>);
	EXPECT_TRUE(IsCallable_v<decltype(&op::staticFn)>);
	EXPECT_TRUE(IsCallable_v<decltype(op::staticFn)>);
	EXPECT_TRUE(IsCallable_v<decltype(globalFn)>);
	EXPECT_TRUE(IsCallable_v<decltype(&globalFn)>);
}

static void globalFn1(int) {}
static bool globalFn2(int*, double*) { return true; }
struct t2
{
	struct op1 { void operator()(int) {} };
	struct op2 { bool operator()(int*, double*) { return true; } };
	void memberFn1(int) {}
	bool memberFn2(int*, double*) { return true; }
	static void staticFn1(int) {}
	static bool staticFn2(int*, double*) { return true; }
};

TEST(TypeTraitsTest, CallableSignature) {
	auto lambda1 = [](int) {};
	auto lambda2 = [](int*, double*)->bool { return true; };

	constexpr bool _1 = IsSameType_v<CallableSignature_t<decltype(lambda1)>::Parameters, ParameterPack_t<int>>;
	constexpr bool _2 = IsSameType_v<CallableSignature_t<decltype(lambda1)>::Return, void>;

	constexpr bool _3 = IsSameType_v<CallableSignature_t<decltype(lambda2)>::Parameters, ParameterPack_t<int*, double*>>;
	constexpr bool _4 = IsSameType_v<CallableSignature_t<decltype(lambda2)>::Return, bool>;

	constexpr bool _5 = IsSameType_v<CallableSignature_t<decltype(globalFn1)>::Parameters, ParameterPack_t<int>>;
	constexpr bool _6 = IsSameType_v<CallableSignature_t<decltype(globalFn1)>::Return, void>;

	constexpr bool _7 = IsSameType_v<CallableSignature_t<decltype(&globalFn1)>::Parameters, ParameterPack_t<int>>;
	constexpr bool _8 = IsSameType_v<CallableSignature_t<decltype(&globalFn1)>::Return, void>;

	constexpr bool _9 = IsSameType_v<CallableSignature_t<decltype(globalFn2)>::Parameters, ParameterPack_t<int*, double*>>;
	constexpr bool _10 = IsSameType_v<CallableSignature_t<decltype(globalFn2)>::Return, bool>;

	constexpr bool _11 = IsSameType_v<CallableSignature_t<decltype(&globalFn2)>::Parameters, ParameterPack_t<int*, double*>>;
	constexpr bool _12 = IsSameType_v<CallableSignature_t<decltype(&globalFn2)>::Return, bool>;

	constexpr bool _13 = IsSameType_v<CallableSignature_t<decltype(&t2::memberFn1)>::Parameters, ParameterPack_t<int>>;
	constexpr bool _14 = IsSameType_v<CallableSignature_t<decltype(&t2::memberFn1)>::Return, void>;

	constexpr bool _15 = IsSameType_v<CallableSignature_t<decltype(&t2::memberFn2)>::Parameters, ParameterPack_t<int*, double*>>;
	constexpr bool _16 = IsSameType_v<CallableSignature_t<decltype(&t2::memberFn2)>::Return, bool>;

	constexpr bool _17 = IsSameType_v<CallableSignature_t<decltype(t2::staticFn1)>::Parameters, ParameterPack_t<int>>;
	constexpr bool _18 = IsSameType_v<CallableSignature_t<decltype(t2::staticFn1)>::Return, void>;

	constexpr bool _19 = IsSameType_v<CallableSignature_t<decltype(&t2::staticFn1)>::Parameters, ParameterPack_t<int>>;
	constexpr bool _20 = IsSameType_v<CallableSignature_t<decltype(&t2::staticFn1)>::Return, void>;

	constexpr bool _21 = IsSameType_v<CallableSignature_t<decltype(t2::staticFn2)>::Parameters, ParameterPack_t<int*, double*>>;
	constexpr bool _22 = IsSameType_v<CallableSignature_t<decltype(t2::staticFn2)>::Return, bool>;

	constexpr bool _23 = IsSameType_v<CallableSignature_t<decltype(&t2::staticFn2)>::Parameters, ParameterPack_t<int*, double*>>;
	constexpr bool _24 = IsSameType_v<CallableSignature_t<decltype(&t2::staticFn2)>::Return, bool>;


	EXPECT_TRUE(_1);
	EXPECT_TRUE(_2);
	EXPECT_TRUE(_3);
	EXPECT_TRUE(_4);
	EXPECT_TRUE(_5);
	EXPECT_TRUE(_6);
	EXPECT_TRUE(_7);
	EXPECT_TRUE(_8);
	EXPECT_TRUE(_9);
	EXPECT_TRUE(_10);
	EXPECT_TRUE(_11);
	EXPECT_TRUE(_12);
	EXPECT_TRUE(_13);
	EXPECT_TRUE(_14);
	EXPECT_TRUE(_15);
	EXPECT_TRUE(_16);
	EXPECT_TRUE(_17);
	EXPECT_TRUE(_18);
	EXPECT_TRUE(_19);
	EXPECT_TRUE(_20);
	EXPECT_TRUE(_21);
	EXPECT_TRUE(_22);
	EXPECT_TRUE(_23);
	EXPECT_TRUE(_24);
}

#endif // TEST_TypeTraitsTest == ON