#include "Core.h"

// [코루틴-04] 스택 오버플로우 EmergencyPages 예제.
// - 이 변경점이 없으면: 오버플로우 가드를 치는 순간 SEH 디스패치가 돌 공간이
//   없어 이중 폴트로 프로세스가 강제 종료됐다. (__except에 닿지 않음)
// - 예약 아래 비상 패드가 배달 공간을 보장하고, 오버플로우 가드를 찍으면
//   STATUS_STACK_OVERFLOW로 확정해 잡을 수 있게 한다.
// - 한계: __chkstk/RTC-fill처럼 RSP가 따라 내려가는 깊은 하강은 커널이 삼켜
//   잡지 못할 수 있다. 코루틴 한 장은 적당한 깊이로 쓰는 것이 안전하다.
static void PrintSection04(const char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		"\n================================================================");
	Console::WriteLine(ConsoleColor::Yellow, "  %s", _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		"================================================================");
}

static int Filter_SO(unsigned _code)
{
	return _code == EXCEPTION_STACK_OVERFLOW ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
}

// 확립된 프레임에서 오버플로우 가드를 직접 찍는다. (RSP는 위에 그대로)
__declspec(noinline) static void Touch_OverflowGuard(volatile char* _p)
{
	*_p = 42;
}

static void fn_Co04_Overflow(CoContext* pCtx)
{
	__try
	{
		volatile char* pGuard = (volatile char*)pCtx->stack_.pStackEnd_;
		Touch_OverflowGuard(pGuard);
		Console::WriteLine(ConsoleColor::Red, "  FAIL [CO04] 가드를 찍었는데 살아있음");
	}
	__except (Filter_SO(GetExceptionCode()))
	{
		Console::WriteLine(ConsoleColor::Green, "  PASS [CO04] 오버플로우를 __except로 받음");
		CoNoteStackOverflow();
		if (CoCurrentCtx()->stack_.overflowed_)
			Console::WriteLine(ConsoleColor::Green, "  PASS [CO04] overflowed_ 표시됨");
		else
			Console::WriteLine(ConsoleColor::Red, "  FAIL [CO04] overflowed_ 미표시");
		if (CoResetStackOverflow())
			Console::WriteLine(ConsoleColor::Green, "  PASS [CO04] 가드존 재설치됨");
		else
			Console::WriteLine(ConsoleColor::Red, "  FAIL [CO04] 가드존 재설치 실패");
	}
}

static void Test_Co04_Overflow()
{
	PrintSection04("CO04: 오버플로우 SEH 도달 + 표시 + 복구");

	CoContext* pCtx = CoRun(fn_Co04_Overflow, cstMid);
	while (pCtx)
		pCtx = CoResume(pCtx);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_Co04_Emergency()
{
	Test_Co04_Overflow();
	g_cCoMgr.Clear();
}
