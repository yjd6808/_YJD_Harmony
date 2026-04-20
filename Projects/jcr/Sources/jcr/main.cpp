#include "Core.h"

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
