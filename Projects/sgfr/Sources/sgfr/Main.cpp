/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:00:00 AM
 * 수정일: 8/9/2026 1:00:00 AM (v2: 콘솔 IO를 jc::Console로 교체)
 * 수정일: 8/10/2026 (v4: 2단계 메뉴 — 메인 메뉴에서 Tutorial 진입 후 목차 출력)
 * =====================
 * sgfr 튜토리얼 런처 (콘솔 목차)
 *
 * 메인 메뉴에서 1(Tutorial)을 선택하면 튜토리얼 목차가 출력된다.
 * 번호를 입력하면 해당 튜토리얼을 실행하고, 0을 입력하면 메인 메뉴로 돌아온다.
 * 메인 메뉴에서 0을 입력하면 종료.
 *
 * [v2] printf/scanf 대신 jc 라이브러리의 jc::Console을 사용한다.
 */

#include "Core.h"
#include "sgfr/TutorialRegistry.h"
#include "sgfr/Practice/PracticeRegistry.h"

namespace
{
	//////////////////////////////////////////////////////////////////////////////////////////
	// 메인 메뉴 출력
	void PrintMainMenu()
	{
		jc::Console::WriteLine("");
		jc::Console::WriteLine("==========================================");
		jc::Console::WriteLine(" sgf 튜토리얼 (메인 메뉴)");
		jc::Console::WriteLine("==========================================");
		jc::Console::WriteLine("   1. Tutorial");
		jc::Console::WriteLine("   2. Practice");
		jc::Console::WriteLine("   0. 종료");
		jc::Console::WriteLine("==========================================");
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// 튜토리얼 목차 출력
	void PrintTutorialMenu()
	{
		jc::Console::WriteLine("");
		jc::Console::WriteLine("==========================================");
		jc::Console::WriteLine(" sgf 튜토리얼 목차");
		jc::Console::WriteLine("==========================================");

		const int count = sgfr::TutorialCount();
		for (int i = 0; i < count; ++i)
		{
			jc::Console::WriteLine("  %2d. %s", i + 1, sgfr::TutorialAt(i).Name_);
		}

		jc::Console::WriteLine("   0. 메인 메뉴로");
		jc::Console::WriteLine("==========================================");
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Practice 목차 출력
	void PrintPracticeMenu()
	{
		jc::Console::WriteLine("");
		jc::Console::WriteLine("==========================================");
		jc::Console::WriteLine(" sgf 튜토리얼 - Practice (연습)");
		jc::Console::WriteLine("==========================================");

		const int count = sgfr::PracticeCount();
		for (int i = 0; i < count; ++i)
		{
			jc::Console::WriteLine("  %2d. %s", i + 1, sgfr::PracticeAt(i).Name_);
		}

		jc::Console::WriteLine("   0. 메인 메뉴로");
		jc::Console::WriteLine("==========================================");
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// 번호 입력 (잘못된 입력이면 -1 반환)
	int ReadSelection()
	{
		char szLine[64];
		jc::Console::ReadLineBuffered("번호 입력: ", szLine, sizeof(szLine));

		int selection = -1;
		if (sscanf_s(szLine, "%d", &selection) != 1)
		{
			return -1;
		}
		return selection;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
int main(int _argc, char** _argv)
{
	// 한글 출력이 깨지지 않도록 콘솔 코드페이지를 UTF-8로 설정한다.
	jc::InitializeJCore(_argc, _argv);

	// 즉시 실행 (메뉴로 실행되도록 하고 싶으면 0으로 바꿔서 빌드할 것)
	if (0) 
	{
		sgfr::PracticeAt(sgfr::PracticeCount() - 1);
		jc::FinalizeJCore();
		return 0;
	}

	for (;;)
	{
		PrintMainMenu();

		const int mainSelection = ReadSelection();
		if (mainSelection == 0)
		{
			jc::Console::WriteLine("종료합니다.");
			break;
		}

		if (mainSelection != 1 && mainSelection != 2)
		{
			jc::Console::WriteLine("잘못된 번호입니다. 다시 입력해주세요.");
			continue;
		}

		// Practice 메뉴 (2번)
		const int practice = (mainSelection == 2) ? 1 : 0;

		for (;;)
		{
			if (practice)
			{
				PrintPracticeMenu();
			}
			else
			{
				PrintTutorialMenu();
			}

			const int selection = ReadSelection();
			if (selection == 0)
			{
				break;
			}

			const int count = practice ? sgfr::PracticeCount() : sgfr::TutorialCount();
			if (selection < 1 || selection > count)
			{
				jc::Console::WriteLine("잘못된 번호입니다. 다시 입력해주세요.");
				continue;
			}

			const sgfr::TutorialEntry& entry = practice ? sgfr::PracticeAt(selection - 1) : sgfr::TutorialAt(selection - 1);
			jc::Console::WriteLine("");
			jc::Console::WriteLine("=== [%d] %s 시작 ===", selection, entry.Name_);
			entry.Fn_();
			jc::Console::WriteLine("=== [%d] %s 종료 ===", selection, entry.Name_);
		}
	}

	jc::FinalizeJCore();
	return 0;
}
