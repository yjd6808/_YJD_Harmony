/*
 * 작성자: 윤정도
 * 생성일: 8/10/2026
 * =====================
 * 연습(Practice) 튜토리얼 목록 정의
 * 이 파일의 배열 순서가 곧 연습 목차 번호다. (1번부터 순서대로 학습 권장)
 */

#include "Core.h"
#include "sgfr/Practice/PracticeRegistry.h"

#include "sgfr/Practice/01_RenderingPipeline/01_RenderingPipeline_Main.h"

// 연습용 튜토리얼 목록 (번호 순서 = 학습 순서)
namespace sgfr
{

static const TutorialEntry s_Practices[] =
{
	{ "렌더링 파이프라인 (Practice) - 첫 조합 연습", Practice_RenderingPipeline_Main },
};

// 등록된 연습 튜토리얼 개수를 반환한다.
int PracticeCount()
{
	return (int)(sizeof(s_Practices) / sizeof(s_Practices[0]));
}

// _index(0부터 시작)번째 연습 튜토리얼 정보를 반환한다.
const TutorialEntry& PracticeAt(int _index)
{
	jc_assert(_index >= 0 && _index < PracticeCount());
	return s_Practices[_index];
}

} // namespace sgfr