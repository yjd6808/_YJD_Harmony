#include "Core.h"
#include <thread>

// [코루틴-05] CoResume 안전장치 + 세대 핸들 예제.
// - 이 변경점이 없으면: 다른 스레드에서 resume해도 막지 못해 풀이 오염되고,
//   종료된 컨텍스트를 예전 포인터로 resume하면 엉뚱한 코루틴이 실행된다(ABA).
static void PrintSection05(const _char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		_T("\n================================================================"));
	Console::WriteLine(ConsoleColor::Yellow, _T("  %s"), _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		_T("================================================================"));
}

static volatile bool g_ran05 = false;
static void fn_Co05_Flag(CoContext*)
{
	CoYield();
	g_ran05 = true;
}

// 다른 스레드에서 resume → 거부되고 본문은 실행되지 않는다.
static void Test_Co05_WrongThread()
{
	PrintSection05(_T("CO05-1: 다른 스레드에서 resume → coeWrongThread"));

	g_ran05 = false;
	CoContext* pCtx = CoRun(fn_Co05_Flag, cstLow);

	std::thread th([&]
	{
		CoContext* r = CoResume(pCtx);
		if (r == nullptr && CoGetLastError() == coeWrongThread)
			Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO05-1] 타 스레드 resume 거부"));
		else
			Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO05-1] 막지 못함"));
	});
	th.join();

	if (!g_ran05)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO05-1] 본문 미실행 확인"));
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO05-1] 본문이 실행됨"));

	while (pCtx)
		pCtx = CoResume(pCtx);
}

// 자기 자신을 resume → 거부된다.
static void fn_Co05_Self(CoContext*)
{
	CoContext* r = CoResume(CoCurrentCtx());
	if (r == nullptr && CoGetLastError() == coeInvalidState)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO05-2] 자기 resume 거부"));
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO05-2] 막지 못함"));
}

static void Test_Co05_Self()
{
	PrintSection05(_T("CO05-2: 자기 자신을 resume → coeInvalidState"));

	CoContext* pCtx = CoRun(fn_Co05_Self, cstLow);
	while (pCtx)
		pCtx = CoResume(pCtx);
}

// 종료 후 같은 주소가 재사용돼도 예전 핸들은 죽은 핸들이다.
static void fn_Co05_Once(CoContext*)
{
	CoYield();
}

static void Test_Co05_StaleHandle()
{
	PrintSection05(_T("CO05-3: 재사용된 주소 + 예전 핸들 → coeStaleHandle"));

	g_cCoMgr.Clear();
	CoHandle h = CoRunH(fn_Co05_Once, cstLow);
	while (CoResumeH(h) != nullptr) {}

	CoHandle hB = CoRunH(fn_Co05_Once, cstLow);
	if (hB.pCtx == h.pCtx)
		Console::WriteLine(ConsoleColor::Cyan, _T("    [CO05-3] 같은 주소 재사용 확인: %p"), h.pCtx);

	CoContext* r = CoResumeH(h);
	if (r == nullptr && CoGetLastError() == coeStaleHandle)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO05-3] 예전 핸들 거부"));
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO05-3] 막지 못함"));

	while (CoResumeH(hB) != nullptr) {}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_Co05_ResumeSafety()
{
	Test_Co05_WrongThread();
	Test_Co05_Self();
	Test_Co05_StaleHandle();
	g_cCoMgr.Clear();
}
