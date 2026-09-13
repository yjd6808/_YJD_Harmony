#include "Core.h"

// 예외 전파 방어 예제.
// - fn 안에서 던진 예외는 CoEntry가 코루틴 스택에서 잡아 두고
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

static void On_Co01_Exception(CoId, std::exception_ptr _ex)
{
	try
	{
		if (_ex)
			std::rethrow_exception(_ex);
	}
	catch (const std::runtime_error& e)
	{
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO01-1] 받음: %s"), e.what());
	}
}

static void Test_Co01_OnException()
{
	PrintSection01(_T("CO01-1: onException → 스케줄러 스택에서 받음"));

	CoId id = CoRun(fn_Co01_Throw, {.onException_ = On_Co01_Exception});
	if (id == CO_INVALID_ID)
		Console::WriteLine(ConsoleColor::Cyan, _T("    [CO01-1] 종료 후 무효 id"));
}

static void Test_Co01_Unchecked()
{
	PrintSection01(_T("CO01-2: CoRun(콜백 없음) → 죽지 않고 끝나며 원인이 남음"));

	CoId id = CoRun(fn_Co01_Throw);
	if (id == CO_INVALID_ID && CoGetLastError() == coeException)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO01-2] 종료 + %s"), CoErrorString(coeException));
	else
		Console::WriteLine(ConsoleColor::Red, _T("  FAIL [CO01-2] id=%llu"), (unsigned long long)id);
	try
	{
		CoTakePendingException();
	}
	catch (const std::runtime_error& e)
	{
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [CO01-2] 보관분 수령: %s"), e.what());
	}
	CoClearPendingException();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_Co01_Exception()
{
	Test_Co01_OnException();
	Test_Co01_Unchecked();
	g_cCoMgr.Clear();
}
