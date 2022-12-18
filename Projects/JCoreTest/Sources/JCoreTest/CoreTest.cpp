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

	// 글로벌 릭체크 못하게.. google_test 수행시 내부적으로 할당되는 메모리가 엄청 많아서 체크 불가능
	// 유닛테스트 단위로 릭체크 수행할 수밖에 없다.
	JCore::JCoreSystem_v.EnableLeakCheck(false);
	JCore::Initialize();
    JCore::NormalConsole::Init();
    JCore::NormalConsole::SetOutputCodePage(JCore::CodePage::UTF8);

	::testing::InitGoogleTest(&argc, argv);
	
	int iExitCode = RUN_ALL_TESTS();

	JCore::Finalize();
    return iExitCode;
}
