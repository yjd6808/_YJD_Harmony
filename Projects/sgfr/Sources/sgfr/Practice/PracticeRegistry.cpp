/*
 * 작성자: 윤정도
 * 생성일: 8/10/2026
 * =====================
 * 연습(Practice) 튜토리얼 목록 정의
 * 이 파일의 배열 순서가 곧 연습 목차 번호다. (1번부터 순서대로 학습 권장)
 */

#include "Core.h"
#include "sgfr/Practice/PracticeRegistry.h"

#include "sgfr/Practice/01_2DSceneTemplate/01_2DSceneTemplate_Main.h"
#include "sgfr/Practice/02_3DSceneTemplate/02_3DSceneTemplate_Main.h"
#include "sgfr/Practice/03_2DScenePractice/03_2DScenePractice_Main.h"
#include "sgfr/Practice/04_3DScenePractice/04_3DScenePractice_Main.h"
#include "sgfr/Practice/05_2DPipelineTemplate/05_2DPipelineTemplate_Main.h"
#include "sgfr/Practice/06_3DPipelineTemplate/06_3DPipelineTemplate_Main.h"
#include "sgfr/Practice/07_2DPipelinePractice/07_2DPipelinePractice_Main.h"
#include "sgfr/Practice/08_3DPipelinePractice/08_3DPipelinePractice_Main.h"

 // 연습용 튜토리얼 목록 (번호 순서 = 학습 순서)
namespace sgfr
{

	static const TutorialEntry s_Practices[] =
	{
		{ _T("2D 씬 템플릿 (Practice) - 기반 코드"), Practice_2DSceneTemplate_Main },
		{ _T("3D 씬 템플릿 (Practice) - 기반 코드"), Practice_3DSceneTemplate_Main },
		{ _T("2D 씬 연습 (Practice)"), Practice_2DScenePractice_Main },
		{ _T("3D 씬 연습 (Practice)"), Practice_3DScenePractice_Main },
		{ _T("2D 렌더링 파이프라인 템플릿 (Practice) - 기반 코드"), Practice_2DPipelineTemplate_Main },
		{ _T("3D 렌더링 파이프라인 템플릿 (Practice) - 기반 코드"), Practice_3DPipelineTemplate_Main },
		{ _T("2D 렌더링 파이프라인 연습 (Practice)"), Practice_2DPipelinePractice_Main },
		{ _T("3D 렌더링 파이프라인 연습 (Practice)"), Practice_3DPipelinePractice_Main },
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// : 등록된 연습 튜토리얼 개수를 반환한다.
	//////////////////////////////////////////////////////////////////////////////////////////
	_s32 PracticeCount()
	{
		return (_s32)(sizeof(s_Practices) / sizeof(s_Practices[0]));
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// : _index(0부터 시작)번째 연습 튜토리얼 정보를 반환한다.
	//////////////////////////////////////////////////////////////////////////////////////////
	const TutorialEntry& PracticeAt(_s32 _index)
	{
		jc_assert(_index >= 0 && _index < PracticeCount());
		return s_Practices[_index];
	}

} // namespace sgfr