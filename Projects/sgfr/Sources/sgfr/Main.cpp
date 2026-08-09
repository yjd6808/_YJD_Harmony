/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:00:00 AM
 * 수정일: 8/9/2026 1:00:00 AM (v2: 콘솔 IO를 jc::Console로 교체)
 * =====================
 * sgfr 튜토리얼 런처 (콘솔 목차)
 *
 * 콘솔에 목차를 출력하고, 번호를 입력받으면 해당 튜토리얼을 실행한다.
 * 튜토리얼 창을 닫으면 다시 목차로 돌아온다. 0을 입력하면 종료.
 *
 * [v2] printf/scanf 대신 jc 라이브러리의 jc::Console을 사용한다.
 */

#include "Core.h"
#include "sgfr/TutorialRegistry.h"

namespace
{
	//////////////////////////////////////////////////////////////////////////////////////////
	// 목차 출력
	void PrintMenu()
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

		jc::Console::WriteLine("   0. 종료");
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
int main()
{
	// 한글 출력이 깨지지 않도록 콘솔 코드페이지를 UTF-8로 설정한다.
	::SetConsoleOutputCP(CP_UTF8);

	// jc 콘솔 초기화 (jc 라이브러리의 Console IO 사용 준비)
	jc::Console::Init();

	for (;;)
	{
		PrintMenu();

		const int selection = ReadSelection();
		if (selection == 0)
		{
			jc::Console::WriteLine("종료합니다.");
			break;
		}

		if (selection < 1 || selection > sgfr::TutorialCount())
		{
			jc::Console::WriteLine("잘못된 번호입니다. 다시 입력해주세요.");
			continue;
		}

		const sgfr::TutorialEntry& entry = sgfr::TutorialAt(selection - 1);
		jc::Console::WriteLine("");
		jc::Console::WriteLine("=== [%d] %s 시작 ===", selection, entry.Name_);
		entry.Fn_();
		jc::Console::WriteLine("=== [%d] %s 종료 ===", selection, entry.Name_);
	}

	return 0;
}
