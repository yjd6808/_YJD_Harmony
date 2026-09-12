#include "Core.h"

// [코루틴-01] 예외 전파 방어 예제.
// - 이 변경점이 없으면: fn 안에서 던진 예외가 트램폴린을 넘어 언와인더를 꼬아
//   프로세스가 죽거나 이상 종료됐다. CoEntry가 코루틴 스택에서 잡아 두고
//   스케줄러 스택에서 다시 던진다.
static void PrintSection01(const _char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		_T("\n================================================================"));
	Console::WriteLine(ConsoleColor::Yellow, _T("  %s"), _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		_T("================================================================"));
}

static void fn_Co01_Throw(CoContext*)
{
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO01] fn 안에서 throw"));
	throw std::runtime_error("코루틴 예외");
}

static void Test_Co01_Checked()
{
	PrintSection01(_T("CO01-1: CoRunChecked → 스케줄러 스택에서 catch"));

	try
	{
		CoContext* pCtx = CoRunChecked(fn_Co01_Throw, cstMid);
		Console::WriteLine(ConsoleColor::Cyan, _T("    [CO01-1] CoRun 반환: %p (종료됐으므로 null)"), pCtx);
	}
	catch (const std::runtime_error& e)
	{
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO01-1] 받음: %s"), e.what());
	}
	CoClearPendingException();
}

static void Test_Co01_Unchecked()
{
	PrintSection01(_T("CO01-2: CoRun(미검사) → 죽지 않고 끝나며 원인이 남음"));

	CoContext* pCtx = CoRun(fn_Co01_Throw, cstMid);
	if (pCtx == nullptr && CoGetLastError() == coeException)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO01-2] 종료 + %s"), CoErrorString(coeException));
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO01-2] pCtx=%p"), pCtx);
	CoClearPendingException();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_Co01_Exception()
{
	Test_Co01_Checked();
	Test_Co01_Unchecked();
	g_cCoMgr.Clear();
}
