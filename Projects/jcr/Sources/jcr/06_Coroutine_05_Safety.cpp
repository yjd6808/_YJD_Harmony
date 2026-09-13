#include "Core.h"
#include <thread>

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
	PrintSection05(_T("CO05-1: 다른 스레드에서 resume → coeStaleHandle"));

	g_ran05 = false;
	CoId id = CoRun(fn_Co05_Flag, {.spec_ = CoStackSpec::Low()});

	std::thread th([&]
	{
		if (!CoResume(id) && CoGetLastError() == coeStaleHandle)
			Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO05-1] 타 스레드 resume 거부"));
		else
			Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO05-1] 막지 못함"));
	});
	th.join();

	if (!g_ran05)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO05-1] 본문 미실행 확인"));
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO05-1] 본문이 실행됨"));

	while (CoResume(id)) {}
}

// 자기 자신을 resume → 거부된다.
static void fn_Co05_Self(CoContext*)
{
	if (!CoResume(CoCurrentCtx()->id_) && CoGetLastError() == coeInvalidState)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO05-2] 자기 resume 거부"));
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO05-2] 막지 못함"));
}

static void Test_Co05_Self()
{
	PrintSection05(_T("CO05-2: 자기 자신을 resume → coeInvalidState"));

	CoId id = CoRun(fn_Co05_Self, {.spec_ = CoStackSpec::Low()});
	while (CoResume(id)) {}
}

// 종료 후 같은 주소가 재사용돼도 예전 id는 죽은 id다.
static void fn_Co05_Once(CoContext*)
{
	CoYield();
}

static void Test_Co05_StaleHandle()
{
	PrintSection05(_T("CO05-3: 재사용된 주소 + 예전 id → coeStaleHandle"));

	g_cCoMgr.Clear();
	CoContext* pA = nullptr;
	CoId h = CoRun(fn_Co05_Once, {.spec_ = CoStackSpec::Low(), .ppOut_ = &pA});
	while (CoResume(h)) {}

	CoContext* pB = nullptr;
	CoId hB = CoRun(fn_Co05_Once, {.spec_ = CoStackSpec::Low(), .ppOut_ = &pB});
	if (pB == pA)
		Console::WriteLine(ConsoleColor::Cyan, _T("    [CO05-3] 같은 주소 재사용 확인: %p"), pB);

	if (!CoResume(h) && CoGetLastError() == coeStaleHandle)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO05-3] 예전 id 거부"));
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO05-3] 막지 못함"));

	while (CoResume(hB)) {}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_Co05_ResumeSafety()
{
	Test_Co05_WrongThread();
	Test_Co05_Self();
	Test_Co05_StaleHandle();
	g_cCoMgr.Clear();
}
