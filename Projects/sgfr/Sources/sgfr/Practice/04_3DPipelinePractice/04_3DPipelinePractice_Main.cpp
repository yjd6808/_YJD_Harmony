/*
 * 작성자: 윤정도
 * 생성일: 9/25/2026
 * =====================
 * 04. 3D 렌더링 파이프라인 연습 (Practice) - 서브메뉴 진입점
 *
 * PracticeRegistry와 같은 등록 방식을 서브메뉴에 적용했다.
 * 새 항목을 추가하려면 s_SubMenus 배열에 한 줄만 추가하면 된다.
 * 새 필터 없이 함수명으로만 분리하고 extern으로 가져와 등록한다.
 */

#include "Core.h"
#include "sgfr/Practice/04_3DPipelinePractice/04_3DPipelinePractice_Main.h"

extern void CPUTransformedVertex3D_Main();
extern void GPUTransformedVertex3D_Main();

namespace
{
	////////////////////////////////////////////////////////////////////////////////////////
	// : 서브메뉴 항목 (이름 + 실행 함수)
	////////////////////////////////////////////////////////////////////////////////////////
	struct SubMenuEntry
	{
		const _char* name_;
		void (*fn_)();
	};

	// 서브메뉴 목록 (배열 순서 = 메뉴 번호 순서)
	static const SubMenuEntry s_SubMenus[] =
	{
		{ _T("CPU 정점 굽기"), CPUTransformedVertex3D_Main },
		{ _T("GPU 정점 굽기"), GPUTransformedVertex3D_Main },
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// : 등록된 서브메뉴 개수를 반환한다.
	////////////////////////////////////////////////////////////////////////////////////////
	_s32 SubMenuCount()
	{
		return (_s32)(sizeof(s_SubMenus) / sizeof(s_SubMenus[0]));
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// : 번호 입력 (잘못된 입력이면 -1 반환)
	////////////////////////////////////////////////////////////////////////////////////////
	_s32 ReadSelection()
	{
		_char szLine[64];
		jc::Console::ReadLineBuffered(_T("번호 입력: "), szLine, sizeof(szLine) / sizeof(_char));

		_s32 selection = -1;
		if (_stscanf_s(szLine, _T("%d"), &selection) != 1)
		{
			return -1;
		}
		return selection;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// : 서브메뉴 출력
	////////////////////////////////////////////////////////////////////////////////////////
	void PrintSubMenu()
	{
		jc::Console::WriteLine(_T(""));
		jc::Console::WriteLine(_T("=========================================="));
		jc::Console::WriteLine(_T(" Practice 04. 3D 렌더링 파이프라인 연습"));
		jc::Console::WriteLine(_T("=========================================="));

		const _s32 count = SubMenuCount();
		for (_s32 i = 0; i < count; ++i)
		{
			jc::Console::WriteLine(_T("  %2d. %s"), i + 1, s_SubMenus[i].name_);
		}

		jc::Console::WriteLine(_T("   0. 뒤로가기"));
		jc::Console::WriteLine(_T("=========================================="));
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// : 서브메뉴를 실행한다. 0을 입력하면 Practice 목차로 돌아간다.
////////////////////////////////////////////////////////////////////////////////////////
void Practice_3DPipelinePractice_Main()
{
	for (;;)
	{
		PrintSubMenu();

		const _s32 selection = ReadSelection();
		if (selection == 0)
		{
			break;
		}
		if (selection >= 1 && selection <= SubMenuCount())
		{
			s_SubMenus[selection - 1].fn_();
		}
		else
		{
			jc::Console::WriteLine(_T("잘못된 번호입니다. 다시 입력해주세요."));
		}
	}
}
