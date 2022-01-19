/*
	�ۼ��� : ������
	PCH ��.. JCore �׽�Ʈ�Դϴ�.
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/TypeTraits.h>
#include <JCore/String/String.h>
#include <JCore/String/StringUtil.h>

using namespace JCore;

TEST(TypeTraitsTest, Type) {

	// ���������� ���콺 �÷��� Ȯ�� ��
	constexpr const char* str1 = JCore::Type<int&&>();
	constexpr const char* str2 = JCore::Type<int&>();
	constexpr const char* str3 = JCore::Type<int>();
}


TEST(TypeTraitsTest, Move) {
	int a = 10;
	int& b = a;

	int&& z = 30;
	int&& c = JCore::Move(z);

	int&& g = JCore::Forward<int&&>(b);
}

