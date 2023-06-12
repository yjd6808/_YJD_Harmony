/*
	작성자 : 윤정도
	PCH 겸.. JCore 테스트입니다.
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/Encoding/CodePage.h>
#include <JCore/Utils/Console.h>
#include <JCore/Random.h>

int R(int min, int max) {
	return Random::GenerateInt(min, max);
}

#if TEST_CoreTest == ON

TEST(JCoreTest, DeleteSafe) {
	int* szAlloc = new int;
	*szAlloc = 20;
	JCORE_DELETE_SAFE(szAlloc);
	EXPECT_TRUE(szAlloc == NULL);
}

TEST(JCoreTest, DeleteArraySafe) {
	int* szAlloc = new int[512];
	JCORE_DELETE_ARRAY_SAFE(szAlloc);
	EXPECT_TRUE(szAlloc == NULL);
}

#endif // TEST_CoreTest == ON


// 원래 main 함수 필요없는데
// 전 프로젝트 UTF8로 교체함에따라 main 함수 명시적으로 추가해줌
int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	Random::EngineInitialize();
    return RUN_ALL_TESTS();
}
