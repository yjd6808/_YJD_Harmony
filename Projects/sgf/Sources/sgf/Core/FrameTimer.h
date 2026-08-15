/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:36:00 PM
 * 수정일: 8/9/2026 1:00:00 AM (v2: jc::TimeSpan 기반으로 개선)
 * =====================
 * 고해상도 프레임 타이머
 *
 * [왜 시간을 재야 하나?]
 *  컴퓨터마다 성능이 달라 1초에 그리는 프레임 수가 다르다.
 *  이동/회전 같은 값에 "프레임당 고정값"을 더하면 빠른 PC에서 더 빨리 움직인다.
 *  매 프레임 경과한 시간(DeltaTime)을 곱해줘야 한다.
 *  위치 += 속도 * DeltaTime;   <- 이 패턴이 게임 프로그래밍의 기본이다.
 *
 * [v2에서 바뀐 점]
 *  - 시간을 float 초가 아닌 jc::TimeSpan(마이크로초 단위 틱)으로 관리한다.
 *    float은 누적 시간이 커질수록 정밀도가 떨어지지만 정수 틱은 그렇지 않다.
 *  - Delta()/Total()이 jc::TimeSpan을 돌려주고,
 *    기존 튜토리얼 호환용으로 DeltaTime()/TotalTime() float 버전도 유지한다.
 *  - 디버거 중단 등으로 생긴 비정상적으로 큰 dt는 250ms로 잘라낸다.
 */

#pragma once

#include "jc/Time.h"

NS_SGF_BEGIN

class FrameTimer
{
public:
	FrameTimer();

	// 타이머를 초기화한다. 게임 루프 진입 직전에 호출.
	void Reset();

	// 매 프레임 1회 호출. 직전 Tick으로부터의 경과 시간을 계산한다.
	void Tick();

	// === v2 API: jc::TimeSpan (Scene/Application의 OnUpdate로 전달) ===

	// 직전 프레임과의 시간 간격
	const jc::TimeSpan& Delta() const { return delta_; }

	// Reset 이후 누적 시간
	const jc::TimeSpan& Total() const { return total_; }

	// === 하위 호환 API: 초 단위 float (기존 01~22장 튜토리얼이 사용) ===

	// 직전 프레임과의 시간 간격(초)
	_f32 DeltaTime() const { return deltaTime_; }

	// 누적 시간(초)
	_f32 TotalTime() const { return totalTime_; }

	// 1초마다 갱신되는 초당 프레임 수
	_f32 Fps() const { return fps_; }

private:
	_s64 frequency_;		// QueryPerformanceFrequency 값 (초당 카운트 수)
	_s64 lastCount_;		// 직전 Tick 시점의 카운터 값
	_s64 totalMicro_;		// 누적 시간 (마이크로초)

	jc::TimeSpan delta_;	// 직전 프레임과의 시간 간격 (TimeSpan)
	jc::TimeSpan total_;	// 누적 시간 (TimeSpan)

	_f32 deltaTime_;		// 직전 프레임과의 시간 간격(초) - 하위 호환 캐시
	_f32 totalTime_;		// 누적 시간(초) - 하위 호환 캐시
	_f32 fpsTimer_;		// 1초를 재는 누적 타이머
	_s32  fpsFrameCount_;	// 1초 동안 그려진 프레임 수
	_f32 fps_;				// 확정된 FPS 값
};

NS_SGF_END
