/*
	작성자 : 윤정도
	PCH 겸.. JCore 테스트입니다.
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
	// 변수명위에 마우스 올려서 확인 ㄱ
}


TEST(TypeTraitsTest, Move) {
	int a = 10;
	int& b = a;

	int&& z = 30;
	int&& c = JCore::Move(z);
	int&& g = JCore::Forward<int&&>(b);
}

#endif // TEST_TypeTraitsTest == ON