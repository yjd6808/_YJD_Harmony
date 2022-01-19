/*
	작성자 : 윤정도
	PCH 겸.. JCore 테스트입니다.
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/TypeTraits.h>
#include <JCore/String/String.h>
#include <JCore/String/StringUtil.h>

using namespace JCore;

TEST(TypeTraitsTest, Type) {

	// 변수명위에 마우스 올려서 확인 ㄱ
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

