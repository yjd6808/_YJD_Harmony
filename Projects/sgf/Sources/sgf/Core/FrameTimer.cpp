/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:36:00 PM
 * 수정일: 8/9/2026 1:00:00 AM (v2: jc::TimeSpan 기반으로 개선)
 * =====================
 * 고해상도 프레임 타이머 구현부
 *
 * [QueryPerformanceCounter란?]
 *  윈도우가 제공하는 가장 정밀한 시계다. "카운터"라는 정수가 초당
 *  frequency_번 증가한다. 두 시점의 카운터 차이를 주파수로 나누면
 *  경과 시간이 나온다. (마이크로초 이하 정밀도)
 */

#include "Core.h"
#include "sgf/Core/FrameTimer.h"

NS_SGF_BEGIN

// 현재 카운터 값을 읽는 내부 헬퍼
static _s64 QueryCounter()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 생성자: 주파수를 읽어두고 타이머를 초기화한다.
// 주파수는 부팅 이후 변하지 않으므로 한 번만 읽으면 된다.
FrameTimer::FrameTimer()
	: frequency_(1)
	, lastCount_(0)
	, totalMicro_(0)
	, delta_(0)
	, total_(0)
	, deltaTime_(0.0f)
	, totalTime_(0.0f)
	, fpsTimer_(0.0f)
	, fpsFrameCount_(0)
	, fps_(0.0f)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	frequency_ = frequency.QuadPart;
	Reset();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 타이머를 초기화한다. 게임 루프 진입 직전에 호출.
void FrameTimer::Reset()
{
	lastCount_ = QueryCounter();
	totalMicro_ = 0;
	delta_ = jc::TimeSpan(0);
	total_ = jc::TimeSpan(0);
	deltaTime_ = 0.0f;
	totalTime_ = 0.0f;
	fpsTimer_ = 0.0f;
	fpsFrameCount_ = 0;
	fps_ = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 매 프레임 경과 시간 계산
void FrameTimer::Tick()
{
	const _s64 currentCount = QueryCounter();
	_s64 elapsedCount = currentCount - lastCount_;
	lastCount_ = currentCount;
	if (elapsedCount < 0)
	{
		elapsedCount = 0;	// 절전 모드 복귀 등 극단 상황 방어
	}

	// 카운트 -> 마이크로초 변환. (jc::TimeSpan의 틱 1개 = 1마이크로초)
	// 곱셈을 먼저 하면 정밀도 손실 없이 변환된다. (_s64 범위는 충분하다)
	_s64 deltaMicro = (elapsedCount * 1000000LL) / frequency_;

	// 중단점/창 이동 등으로 생긴 비정상적으로 큰 dt는 250ms로 자른다.
	// 이렇게 하지 않으면 "위치 += 속도 * dt"에서 물체가 순간이동한다.
	const _s64 MAX_DELTA_MICRO = 250000;
	if (deltaMicro > MAX_DELTA_MICRO)
	{
		deltaMicro = MAX_DELTA_MICRO;
	}

	totalMicro_ += deltaMicro;
	delta_ = jc::TimeSpan(deltaMicro);
	total_ = jc::TimeSpan(totalMicro_);

	// 하위 호환용 float 캐시 (기존 튜토리얼이 사용)
	deltaTime_ = static_cast<_f32>(static_cast<_f64>(deltaMicro) / 1000000.0);
	totalTime_ = static_cast<_f32>(static_cast<_f64>(totalMicro_) / 1000000.0);

	// FPS 집계: 1초 동안 그린 프레임 수를 센다.
	fpsTimer_ += deltaTime_;
	fpsFrameCount_ += 1;
	if (fpsTimer_ >= 1.0f)
	{
		fps_ = static_cast<_f32>(fpsFrameCount_) / fpsTimer_;
		fpsTimer_ = 0.0f;
		fpsFrameCount_ = 0;
	}
}

NS_SGF_END
