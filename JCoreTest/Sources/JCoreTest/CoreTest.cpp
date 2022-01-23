/*
	작성자 : 윤정도
	PCH 겸.. JCore 테스트입니다.
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/Core.h>

#if TEST_CoreTest == ON

TEST(JCoreTest, DeleteSafe) {
	int* szAlloc = new int;
	*szAlloc = 20;
	DeleteSafe(szAlloc);
	EXPECT_TRUE(szAlloc == NULL);
}

TEST(JCoreTest, DeleteArraySafe) {
	int* szAlloc = new int[512];
	DeleteArraySafe(szAlloc);
	EXPECT_TRUE(szAlloc == NULL);
}

#endif // TEST_CoreTest == ON