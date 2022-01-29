/*
	작성자 : 윤정도
	PCH 겸.. JCore 테스트입니다.
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/TypeTraits.h>
#include <JCore/String.h>
#include <JCore/StringUtil.h>
#include <JCore/Core.h>

using namespace JCore;
using namespace std;

#if TEST_TypeTraitsTest == ON

TEST(TypeTraitsTest, Type) {
	EXPECT_TRUE(Type<int&>() == "int&");
	EXPECT_TRUE(Type<const int&>() == "const int&");
	EXPECT_TRUE(Type<int&&>() == "int&&");
	// 변수명위에 마우스 올려서 확인 ㄱ
}

TEST(TypeTraitsTest, CTType) {
	EXPECT_TRUE(CTType<int&>() == "int&");
	EXPECT_TRUE(CTType<const int&>() == "const int&");
	EXPECT_TRUE(CTType<int&&>() == "int&&");
}

TEST(TypeTraitsTest, Type_v) {
	EXPECT_TRUE(Type_v<int> == "int");
	EXPECT_TRUE(Type_v<int> != "~int");
	EXPECT_TRUE(Type_v<int> != "int~");
	EXPECT_TRUE(Type_v<int> != "in");
	EXPECT_TRUE(Type_v<int> != "nt");
}

TEST(TypeTraitsTest, IsSameType_v) {
	EXPECT_TRUE((IsSameType_v<int, int>));	// 일치
	EXPECT_TRUE((IsSameType_v<int, int*>) == false);
	EXPECT_TRUE((IsSameType_v<int*, int>) == false);
	EXPECT_TRUE((IsSameType_v<int, char>) == false);
	EXPECT_TRUE((IsSameType_v<char*, char*>));	// 일치
	EXPECT_TRUE((IsSameType_v<char*, char**>) == false);
	EXPECT_TRUE((IsSameType_v<const char&, const char>) == false);
}

TEST(TypeTraitsTest, IsStringType_v) {
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

TEST(TypeTraitsTest, IsFloatingPointType_v) {
	EXPECT_TRUE((IsFloatingPointType_v<float>));
	EXPECT_TRUE((IsFloatingPointType_v<double>));
	EXPECT_TRUE((IsFloatingPointType_v<double&>));
	EXPECT_TRUE((IsFloatingPointType_v<double&&>));
	EXPECT_TRUE((IsFloatingPointType_v<const volatile double&>));
	EXPECT_TRUE((IsFloatingPointType_v<long double&>));
	EXPECT_TRUE((IsFloatingPointType_v<long double>));
}

TEST(TypeTraitsTest, IsIntegerType_v) {
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



#endif // TEST_TypeTraitsTest == ON