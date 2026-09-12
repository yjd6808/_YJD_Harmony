#include "Core.h"

// [코루틴-14] API 편의성 예제. (userData / 값 채널 / 람다 / CoScoped)
// - 이 변경점이 없으면: 데이터를 넘기려면 전역 변수를 써야 했고,
//   결과를 돌려받을 방법이 없었으며, 끝나지 않은 코루틴은 풀로 안 돌아왔다.
static void PrintSection14(const _char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		_T("\n================================================================"));
	Console::WriteLine(ConsoleColor::Yellow, _T("  %s"), _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		_T("================================================================"));
}

static void fn_Co14_UserData(CoContext* pCtx)
{
	int* pAcc = (int*)pCtx->userData_;
	*pAcc += 1;
	CoYield();
	*pAcc += 10;
}

static void Test_Co14_UserData()
{
	PrintSection14(_T("CO14-1: userData 전달"));

	int acc = 0;
	CoContext* pCtx = CoRunU(fn_Co14_UserData, &acc, cstMid);
	while (pCtx)
		pCtx = CoResume(pCtx);
	if (acc == 11)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO14-1] acc=%d"), acc);
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO14-1] acc=%d"), acc);
}

static void fn_Co14_Echo(CoContext*)
{
	_u64 v = CoYield((_u64)100);
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO14-2] 받음: %llu"), v);
	CoYield((_u64)200);
}

static void Test_Co14_Transfer()
{
	PrintSection14(_T("CO14-2: 값 채널 왕복"));

	CoContext* pCtx = CoRun(fn_Co14_Echo, cstMid);
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO14-2] 첫 yield: %llu (기대 100)"),
		(unsigned long long)pCtx->transfer_);
	_u64 out = 0;
	pCtx = CoResumeV(pCtx, (_u64)111, &out);
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO14-2] 받음: %llu (기대 200)"), (unsigned long long)out);
	while (pCtx)
		pCtx = CoResume(pCtx);
	if (out == 200)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO14-2] 왕복 성공"));
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO14-2] 값 불일치"));
}

static void Test_Co14_Lambda()
{
	PrintSection14(_T("CO14-3: 람다 캡처"));

	int acc = 0;
	CoContext* pCtx = CoRunFn([&](CoContext*)
	{
		for (int i = 0; i < 3; ++i)
		{
			acc += i;
			CoYield();
		}
	}, cstMid);
	while (pCtx)
		pCtx = CoResume(pCtx);
	if (acc == 3)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO14-3] acc=%d"), acc);
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO14-3] acc=%d"), acc);
}

static void fn_Co14_Cancel(CoContext*)
{
	while (!CoCancelRequested())
		CoYield();
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO14-4] 취소 요청 확인 후 종료"));
}

static void Test_Co14_Scoped()
{
	PrintSection14(_T("CO14-4: CoScoped 협력적 취소"));

	{
		CoScoped sc(CoRunH(fn_Co14_Cancel, cstMid));
		sc.Resume();
		sc.Cancel();
		if (sc.Done())
			Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO14-4] 취소 후 종료"));
		else
			Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO14-4] 미종료"));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_Co14_Api()
{
	Test_Co14_UserData();
	Test_Co14_Transfer();
	Test_Co14_Lambda();
	Test_Co14_Scoped();
	g_cCoMgr.Clear();
}
