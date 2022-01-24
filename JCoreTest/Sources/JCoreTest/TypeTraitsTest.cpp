/*
	�ۼ��� : ������
	PCH ��.. JCore �׽�Ʈ�Դϴ�.
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/TypeTraits.h>
#include <JCore/String.h>
#include <JCore/StringUtil.h>

using namespace JCore;
using namespace std;

#if TEST_TypeTraitsTest == ON

TEST(TypeTraitsTest, Type) {
	EXPECT_TRUE(Type<int&>() == "int&");
	EXPECT_TRUE(Type<const int&>() == "const int&");
	EXPECT_TRUE(Type<int&&>() == "int&&");
	// ���������� ���콺 �÷��� Ȯ�� ��
}


TEST(TypeTraitsTest, Move) {
	int a = 10;
	int& b = a;

	int&& z = 30;
	int&& c = JCore::Move(z);
	int&& g = JCore::Forward<int&&>(b);
}

#endif // TEST_TypeTraitsTest == ON