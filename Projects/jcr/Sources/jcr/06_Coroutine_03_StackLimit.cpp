#include "Core.h"
#include <intrin.h>

// [코루틴-03] 확장 후 TEB StackLimit 동기화 예제.
// - 이 변경점이 없으면: 확장 뒤 gs:[16]이 그대로라 확장된 영역에서 예외를
//   던지면 SEH가 스택 범위를 벗어났다고 보고 프로세스를 죽였다.
// - 한계 동기화는 yield 시점에 살아있는 TEB 값을 기준으로 맞춘다.
static void PrintSection03(const _char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		_T("\n================================================================"));
	Console::WriteLine(ConsoleColor::Yellow, _T("  %s"), _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		_T("================================================================"));
}

static void fn_Co03_GrowThrow(CoContext* pCtx)
{
	volatile char big[CO_PAGE_SIZE * 4];
	for (int i = (int)sizeof(big) - 1; i >= 0; i -= CO_PAGE_SIZE)
		big[i] = (char)i;
	(void)big;

	CoYield();	// 여기서 한계 동기화

	void* limitNow = (void*)__readgsqword(0x10);
	if (limitNow == pCtx->stack_.pStackLimit_)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO03] TEB 한계 동기화됨"));
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO03] TEB 한계 불일치"));

	try { throw 42; }
	catch (int v)
	{
		if (v == 42)
			Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO03] 확장 영역에서 throw/catch"));
		else
			Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO03] 값 불일치"));
	}
}

static void Test_Co03_GrowThrow()
{
	PrintSection03(_T("CO03: 16KB 확장 후 throw/catch + 한계 동기화"));

	CoContext* pCtx = CoRun(fn_Co03_GrowThrow, cstMid);
	while (pCtx)
		pCtx = CoResume(pCtx);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_Co03_StackLimit()
{
	Test_Co03_GrowThrow();
	g_cCoMgr.Clear();
}
