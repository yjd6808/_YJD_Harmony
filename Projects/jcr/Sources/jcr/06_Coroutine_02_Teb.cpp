#include "Core.h"
#include <intrin.h>

// [코루틴-02] TEB 스택 범위 예제. (B안: DeallocationStack은 교체하지 않음)
// - 이 변경점이 없으면: StackBase/Limit만 바뀌어 CRT·언와인더·디버거가 보는
//   범위가 어긋났다. DeallocationStack까지 교체하면 커널이 가드 폴트를 직접
//   확장해 VEH가 안 불리므로(실측 확인) 스레드 원본을 둔다.
static void PrintSection02(const char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		"\n================================================================");
	Console::WriteLine(ConsoleColor::Yellow, "  %s", _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		"================================================================");
}

static void fn_Co02_ReadTeb(CoContext* pCtx)
{
	void* base = (void*)__readgsqword(0x08);
	void* limit = (void*)__readgsqword(0x10);
	void* dealloc = (void*)__readgsqword(0x1478);
	Console::WriteLine(ConsoleColor::Cyan, "    [CO02] TEB 안: Base=0x%p Limit=0x%p Dealloc=0x%p",
		base, limit, dealloc);
	Console::WriteLine(ConsoleColor::Cyan, "    [CO02] 스택   : Base=0x%p End =0x%p",
		pCtx->stack_.pStackBase_, pCtx->stack_.pStackEnd_);
	if (base == pCtx->stack_.pStackBase_)
		Console::WriteLine(ConsoleColor::Green, "  PASS [CO02] StackBase 일치");
	else
		Console::WriteLine(ConsoleColor::Red, "  FAIL [CO02] StackBase 불일치");
}

static void Run_Co02_Teb()
{
	PrintSection02("CO02: 코루틴 안에서 보는 TEB 범위");

	CoContext* pCtx = CoRun(fn_Co02_ReadTeb, cstMid);
	while (pCtx)
		pCtx = CoResume(pCtx);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_Co02_Teb()
{
	Run_Co02_Teb();
	g_cCoMgr.Clear();
}
