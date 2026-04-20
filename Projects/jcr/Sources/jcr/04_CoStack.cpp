// created by AI.
#include "Core.h"

//////////////////////////////////////////////////////////////////////////////////////////
// 관리되는 스택 구조체
struct ManagedStack
{
	char*  base;
	size_t reserveSize;
	size_t pageSize;
};

// 관리되는 스택 목록
const int MAX_MANAGED_STACKS = 10;
ManagedStack gManagedStacks[MAX_MANAGED_STACKS] = {};
int gStackCount = 0;

// VEH 카운터 — 테스트 모드일 때만 카운팅
bool gVehCountMode = false;  // true 이면 카운팅 활성화
int  gVehCallCount = 0;      // 실제 VEH 호출 횟수

//////////////////////////////////////////////////////////////////////////////////////////
// 스택 등록
void RegisterStack(void* _pBase, size_t _reserveSize, size_t _pageSize)
{
	if (gStackCount >= MAX_MANAGED_STACKS)
	{
		_LogError_("Max managed stacks exceeded");
		return;
	}

	gManagedStacks[gStackCount].base        = (char*)_pBase;
	gManagedStacks[gStackCount].reserveSize = _reserveSize;
	gManagedStacks[gStackCount].pageSize    = _pageSize;
	gStackCount++;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 스택 등록 해제
void UnregisterStack(void* _pBase)
{
	for (int i = 0; i < gStackCount; ++i)
	{
		if (gManagedStacks[i].base == _pBase)
		{
			gManagedStacks[i] = gManagedStacks[gStackCount - 1];
			gStackCount--;
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 주소로 스택 찾기
ManagedStack* FindStack(void* _pAddr)
{
	char* pPtr = (char*)_pAddr;
	for (int i = 0; i < gStackCount; ++i)
	{
		if (pPtr >= gManagedStacks[i].base &&
		    pPtr <  gManagedStacks[i].base + gManagedStacks[i].reserveSize)
			return &gManagedStacks[i];
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Vectored Exception Handler - 가드 페이지 위반 시 스택 자동 확장
LONG CALLBACK VectoredHandler(EXCEPTION_POINTERS* _pEp)
{
	if (_pEp->ExceptionRecord->ExceptionCode != STATUS_GUARD_PAGE_VIOLATION)
		return EXCEPTION_CONTINUE_SEARCH;

	void* pFaultAddr = (void*)_pEp->ExceptionRecord->ExceptionInformation[1];

	// 우리가 관리하는 스택인지 검사
	ManagedStack* pStack = FindStack(pFaultAddr);
	if (!pStack)
		return EXCEPTION_CONTINUE_SEARCH;

	// 테스트 모드일 때 호출 횟수 카운팅
	if (gVehCountMode)
		++gVehCallCount;

	size_t pageSize = pStack->pageSize;

	// fault 난 페이지의 시작 주소 (페이지 정렬)
	char* pPage = (char*)((uintptr_t)pFaultAddr & ~(pageSize - 1));

	// 1. 해당 페이지를 일반 COMMIT으로 변경 (GUARD 효과 명시적 해제)
	DWORD old;
	VirtualProtect(pPage, pageSize, PAGE_READWRITE, &old);

	// 2. 다음 페이지 확장
	char* pNext = pPage + pageSize;
	if (pNext >= pStack->base && pNext < pStack->base + pStack->reserveSize)
	{
		// 다음 페이지 COMMIT
		VirtualAlloc(pNext, pageSize, MEM_COMMIT, PAGE_READWRITE);

		// 그 다음 페이지를 새 GUARD로 설정 (COMMIT 먼저 수행)
		char* pNext2 = pNext + pageSize;
		if (pNext2 < pStack->base + pStack->reserveSize)
		{
			VirtualAlloc(pNext2, pageSize, MEM_COMMIT, PAGE_READWRITE);
			VirtualProtect(pNext2, pageSize, PAGE_READWRITE | PAGE_GUARD, &old);
		}
	}

	return EXCEPTION_CONTINUE_EXECUTION;
}

//////////////////////////////////////////////////////////////////////////////////////////
size_t GetPageSize()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwPageSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
void* StackAlloc(size_t _reserveSize, size_t _pageSize)
{
	char* pBase = (char*)VirtualAlloc(nullptr, _reserveSize, MEM_RESERVE, PAGE_READWRITE);
	if (pBase == nullptr)
	{
		DWORD error = GetLastError();
		_LogError_("VirtualAlloc failed. Error code: %lu", error);
		return nullptr;
	}

	// page0: COMMIT + RW,  page1: COMMIT + GUARD
	char* pValloc = (char*)VirtualAlloc(pBase, _pageSize * 2, MEM_COMMIT, PAGE_READWRITE);
	if (pValloc == nullptr)
	{
		DWORD error = GetLastError();
		_LogError_("VirtualAlloc failed. Error code: %lu", error);
		VirtualFree(pBase, 0, MEM_RELEASE);
		return nullptr;
	}

	DWORD old = 0;
	BOOL protResult = VirtualProtect(pBase + _pageSize, _pageSize, PAGE_READWRITE | PAGE_GUARD, &old);
	if (!protResult)
	{
		DWORD error = GetLastError();
		_LogError_("VirtualProtect failed. Error code: %lu", error);
		VirtualFree(pBase, 0, MEM_RELEASE);
		return nullptr;
	}

	return pBase;
}

//////////////////////////////////////////////////////////////////////////////////////////
void StackInit(void* _pBase, size_t _reserveSize, size_t _pageSize)
{
	// 전체 decommit 후 초기 상태로 재구성
	VirtualFree(_pBase, _reserveSize, MEM_DECOMMIT);

	// page0: COMMIT + RW,  page1: COMMIT + GUARD
	char* pValloc = (char*)VirtualAlloc(_pBase, _pageSize * 2, MEM_COMMIT, PAGE_READWRITE);
	if (pValloc == nullptr)
	{
		DWORD error = GetLastError();
		_LogError_("VirtualAlloc failed. Error code: %lu", error);
		return;
	}

	DWORD old = 0;
	BOOL protResult = VirtualProtect((char*)_pBase + _pageSize, _pageSize, PAGE_READWRITE | PAGE_GUARD, &old);
	if (!protResult)
	{
		DWORD error = GetLastError();
		_LogError_("VirtualProtect failed. Error code: %lu", error);
		VirtualFree(_pBase, 0, MEM_RELEASE);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void StackFree(void* _pBase)
{
	VirtualFree(_pBase, 0, MEM_RELEASE);
}

//////////////////////////////////////////////////////////////////////////////////////////
void DumpPages(void* _pBase, size_t _reserveSize, size_t _pageSize, const char* _pTitle)
{
	Console::WriteLine(ConsoleColor::Green, "\n==== %s ====", _pTitle);

	size_t pageCount = _reserveSize / _pageSize;

	for (size_t i = 0; i < pageCount; ++i)
	{
		char* pAddr = (char*)_pBase + i * _pageSize;

		MEMORY_BASIC_INFORMATION mbi;
		VirtualQuery(pAddr, &mbi, sizeof(mbi));

		const char* pStateStr = "";
		if      (mbi.State == MEM_COMMIT)  pStateStr = "COMMIT";
		else if (mbi.State == MEM_RESERVE) pStateStr = "RESERVE";
		else if (mbi.State == MEM_FREE)    pStateStr = "FREE";

		bool isGuard = (mbi.Protect & PAGE_GUARD) != 0;

		if (mbi.State == MEM_FREE)
		{
			Console::WriteLine(ConsoleColor::Red,
				"[%02llu] %p : FREE", i, pAddr);
		}
		else if (isGuard)
		{
			Console::WriteLine(ConsoleColor::Yellow,
				"[%02llu] %p : %s + GUARD", i, pAddr, pStateStr);
		}
		else if (mbi.State == MEM_COMMIT)
		{
			Console::WriteLine(ConsoleColor::Cyan,
				"[%02llu] %p : COMMIT", i, pAddr);
		}
		else
		{
			Console::WriteLine(ConsoleColor::White,
				"[%02llu] %p : RESERVE", i, pAddr);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 1. 스택확장 테스트 - 기존 코드
//////////////////////////////////////////////////////////////////////////////////////////
static void Test_StackExpansion(int _argc, char** _argv)
{
	constexpr size_t RESERVE_SIZE = 64 * 1024;
	size_t pageSize = GetPageSize();

	Console::WriteLine(ConsoleColor::White, "Page Size: %llu bytes", pageSize);

	// VEH 핸들러 등록 (1 = 최우선 호출)
	void* pVeh = AddVectoredExceptionHandler(1, VectoredHandler);

	// 1. StackAlloc
	void* pStack = StackAlloc(RESERVE_SIZE, pageSize);
	RegisterStack(pStack, RESERVE_SIZE, pageSize);
	DumpPages(pStack, RESERVE_SIZE, pageSize, "After StackAlloc");
	Console::ReadKeyWhile("Press spacebar to continue...", ConsoleKey::Spacebar);

	// 2. TouchStack - VEH 핸들러가 GUARD 페이지 위반을 잡아 자동 확장
	((char*)pStack)[pageSize + 100] = 0;
	DumpPages(pStack, RESERVE_SIZE, pageSize, "After TouchStack");
	Console::ReadKeyWhile("Press spacebar to continue...", ConsoleKey::Spacebar);

	// 3. StackInit - 스택 초기 상태로 재설정
	StackInit(pStack, RESERVE_SIZE, pageSize);
	DumpPages(pStack, RESERVE_SIZE, pageSize, "After StackInit");
	Console::ReadKeyWhile("Press spacebar to continue...", ConsoleKey::Spacebar);

	// 4. TouchStack - 재설정 후 다시 자동 확장 확인
	((char*)pStack)[pageSize + 100] = 0;
	DumpPages(pStack, RESERVE_SIZE, pageSize, "After TouchStack");
	Console::ReadKeyWhile("Press spacebar to continue...", ConsoleKey::Spacebar);

	// 5. StackFree
	UnregisterStack(pStack);
	StackFree(pStack);
	DumpPages(pStack, RESERVE_SIZE, pageSize, "After StackFree");

	// VEH 핸들러 해제
	RemoveVectoredExceptionHandler(pVeh);

	Console::ReadKeyWhile("Press spacebar to exit...", ConsoleKey::Spacebar);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 2. 스택확장 성능 - VEH 핸들러 호출 비용 측정
// 동일 스택을 TRIAL_COUNT번 StackInit → GUARD 페이지 터치(VEH 트리거) 반복하여
// 총 소요 시간과 1회 평균 VEH 호출 비용을 출력한다.
//////////////////////////////////////////////////////////////////////////////////////////
static void Test_StackExpansionPerf(int _argc, char** _argv)
{
	constexpr size_t RESERVE_SIZE = 64 * 1024;
	constexpr int    TRIAL_COUNT  = 1000;

	size_t pageSize = GetPageSize();

	Console::WriteLine(ConsoleColor::White,
		"[성능 테스트] VEH 핸들러 %d회 트리거 비용 측정", TRIAL_COUNT);
	Console::WriteLine(ConsoleColor::White, "Page Size  : %llu bytes", pageSize);
	Console::WriteLine(ConsoleColor::White, "Trial Count: %d", TRIAL_COUNT);
	Console::WriteLine(ConsoleColor::White, "");

	// VEH 핸들러 등록
	void* pVeh = AddVectoredExceptionHandler(1, VectoredHandler);

	// 스택 할당 및 등록
	void* pStack = StackAlloc(RESERVE_SIZE, pageSize);
	if (pStack == nullptr)
	{
		Console::WriteLine(ConsoleColor::Red, "StackAlloc 실패");
		RemoveVectoredExceptionHandler(pVeh);
		return;
	}
	RegisterStack(pStack, RESERVE_SIZE, pageSize);

	// ─────────────────────────────────────────────────────────────
	// 측정 루프
	//   각 반복: StackInit → GUARD 페이지 터치(VEH 1회 호출) → 반복
	//
	// StackInit 비용 자체를 제외하기 위해 StackInit 시간도 따로 측정한 뒤
	// 순수 VEH 호출 비용(총시간 - StackInit 합산)을 계산한다.
	// ─────────────────────────────────────────────────────────────

	// (A) StackInit 전용 루프 - 기준 비용 측정
	StopWatchHR swA;
	swA.Start();
	for (int i = 0; i < TRIAL_COUNT; ++i)
	{
		StackInit(pStack, RESERVE_SIZE, pageSize);
	}
	TimeSpan tsA = swA.Stop();

	// (B) StackInit + VEH 트리거 루프 — 카운팅 모드 활성화
	gVehCountMode = true;
	gVehCallCount = 0;

	StopWatchHR swB;
	swB.Start();
	for (int i = 0; i < TRIAL_COUNT; ++i)
	{
		StackInit(pStack, RESERVE_SIZE, pageSize);

		// GUARD 페이지(page1) 을 명시적으로 터치 → VEH 1회 호출
		volatile char* pGuard = (volatile char*)pStack + pageSize + 100;
		*pGuard = 0;
	}
	TimeSpan tsB = swB.Stop();

	gVehCountMode = false;
	int vehCallCount = gVehCallCount;   // 루프 후 실제 호출 횟수 저장

	double initOnlyMs = tsA.GetTotalMiliSeconds();
	double totalMs    = tsB.GetTotalMiliSeconds();

	// ─────────────────────────────────────────────────────────────
	// 결과 계산
	// ─────────────────────────────────────────────────────────────
	double vehTotalMs  = totalMs - initOnlyMs;           // 순수 VEH 합산 시간
	double vehAvgUs    = vehTotalMs * 1000.0 / TRIAL_COUNT; // 1회 평균 (µs)
	double vehAvgNs    = vehAvgUs   * 1000.0;               // 1회 평균 (ns)

	bool countOk = (vehCallCount == TRIAL_COUNT);

	Console::WriteLine(ConsoleColor::LightCyan,  "──────────────────────────────────────");
	Console::WriteLine(ConsoleColor::White,       "StackInit %d회 합산       : %.3f ms", TRIAL_COUNT, initOnlyMs);
	Console::WriteLine(ConsoleColor::White,       "StackInit+VEH %d회 합산   : %.3f ms", TRIAL_COUNT, totalMs);
	Console::WriteLine(ConsoleColor::LightGreen,  "VEH 순수 합산             : %.3f ms", vehTotalMs);
	Console::WriteLine(ConsoleColor::LightGreen,  "VEH 1회 평균              : %.3f µs  (%.1f ns)", vehAvgUs, vehAvgNs);
	Console::WriteLine(countOk ? ConsoleColor::LightGreen : ConsoleColor::LightRed,
		"VEH 실제 호출 횟수        : %d / %d  (%s)",
		vehCallCount, TRIAL_COUNT, countOk ? "OK" : "MISMATCH");
	Console::WriteLine(ConsoleColor::LightCyan,   "──────────────────────────────────────");

	// 정리
	UnregisterStack(pStack);
	StackFree(pStack);
	RemoveVectoredExceptionHandler(pVeh);

	Console::ReadKeyWhile("Press spacebar to exit...", ConsoleKey::Spacebar);
}

//////////////////////////////////////////////////////////////////////////////////////////
int call_04_CoStack(int _argc, char** _argv)
{
	auto pOption = jc::MakeShared<ConsoleMenuItemOption>();
	pOption->inputTitle_               = "선택> ";
	pOption->inputLeftBrace_           = "[";
	pOption->inputRightBrace_          = "]";
	pOption->inputRightPadding_        = " ";
	pOption->inputFormatPadding_       = 1;
	pOption->pressAnyKeyAfterCallback_ = false;

	auto pMenu = jc::MakeShared<ConsoleMenuItem>();
	pMenu->AddHeader(
		"========================================\n"
		"      CoStack R&D - 코루틴용 스택 연구\n"
		"========================================");
	pMenu->Add("1", "스택확장 테스트",
		CONSOLE_MENU_ACTION(Test_StackExpansion(_argc, _argv))
	);
	pMenu->Add("2", "스택확장 성능 (VEH 1000회 호출 비용)",
		CONSOLE_MENU_ACTION(Test_StackExpansionPerf(_argc, _argv))
	);
	pMenu->AddBack("0", "뒤로가기");

	ConsoleMenuItem::Show(pMenu, pOption);

	return 0;
}
