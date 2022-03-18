/*
	�ۼ��� : ������
	Ŀ���� ���� �׽�Ʈ
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Exception.h>

using namespace JCore;
using namespace std;


void CheckTest(int* ptr) {
	if (ptr == nullptr)
		throw NullPointerException("ptr == nullptr");
}

TEST(ExceptionTest, ExceptionTest) {
	int f = 5;
	EXPECT_THROW(CheckTest(nullptr), NullPointerException);
	EXPECT_NO_THROW(CheckTest(&f), NullPointerException);
}

