/*
	작성자 : 윤정도
	PCH 겸.. JCore 테스트입니다.
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/Core.h>

#if TEST_CoreTest == ON

TEST(JCoreTest, safe_delete) {
	int* szAlloc = new int;
	*szAlloc = 20;
	safe_delete(szAlloc);
	EXPECT_TRUE(szAlloc == NULL);
}

TEST(JCoreTest, safe_delete_array) {
	int* szAlloc = new int[512];
	safe_delete_array(szAlloc);
	EXPECT_TRUE(szAlloc == NULL);
}

#endif // TEST_CoreTest == ON