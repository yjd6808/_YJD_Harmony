/*
	작성자 : 윤정도
	커스텀 예외 테스트
*/



#if TEST_ExceptionTest == ON

#include "jc/Exception.h"


using namespace std;

void CheckTest(int* ptr) {
	if (ptr == nullptr)
		throw NullPointerException("ptr == nullptr");
}

TEST(ExceptionTest, ExceptionTest) {
	int f = 5;
	EXPECT_THROW(CheckTest(nullptr), NullPointerException);
	EXPECT_NO_THROW(CheckTest(&f));
}

#endif
