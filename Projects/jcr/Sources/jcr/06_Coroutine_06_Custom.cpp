#include "Core.h"

// [코루틴-06] cstCustom 크기 정규화 + 풀 오염 방지 예제.
// - 이 변경점이 없으면: 5000B 요청이 Low 티어인 척 5000B 스택을 받아 정렬이 깨지고,
//   종료 후 Low 풀에 들어가 다음 사용자가 좁은 스택을 받아 엉뚱한 시점에 오버플로우난다.
static void PrintSection06(const char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		"\n================================================================");
	Console::WriteLine(ConsoleColor::Yellow, "  %s", _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		"================================================================");
}

static void fn_Co06_YieldOnce(CoContext*)
{
	CoYield();
}

// 작은 custom은 티어로 올림되고 크기도 티어 크기가 된다.
static void Test_Co06_RoundUp()
{
	PrintSection06("CO06-1: custom 5000B → Low 16KB로 올림");

	CoContext* pCtx = CoRun(fn_Co06_YieldOnce, cstCustom, 5000);
	if (pCtx != nullptr
		&& pCtx->stack_.stackTier_ == cstLow
		&& pCtx->stack_.size_ == CO_STACK_SIZE_LOW
		&& ((uintptr_t)pCtx->stack_.pStackBase_ % CO_PAGE_SIZE) == 0)
		Console::WriteLine(ConsoleColor::Green, "  PASS [CO06-1] tier=Low size=16KB 페이지 정렬");
	else
		Console::WriteLine(ConsoleColor::Red, "  FAIL [CO06-1] 올림 실패");

	while (pCtx)
		pCtx = CoResume(pCtx);
}

// 크기가 0인 custom은 만들 수 없다.
static void Test_Co06_Zero()
{
	PrintSection06("CO06-2: custom 크기 0 → coeInvalidStackSize");

	CoContext* pCtx = CoRun(fn_Co06_YieldOnce, cstCustom, 0);
	if (pCtx == nullptr && CoGetLastError() == coeInvalidStackSize)
		Console::WriteLine(ConsoleColor::Green, "  PASS [CO06-2] %s", CoErrorString(coeInvalidStackSize));
	else
		Console::WriteLine(ConsoleColor::Red, "  FAIL [CO06-2] 거부되지 않음");
}

// custom이 풀을 오염시키지 않는다. (6KB 터치: 5000B에는 안 들어가고 진짜 Low에는 들어감)
static void fn_Co06_Touch6K(CoContext*)
{
	volatile char buf[6 * 1024];
	for (int i = 0; i < (int)sizeof(buf); i += 4096)
		buf[i] = (char)i;
}

static void Test_Co06_NoPoolMix()
{
	PrintSection06("CO06-3: custom 종료 후 Low 재사용해도 6KB 정상");

	CoContext* pCtx = CoRun(fn_Co06_YieldOnce, cstCustom, 5000);
	while (pCtx)
		pCtx = CoResume(pCtx);

	pCtx = CoRun(fn_Co06_Touch6K, cstLow);
	if (pCtx == nullptr && CoGetLastError() == coeNone)
		Console::WriteLine(ConsoleColor::Green, "  PASS [CO06-3] 풀 오염 없음");
	else
		Console::WriteLine(ConsoleColor::Red, "  FAIL [CO06-3] 오염됨");
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_Co06_CustomTier()
{
	Test_Co06_RoundUp();
	Test_Co06_Zero();
	Test_Co06_NoPoolMix();
	g_cCoMgr.Clear();
}
