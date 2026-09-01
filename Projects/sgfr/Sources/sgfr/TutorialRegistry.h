/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:00:00 AM
 * =====================
 * 튜토리얼 목록 관리자
 *
 * [설계 의도]
 * - 각 튜토리얼은 "이름 + 실행 함수 포인터" 한 쌍으로 등록된다.
 * - Main.cpp는 이 목록을 콘솔에 출력하고, 번호를 입력받아 해당 함수를 호출한다.
 * - 새 튜토리얼을 추가하려면 TutorialRegistry.cpp의 배열에 한 줄만 추가하면 된다.
 */

#pragma once

namespace sgfr
{

// 튜토리얼 실행 함수 타입 (인자 없음, 반환 없음)
using TutorialFn = void(*)();

// 튜토리얼 한 개의 정보
struct TutorialEntry
{
	const char* name_;			// 콘솔 목차에 표시될 이름
	TutorialFn fn_;				// 실행할 함수
};

// 등록된 튜토리얼 개수를 반환한다.
_s32 TutorialCount();

// _index(0부터 시작)번째 튜토리얼 정보를 반환한다.
const TutorialEntry& TutorialAt(_s32 _index);

}