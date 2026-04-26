#include "Core.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Vectored Exception Handler - 스레드 스택 가드 페이지를 직접 확장하고 CONTINUE_EXECUTION 반환
// ※ 확인 결과: 페이지 가드를 직접 터치했는데도 VEH 핸들러 타지 않고 커밋됨. 더 윗단에서 이미 OS가 가드 페이지 위반을 처리해서 그런듯.
LONG CALLBACK PageGuardVectoredHandler(EXCEPTION_POINTERS* _pEp)
{
	if (_pEp->ExceptionRecord->ExceptionCode != STATUS_GUARD_PAGE_VIOLATION)
		return EXCEPTION_CONTINUE_SEARCH;


	void* pFaultAddr = (void*)_pEp->ExceptionRecord->ExceptionInformation[1];

	Console::WriteLine(ConsoleColor::Yellow,"[VEH] GUARD PAGE VIOLATION at %p",pFaultAddr);

	// OS에 위임하지 않고 직접 처리 완료 → 이후 가드 페이지도 VEH가 계속 호출됨
	return EXCEPTION_CONTINUE_SEARCH;
}

//////////////////////////////////////////////////////////////////////////////////////////
int call_05_PageGuard(int _argc, char** _argv)
{
	// VEH 핸들러 등록 (1 = 최우선 호출)
	void* pVeh = AddVectoredExceptionHandler(1, PageGuardVectoredHandler);

	char tmp[1];
	char* pCurrent = tmp;

	int    loopCount = 0;
	int	   guardPageStartIndex = -1;
	SIZE_T total     = 0;
	MEMORY_BASIC_INFORMATION mbi{};
	vector<MEMORY_BASIC_INFORMATION> guardPageInfos;

	// 현재 스택 포인터에서 하위 방향으로 페이지 순회
	while (true)
	{
		if (!VirtualQuery(pCurrent, &mbi, sizeof(mbi)))
			break;

		WinApi::PrintMemoryBasicInformation(mbi, loopCount);

		if ((mbi.Protect & PAGE_GUARD) != 0)
		{
			if (guardPageStartIndex == -1)
				guardPageStartIndex = loopCount;
			guardPageInfos.push_back(mbi);
		}

		if (mbi.State != MEM_COMMIT)
			break;

		total    += 0x1000;
		pCurrent -= 0x1000;
		++loopCount;
		mbi = {};
	}

	Console::WriteLine(ConsoleColor::White,
		"Total committed walked: %llu bytes, Guard pages found: %llu",
		total,
		guardPageInfos.size());

	// 루프 탈출 시점의 비커밋 페이지 출력 (하이라이트)
	if (VirtualQuery(pCurrent, &mbi, sizeof(mbi)))
	{
		Console::SetColor(ConsoleColor::LightCyan);
		WinApi::PrintMemoryBasicInformation(mbi, loopCount);
		Console::RestoreColor();
	}

	if (guardPageInfos.empty())
	{
		Console::WriteLine(ConsoleColor::Red, "No guard pages found. Aborting.");
		RemoveVectoredExceptionHandler(pVeh);
		return -1;
	}

	Console::ReadKeyWhile("Press spacebar to touch guard page...", ConsoleKey::Spacebar);

	// 가드 페이지 터치
	*(char*)guardPageInfos[0].BaseAddress = 0;

	// 터치 후 가드 페이지였던 페이지들의 변화된 상태 출력
	Console::WriteLine(ConsoleColor::White, "\n-- After Touch: former guard pages --");
	for (int i = 0; i < (int)guardPageInfos.size(); ++i)
	{
		mbi = {};
		if (VirtualQuery(guardPageInfos[i].BaseAddress, &mbi, sizeof(mbi)))
		{
			Console::SetColor(ConsoleColor::LightGreen);
			WinApi::PrintMemoryBasicInformation(mbi, guardPageStartIndex + i);
			Console::RestoreColor();
		}
	}

	// 루프 탈출 시점의 페이지 재조회 - OS가 새 가드를 설정한 위치 확인
	mbi = {};
	if (VirtualQuery(pCurrent, &mbi, sizeof(mbi)))
	{
		Console::SetColor(ConsoleColor::LightCyan);
		WinApi::PrintMemoryBasicInformation(mbi, loopCount);
		Console::RestoreColor();
	}

	// VEH 핸들러 해제
	RemoveVectoredExceptionHandler(pVeh);

	Console::ReadKeyWhile("Press spacebar to exit...", ConsoleKey::Spacebar);

	return 0;
}
