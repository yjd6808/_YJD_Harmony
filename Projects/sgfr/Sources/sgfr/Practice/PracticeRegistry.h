/*
 * 작성자: 윤정도
 * 생성일: 8/10/2026
 * =====================
 * 연습(Practice) 튜토리얼 목록 관리자
 *
 * [설계 의도]
 *  - Tutorial과 달리 연습은 자유롭게 늘어나므로 별도 레지스트리로 분리했다.
 *  - 등록 방식은 TutorialRegistry와 동일하다. (이름 + 실행 함수 포인터)
 *  - 새 연습을 추가하려면 PracticeRegistry.cpp의 배열에 한 줄만 추가하면 된다.
 */

#pragma once

#include "sgfr/TutorialRegistry.h"

namespace sgfr
{

// 등록된 연습 튜토리얼 개수를 반환한다.
int PracticeCount();

// _index(0부터 시작)번째 연습 튜토리얼 정보를 반환한다.
const TutorialEntry& PracticeAt(int _index);

}