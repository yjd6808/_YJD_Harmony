/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:30:00 AM
 * =====================
 * 03. 디바이스와 스왑체인 - 보조 함수 구현부
 */

#include "Core.h"
#include "sgfr/03_DeviceAndSwapChain/03_DeviceAndSwapChain_Function.h"

using namespace sgf;
using namespace jc;

// 시간(초)을 받아 무지개처럼 부드럽게 순환하는 색상을 만든다.
//
// [원리]
//  sin 함수는 -1 ~ +1 사이를 부드럽게 오가는 파도다.
//  "* 0.5 + 0.5"를 해주면 0 ~ 1 범위가 되어 색상 값으로 쓸 수 있다.
//  R/G/B에 서로 다른 위상(0, 120도, 240도)을 더하면 색상이 무지개처럼 도는다.
Color RainbowColor(_f32 _timeSec)
{
	const _f32 r = std::sin(_timeSec) * 0.5f + 0.5f;
	const _f32 g = std::sin(_timeSec + jc_math_deg2rad(120.0f)) * 0.5f + 0.5f;
	const _f32 b = std::sin(_timeSec + jc_math_deg2rad(240.0f)) * 0.5f + 0.5f;
	return Color(r, g, b, 1.0f);
}
