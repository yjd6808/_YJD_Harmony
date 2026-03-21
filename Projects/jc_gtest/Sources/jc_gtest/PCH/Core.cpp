/*
	작성자 : 윤정도
	PCH 겸.. jc 테스트입니다.
*/

#include "jc/Encoding/CodePage.h"
#include "jc/Utils/Console.h"
#include "jc/Random.h"

#include "jc/Container/PropertyStatics.h"

static Random g_cR;

int R(int _min, int _max) 
{
	return g_cR.GenerateInt(_min, _max);
}

#if TEST_CoreTest == ON

TEST(JCoreTest, DeleteSafe) 
{
	int* szAlloc = new int;
	*szAlloc = 20;
	JC_DELETE_SAFE(szAlloc);
	EXPECT_TRUE(szAlloc == NULL);
}

TEST(JCoreTest, DeleteArraySafe) 
{
	int* szAlloc = new int[512];
	JC_DELETE_ARRAY_SAFE(szAlloc);
	EXPECT_TRUE(szAlloc == NULL);
}

#endif // TEST_CoreTest == ON

// 원래 main 함수 필요없는데
// 전 프로젝트 UTF8로 교체함에따라 main 함수 명시적으로 추가해줌
int main(int _argc, char** _argv) 
{
	::testing::InitGoogleTest(&_argc, _argv);
	InitializeJCore(_argc, _argv);
	g_cR.Initialize();
	const int ret = RUN_ALL_TESTS();
	PropertyStatics::Finalize();
	FinalizeJCore();
    return ret;
}
