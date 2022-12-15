/*
	작성자 : 윤정도
	PCH 겸.. JCore 테스트입니다.
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/Core.h>
#include <JCore/Encoding/CodePage.h>
#include <JCore/Utils/Console.h>
#include <JCore/System/JCoreSystem.h>


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


// 원래 main 함수 필요없는데
// 전 프로젝트 UTF8로 교체함에따라 main 함수 명시적으로 추가해줌
int main(int argc, char** argv) {
	JCore::Initialize();
	JCore::JCoreSystem_v.EnableLeakCheck(false);	// 글로벌 릭체크 안함. 테스트한다고 아무렇게나 동적할당하는 코드를 작성해서;
													// 테스트 단위별로 릭체크를 수행토록 해야함
    JCore::NormalConsole::Init();
    JCore::NormalConsole::SetOutputCodePage(JCore::CodePage::UTF8);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
