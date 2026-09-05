#include "Core.h"
#include "06_Coroutine_Menu.h"

static auto fn_running_thread = []
{
	jc::Thread th([] (void* _param)
		{
		}, nullptr, "RunningThread", true);
	th.Abort();
	return 10;
}();

#pragma pack(push, 1)
struct check_point
{
	// callee saved registers
	_u64 rbx_ = 0;
	_u64 rbp_ = 0;
	_u64 rdi_ = 0;
	_u64 rsp_ = 0;
	_u64 r12_ = 0;
	_u64 r13_ = 0;
	_u64 r14_ = 0;
	_u64 r15_ = 0;
	char xmm_[16 * 10]{}; // xmm6 ~ xmm15
	bool saved_ = false;
} ck;
#pragma pack(pop)



int main(int _argc, char** _argv) 
{
	new char;
	VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	InitializeJCore(0, nullptr);
	InitializeDefaultLogger();

	// ====================================================
	// 메뉴 옵션 설정
	// ====================================================
	auto pOption = jc::MakeShared<ConsoleMenuItemOption>();
	pOption->inputTitle_              = "수행할 함수를 선택> ";
	pOption->inputLeftBrace_          = "[";
	pOption->inputRightBrace_         = "]";
	pOption->inputRightPadding_       = " ";
	pOption->pressAnyKeyAfterCallback_ = true;

	// ====================================================
	// 루트 메뉴 구성
	// ====================================================
	auto pRoot = jc::MakeShared<ConsoleMenuItem>();
	pRoot->AddHeader(
		"========================================\n"
		"           학습 프로그램 메뉴\n"
		"========================================");
	pRoot->Add("1", "ASM Study - 어셈블리 기초 학습",
		CONSOLE_MENU_ACTION(call_01_AsmStudy(_argc, _argv))	
	);
	pRoot->Add("2", "Heap Study - 힙 메모리 학습",
		CONSOLE_MENU_ACTION(call_02_HeapStudy(_argc, _argv))
	);
	pRoot->Add("3", "RSP Game - 가위바위보 게임",
		CONSOLE_MENU_ACTION(call_03_RSPGame(_argc, _argv))
	);
	pRoot->Add("4", "CoStack R&D - 코루틴용 스택 연구",
		CONSOLE_MENU_ACTION(call_04_CoStack(_argc, _argv))
	);
	pRoot->Add("5", "PageGuard - 페이지 가드 터치",
		CONSOLE_MENU_ACTION(call_05_PageGuard(_argc, _argv))
	);

	// 6. 코루틴은 별도 서브메뉴로 묶는다. (선택하면 코루틴 예제만 나옴)
	ConsoleMenuItem* pCoroutine = nullptr;
	pRoot->AddMenu("6", "Coroutine - 코루틴 연구", pCoroutine);
	pCoroutine->AddHeader(
		"========================================\n"
		"           코루틴 예제 메뉴\n"
		"========================================");
	pCoroutine->Add("1", "Coroutine 기초 TC",
		CONSOLE_MENU_ACTION(Test_CoBasic())
	);
	pCoroutine->Add("2", "07 에러보고 + VEH 자동등록",
		CONSOLE_MENU_ACTION(Test_Co07_ErrorVeh())
	);
	pCoroutine->Add("3", "05 CoResume 안전장치 + 세대핸들",
		CONSOLE_MENU_ACTION(Test_Co05_ResumeSafety())
	);
	pCoroutine->Add("4", "06 cstCustom 크기정규화",
		CONSOLE_MENU_ACTION(Test_Co06_CustomTier())
	);
	pCoroutine->Add("5", "01 예외 전파 방어",
		CONSOLE_MENU_ACTION(Test_Co01_Exception())
	);
	pCoroutine->Add("6", "02 TEB 스택 범위",
		CONSOLE_MENU_ACTION(Test_Co02_Teb())
	);
	pCoroutine->Add("7", "03 StackLimit 동기화",
		CONSOLE_MENU_ACTION(Test_Co03_StackLimit())
	);
	pCoroutine->Add("8", "08 MXCSR 보존",
		CONSOLE_MENU_ACTION(Test_Co08_Mxcsr())
	);
	pCoroutine->Add("9", "04 오버플로우 EmergencyPages",
		CONSOLE_MENU_ACTION(Test_Co04_Emergency())
	);
	pCoroutine->Add("10", "15 VEH 자체 방어",
		CONSOLE_MENU_ACTION(Test_Co15_VehDefense())
	);
	pCoroutine->Add("11", "14 API 편의성",
		CONSOLE_MENU_ACTION(Test_Co14_Api())
	);
	pCoroutine->Add("12", "전체 실행",
		CONSOLE_MENU_ACTION(call_06_Coroutine(_argc, _argv))
	);
	pCoroutine->AddBack("0", "뒤로");
	pRoot->AddBack("0", "종료");

	// ====================================================
	// 메뉴 실행 (Back 선택 시 루프 종료)
	// ====================================================
	ConsoleMenuItem::Show(pRoot, pOption);
	Console::WriteLine("\n프로그램을 종료합니다.");

	FinalizeDefaultLogger();
	FinalizeJCore();
	return 0;
}
