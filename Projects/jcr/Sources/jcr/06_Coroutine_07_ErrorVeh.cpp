#include "Core.h"

static void PrintSection07(const _char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		_T("\n================================================================"));
	Console::WriteLine(ConsoleColor::Yellow, _T("  %s"), _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		_T("================================================================"));
}

// fn이 null이면 코루틴을 만들 수 없으므로 실패 원인이 보여야 한다.
static void Test_Co07_NullFunction()
{
	PrintSection07(_T("CO07-1: CoRun(null) → coeNullFunction"));

	CoId id = CoRun(nullptr);
	if (id == CO_INVALID_ID && CoGetLastError() == coeNullFunction)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO07-1] %s"), CoErrorString(coeNullFunction));
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO07-1] 원인이 보이지 않음"));
}

// VEH를 직접 등록하지 않아도 스택 확장이 동작해야 한다.
static void fn_Co07_Grow(CoContext*)
{
	volatile char buf[10 * 1024];
	for (int i = 0; i < (int)sizeof(buf); i += 4096)
		buf[i] = (char)i;
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO07-2] 10KB 터치 완료 (VEH 자동 등록으로 확장됨)"));
}

static void Test_Co07_AutoVeh()
{
	PrintSection07(_T("CO07-2: VEH 미등록 상태에서 10KB 스택 확장"));

	CoId id = CoRun(fn_Co07_Grow);
	if (id == CO_INVALID_ID && CoGetLastError() == coeNone)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO07-2] 확장 후 정상 종료"));
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO07-2] 확장 실패"));
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_Co07_ErrorVeh()
{
	Test_Co07_NullFunction();
	Test_Co07_AutoVeh();
	g_cCoMgr.Clear();
}
