#include "Core.h"
#include <float.h>

// [코루틴-08] MXCSR/x87 제어 상태 보존 예제.
// - 이 변경점이 없으면: 코루틴이 반올림 모드를 바꾸고 yield하면 스레드와
//   다른 모든 코루틴이 바뀐 모드로 계산했다. (Win x64 ABI 위반)
static void PrintSection08(const char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		"\n================================================================");
	Console::WriteLine(ConsoleColor::Yellow, "  %s", _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		"================================================================");
}

static void fn_Co08_Rounding(CoContext*)
{
	unsigned old = 0;
	_controlfp_s(&old, _RC_DOWN, _MCW_RC);	// 코루틴에서 내림 모드로 변경
	Console::WriteLine(ConsoleColor::Cyan, "    [CO08] 코루틴: 내림 모드 설정 후 yield");
	CoYield();
	unsigned now = 0;
	_controlfp_s(&now, 0, 0);
	if ((now & _MCW_RC) == _RC_DOWN)
		Console::WriteLine(ConsoleColor::Green, "  PASS [CO08] 재개 후에도 내림 모드 유지");
	else
		Console::WriteLine(ConsoleColor::Red, "  FAIL [CO08] 모드 유실");
}

static void Test_Co08_Rounding()
{
	PrintSection08("CO08: 반올림 모드 격리");

	CoContext* pCtx = CoRun(fn_Co08_Rounding, cstMid);

	unsigned mid = 0;
	_controlfp_s(&mid, 0, 0);
	if ((mid & _MCW_RC) == _RC_NEAR)
		Console::WriteLine(ConsoleColor::Green, "  PASS [CO08] 스레드 쪽은 그대로(최근접)");
	else
		Console::WriteLine(ConsoleColor::Red, "  FAIL [CO08] 스레드 쪽 오염");

	while (pCtx)
		pCtx = CoResume(pCtx);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_Co08_Mxcsr()
{
	Test_Co08_Rounding();
	g_cCoMgr.Clear();
}
