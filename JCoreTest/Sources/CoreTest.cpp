/*
	작성자 : 윤정도
	JCore 테스트입니다.
*/

#include "JPCH.h"

#include <JCore/Core.h>


TEST(JCoreTest, JSAFE_DELETE) {
	int* szAlloc = new int;
	*szAlloc = 20;
	JSAFE_DELETE(szAlloc);
	EXPECT_TRUE(szAlloc == NULL);
}

TEST(JCoreTest, JSAFE_DELETE_ARRAY) {
	int* szAlloc = new int[512];
	JSAFE_DELETE_ARRAY(szAlloc);
	EXPECT_TRUE(szAlloc == NULL);
}