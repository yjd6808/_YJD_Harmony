/*
	작성자 : 윤정도
	커스텀 예외 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Exception.h>

using namespace JCore;
using namespace std;

#if TEST_ComparatorTest == ON

void CheckTest(int* ptr) {
	if (ptr == nullptr)
		throw NullPointerException("ptr == nullptr");
}

TEST(ExceptionTest, ExceptionTest) {
	int f = 5;
	EXPECT_THROW(CheckTest(nullptr), NullPointerException);
	EXPECT_NO_THROW(CheckTest(&f), NullPointerException);
}

#endif // TEST_ComparatorTest == ON
