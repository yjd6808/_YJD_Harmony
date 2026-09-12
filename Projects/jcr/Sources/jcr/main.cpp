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
	pOption->inputTitle_              = _T("수행할 함수를 선택> ");
	pOption->inputLeftBrace_          = _T("[");
	pOption->inputRightBrace_         = _T("]");
	pOption->inputRightPadding_       = _T(" ");
	pOption->pressAnyKeyAfterCallback_ = true;

	// ====================================================
	// 루트 메뉴 구성
	// ====================================================
	auto pRoot = jc::MakeShared<ConsoleMenuItem>();
	pRoot->AddHeader(
		_T("========================================\n")
		_T("           학습 프로그램 메뉴\n")
		_T("========================================"));
	pRoot->Add(_T("1"), _T("ASM Study - 어셈블리 기초 학습"),
		CONSOLE_MENU_ACTION(call_01_AsmStudy(_argc, _argv))	
	);
	pRoot->Add(_T("2"), _T("Heap Study - 힙 메모리 학습"),
		CONSOLE_MENU_ACTION(call_02_HeapStudy(_argc, _argv))
	);
	pRoot->Add(_T("3"), _T("RSP Game - 가위바위보 게임"),
		CONSOLE_MENU_ACTION(call_03_RSPGame(_argc, _argv))
	);
	pRoot->Add(_T("4"), _T("CoStack R&D - 코루틴용 스택 연구"),
		CONSOLE_MENU_ACTION(call_04_CoStack(_argc, _argv))
	);
	pRoot->Add(_T("5"), _T("PageGuard - 페이지 가드 터치"),
		CONSOLE_MENU_ACTION(call_05_PageGuard(_argc, _argv))
	);

	// 6. 코루틴은 별도 서브메뉴로 묶는다. (선택하면 코루틴 예제만 나옴)
	ConsoleMenuItem* pCoroutine = nullptr;
	pRoot->AddMenu(_T("6"), _T("Coroutine - 코루틴 연구"), pCoroutine);
	pCoroutine->AddHeader(
		_T("========================================\n")
		_T("           코루틴 예제 메뉴\n")
		_T("========================================"));
	pCoroutine->Add(_T("1"), _T("Coroutine 기초 TC"),
		CONSOLE_MENU_ACTION(Test_CoBasic())
	);
	pCoroutine->Add(_T("2"), _T("07 에러보고 + VEH 자동등록"),
		CONSOLE_MENU_ACTION(Test_Co07_ErrorVeh())
	);
	pCoroutine->Add(_T("3"), _T("05 CoResume 안전장치 + 세대핸들"),
		CONSOLE_MENU_ACTION(Test_Co05_ResumeSafety())
	);
	pCoroutine->Add(_T("4"), _T("06 cstCustom 크기정규화"),
		CONSOLE_MENU_ACTION(Test_Co06_CustomTier())
	);
	pCoroutine->Add(_T("5"), _T("01 예외 전파 방어"),
		CONSOLE_MENU_ACTION(Test_Co01_Exception())
	);
	pCoroutine->Add(_T("6"), _T("02 TEB 스택 범위"),
		CONSOLE_MENU_ACTION(Test_Co02_Teb())
	);
	pCoroutine->Add(_T("7"), _T("03 StackLimit 동기화"),
		CONSOLE_MENU_ACTION(Test_Co03_StackLimit())
	);
	pCoroutine->Add(_T("8"), _T("08 MXCSR 보존"),
		CONSOLE_MENU_ACTION(Test_Co08_Mxcsr())
	);
	pCoroutine->Add(_T("9"), _T("04 오버플로우 EmergencyPages"),
		CONSOLE_MENU_ACTION(Test_Co04_Emergency())
	);
	pCoroutine->Add(_T("10"), _T("15 VEH 자체 방어"),
		CONSOLE_MENU_ACTION(Test_Co15_VehDefense())
	);
	pCoroutine->Add(_T("11"), _T("14 API 편의성"),
		CONSOLE_MENU_ACTION(Test_Co14_Api())
	);
	pCoroutine->Add(_T("12"), _T("전체 실행"),
		CONSOLE_MENU_ACTION(call_06_Coroutine(_argc, _argv))
	);
	pCoroutine->AddBack(_T("0"), _T("뒤로"));
	pRoot->AddBack(_T("0"), _T("종료"));

	// ====================================================
	// 메뉴 실행 (Back 선택 시 루프 종료)
	// ====================================================
	ConsoleMenuItem::Show(pRoot, pOption);
	Console::WriteLine(_T("\n프로그램을 종료합니다."));

	FinalizeDefaultLogger();
	FinalizeJCore();
	return 0;
}
