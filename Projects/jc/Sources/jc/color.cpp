/*
	작성자 : 윤정도
	설명 : color 정적 상수 멤버 정의 (color.inl 단일 TU 포함)
	사용자정의 2026-08-16 : color static constexpr 선언은 MSVC에서 클래스 내부 초기화가 강제되어
		값을 color.inl로 분리할 수 없음. static const 선언 + 단일 TU에서 정의하는 고전 패턴 사용.
*/

#include "jc/Math.h"

NS_JC_BEGIN

#include "color.inl"

NS_END