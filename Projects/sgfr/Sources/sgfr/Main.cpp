/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:00:00 AM
 * 수정일: 8/9/2026 1:00:00 AM (콘솔 IO를 jc::Console로 교체)
 * 수정일: 8/10/2026 (2단계 메뉴 — 메인 메뉴에서 Tutorial 진입 후 목차 출력)
 * 수정일: 8/16/2026 (파일 로그 검증 — 명령행 인자로 튜토리얼 직접 실행 + 로거 초기화)
 * =====================
 * sgfr 튜토리얼 런처 (콘솔 목차)
 *
 * 메인 메뉴에서 1(Tutorial)을 선택하면 튜토리얼 목차가 출력된다.
 * 번호를 입력하면 해당 튜토리얼을 실행하고, 0을 입력하면 메인 메뉴로 돌아온다.
 * 메인 메뉴에서 0을 입력하면 종료.
 *
 * printf/scanf 대신 jc 라이브러리의 jc::Console을 사용한다.
 * 파일 로그 검증: 명령행 인자로 튜토리얼 번호를 넘기면 메뉴 없이 바로 실행한다.
 * - sgfr.exe 29 → 29번 튜토리얼(SceneGraph) 직접 실행 후 종료
 * - sgfr.exe → 기존 콘솔 메뉴
 * 실행 전 InitializeDefaultLogger(콘솔) + InitializeFileLogger(파일)로
 * 튜토리얼 동작 전 과정을 logs/ 폴더에 기록해 검증한다.
 */

#include "Core.h"
#include "sgfr/TutorialRegistry.h"
#include "sgfr/Practice/PracticeRegistry.h"

namespace
{
//////////////////////////////////////////////////////////////////////////////////////////
	// 명령행 인자에서 튜토리얼 번호를 읽는다.
	// - argv[1]이 1~TutorialCount() 범위의 정수면 유효 번호 반환 (0부터 시작하는 인덱스)
	// - 그 외(없음/비정수/범위 밖)는 -1 반환
	_s32 ReadTutorialArg(_s32 _argc, _s8** _argv)
	{
		if (_argc < 2)
		{
			return -1;
		}

		_s32 number = -1;
		if (sscanf_s(_argv[1], "%d", &number) != 1)
		{
			return -1;
		}

		if (number < 1 || number > sgfr::TutorialCount())
		{
			jc::Console::WriteLine("잘못된 튜토리얼 번호입니다: %d (범위: 1~%d)", number, sgfr::TutorialCount());
			return -1;
		}
		return number - 1;	// 0부터 시작하는 인덱스로 변환
	}

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

		const _s32 count = sgfr::TutorialCount();
		for (_s32 i = 0; i < count; ++i)
		{
			jc::Console::WriteLine("  %2d. %s", i + 1, sgfr::TutorialAt(i).name_);
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

		const _s32 count = sgfr::PracticeCount();
		for (_s32 i = 0; i < count; ++i)
		{
			jc::Console::WriteLine("  %2d. %s", i + 1, sgfr::PracticeAt(i).name_);
		}

		jc::Console::WriteLine("   0. 메인 메뉴로");
		jc::Console::WriteLine("==========================================");
	}

//////////////////////////////////////////////////////////////////////////////////////////
	// 번호 입력 (잘못된 입력이면 -1 반환)
	_s32 ReadSelection()
	{
		_s8 szLine[64];
		jc::Console::ReadLineBuffered("번호 입력: ", szLine, sizeof(szLine));

		_s32 selection = -1;
		if (sscanf_s(szLine, "%d", &selection) != 1)
		{
			return -1;
		}
		return selection;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 main(_s32 _argc, _s8** _argv)
{
	// 한글 출력이 깨지지 않도록 콘솔 코드페이지를 UTF-8로 설정한다.
	jc::InitializeJCore(_argc, _argv);

	// 파일 로그 검증: 콘솔 + 파일 로거 동시 초기화
	// - 콘솔: 화면에서 바로 확인 (실행 중 보기)
	// - 파일: logs/ 폴더에 타임스탬프 로그 파일 생성 (사후 검증)
	InitializeDefaultLogger("[sgfr] ");
	InitializeFileLogger("logs");
	_LogInfo_("sgfr 런처 시작 (argc=%d, argv[0]=%s)", _argc, _argv[0]);

	// 명령행 인자로 특정 튜토리얼 직접 실행
	const _s32 directIndex = ReadTutorialArg(_argc, _argv);
	if (directIndex >= 0)
	{
		const sgfr::TutorialEntry& entry = sgfr::TutorialAt(directIndex);
		jc::Console::WriteLine("");
		jc::Console::WriteLine("=== [%d] %s 직접 실행 ===", directIndex + 1, entry.name_);
		_LogInfo_("[Tutorial] 시작: %d. %s", directIndex + 1, entry.name_);

		const jc::DateTime tBegin = jc::DateTime::Now();
		try
		{
			entry.fn_();
		}
		catch (const std::exception& _ex)
		{
			_LogError_("[Tutorial] 예외 발생: %s", _ex.what());
		}
		catch (...)
		{
			_LogError_("[Tutorial] 알 수 없는 예외 발생");
		}
		const jc::DateTime tEnd = jc::DateTime::Now();

		_LogInfo_("[Tutorial] 종료: %d. %s (경과 %d ms)", directIndex + 1, entry.name_,
			(_s32)tEnd.Diff(tBegin).GetTotalMiliSecondsInt32());
		jc::Console::WriteLine("=== [%d] %s 종료 ===", directIndex + 1, entry.name_);

		FinalizeDefaultLogger();
		jc::FinalizeJCore();
		return 0;
	}

	// 즉시 실행: 1로 바꾸면 메뉴 없이 마지막 Practice를 바로 실행한다. (기본 0 = 메뉴 실행)
	if (0)
	{
		const sgfr::TutorialEntry& entry = sgfr::PracticeAt(sgfr::PracticeCount() - 1);
		entry.fn_();
		FinalizeDefaultLogger();
		jc::FinalizeJCore();
		return 0;
	}

	for (;;)
	{
		PrintMainMenu();

		const _s32 mainSelection = ReadSelection();
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
		const _s32 practice = (mainSelection == 2) ? 1 : 0;

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

			const _s32 selection = ReadSelection();
			if (selection == 0)
			{
				break;
			}

			const _s32 count = practice ? sgfr::PracticeCount() : sgfr::TutorialCount();
			if (selection < 1 || selection > count)
			{
				jc::Console::WriteLine("잘못된 번호입니다. 다시 입력해주세요.");
				continue;
			}

			const sgfr::TutorialEntry& entry = practice ? sgfr::PracticeAt(selection - 1) : sgfr::TutorialAt(selection - 1);
			jc::Console::WriteLine("");
			jc::Console::WriteLine("=== [%d] %s 시작 ===", selection, entry.name_);
			_LogInfo_("[Tutorial] 시작: %d. %s", selection, entry.name_);

			const jc::DateTime tBegin = jc::DateTime::Now();
			entry.fn_();
			const jc::DateTime tEnd = jc::DateTime::Now();

			_LogInfo_("[Tutorial] 종료: %d. %s (경과 %d ms)", selection, entry.name_,
				(_s32)tEnd.Diff(tBegin).GetTotalMiliSecondsInt32());
			jc::Console::WriteLine("=== [%d] %s 종료 ===", selection, entry.name_);
		}
	}

	_LogInfo_("sgfr 런처 종료");
	FinalizeDefaultLogger();
	jc::FinalizeJCore();
	return 0;
}