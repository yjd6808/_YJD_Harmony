/*
	�ۼ��� : ������
	PCH ��.. JCore �׽�Ʈ�Դϴ�.
*/

#include <JCoreTest/CoreTest.h>
#include <JCoreTest/TestUtil/Object.h>

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
	// ���������� ���콺 �÷��� Ȯ�� ��
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
	EXPECT_TRUE((IsSameType_v<int, int>));	// ��ġ
	EXPECT_TRUE((IsSameType_v<int, int*>) == false);
	EXPECT_TRUE((IsSameType_v<int*, int>) == false);
	EXPECT_TRUE((IsSameType_v<int, char>) == false);
	EXPECT_TRUE((IsSameType_v<char*, char*>));	// ��ġ
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


TEST(TypeTraitsTest, DynamicCastable_v) {
	

	struct NoobModel {};

	EXPECT_TRUE((DynamicCastable_v<Model*, SuperModel*>));
	EXPECT_TRUE((DynamicCastable_v<Model*, Model*>));
	EXPECT_TRUE((DynamicCastable_v<SuperModel*, SuperModel*>));
	EXPECT_TRUE((DynamicCastable_v<SuperModel*, Model*>));

	EXPECT_TRUE((DynamicCastable_v<Model&, SuperModel&>));
	EXPECT_TRUE((DynamicCastable_v<Model&, Model&>));
	EXPECT_TRUE((DynamicCastable_v<SuperModel&, SuperModel&>));
	EXPECT_TRUE((DynamicCastable_v<SuperModel&, Model&>));

	// �θ� �ڽİ��谡 �ƴ� ��쿡�� �ȵ�
	EXPECT_FALSE((DynamicCastable_v<NoobModel*, SuperModel*>));
	EXPECT_FALSE((DynamicCastable_v<Model*, NoobModel*>));
	EXPECT_FALSE((DynamicCastable_v<NoobModel*, Model*>));
	EXPECT_TRUE((DynamicCastable_v<NoobModel*, NoobModel*>));	// ���� ���� Ÿ���̹Ƿ� OK
	

	// �� Ÿ���� ������ ����
	EXPECT_FALSE((DynamicCastable_v<Model, SuperModel>));
	EXPECT_FALSE((DynamicCastable_v<Model, Model>));
	EXPECT_FALSE((DynamicCastable_v<SuperModel, SuperModel>));
	EXPECT_FALSE((DynamicCastable_v<SuperModel, Model>));

	// ����Ÿ���� ������ Ÿ�Գ����� ��� / �׷��� ��Ÿ���� �ȴ�
	EXPECT_FALSE((DynamicCastable_v<int, int>));
	EXPECT_TRUE((DynamicCastable_v<int*, int*>));
	EXPECT_TRUE((DynamicCastable_v<int&, int&>));
}

#endif // TEST_TypeTraitsTest == ON