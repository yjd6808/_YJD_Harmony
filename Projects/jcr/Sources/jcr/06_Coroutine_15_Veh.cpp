#include "Core.h"

// [코루틴-15] VEH 자체 방어 예제.
// - 이 변경점이 없으면: VEH 안에서 나는 폴트는 디버깅이 거의 불가능하고,
//   재진입 플래그가 눌러붙거나 남의 가드를 삼켜 조용히 망가졌다.
// - VEH 통계(실제 쓴 깊이)를 보고 가드 예산이 충분한지 확인한다.
static void PrintSection15(const _char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		_T("\n================================================================"));
	Console::WriteLine(ConsoleColor::Yellow, _T("  %s"), _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		_T("================================================================"));
}

static void fn_Co15_Grow(CoContext*)
{
	volatile char buf[10 * 1024];
	for (int i = 0; i < (int)sizeof(buf); i += 4096)
		buf[i] = (char)i;
}

static void Test_Co15_Stats()
{
	PrintSection15(_T("CO15: VEH 깊이 통계"));

	CoContext* pCtx = CoRun(fn_Co15_Grow, cstMid);
	while (pCtx)
		pCtx = CoResume(pCtx);

	CoMgr::CoVehStats st = g_cCoMgr.GetVehStats();
	Console::WriteLine(ConsoleColor::Cyan,
		_T("    [CO15] maxDispatchUsed=%llu minRemain=%lld"),
		(unsigned long long)st.maxDispatchUsed, (long long)st.minRemain);
	if (st.maxDispatchUsed == 0 && st.minRemain == (size_t)-1)
		Console::WriteLine(ConsoleColor::Cyan,
			_T("    [CO15] 우리 VEH가 안 불림 (커널이 성장을 직접 처리함)"));
	else if ((long long)st.minRemain >= 2048)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO15] 가드 여유 충분"));
	else
		Console::WriteLine(ConsoleColor::Yellow, _T("  WARN [CO15] 가드 여유 부족. 예산 상향 검토"));
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_Co15_VehDefense()
{
	Test_Co15_Stats();
	g_cCoMgr.Clear();
}
