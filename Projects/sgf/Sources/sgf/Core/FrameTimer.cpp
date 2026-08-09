/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:36:00 PM
 * 수정일: 8/9/2026 1:00:00 AM (v2: jc::TimeSpan 기반으로 개선)
 * =====================
 * 고해상도 프레임 타이머 구현부
 *
 * [QueryPerformanceCounter란?]
 *  윈도우가 제공하는 가장 정밀한 시계다. "카운터"라는 정수가 초당
 *  m_Frequency번 증가한다. 두 시점의 카운터 차이를 주파수로 나누면
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
	: m_Frequency(1)
	, m_LastCount(0)
	, m_TotalMicro(0)
	, m_Delta(0)
	, m_Total(0)
	, m_DeltaTime(0.0f)
	, m_TotalTime(0.0f)
	, m_FpsTimer(0.0f)
	, m_FpsFrameCount(0)
	, m_Fps(0.0f)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	m_Frequency = frequency.QuadPart;
	Reset();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 타이머를 초기화한다. 게임 루프 진입 직전에 호출.
void FrameTimer::Reset()
{
	m_LastCount = QueryCounter();
	m_TotalMicro = 0;
	m_Delta = jc::TimeSpan(0);
	m_Total = jc::TimeSpan(0);
	m_DeltaTime = 0.0f;
	m_TotalTime = 0.0f;
	m_FpsTimer = 0.0f;
	m_FpsFrameCount = 0;
	m_Fps = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 매 프레임 경과 시간 계산
void FrameTimer::Tick()
{
	const _s64 currentCount = QueryCounter();
	_s64 elapsedCount = currentCount - m_LastCount;
	m_LastCount = currentCount;
	if (elapsedCount < 0)
	{
		elapsedCount = 0;	// 절전 모드 복귀 등 극단 상황 방어
	}

	// 카운트 -> 마이크로초 변환. (jc::TimeSpan의 틱 1개 = 1마이크로초)
	// 곱셈을 먼저 하면 정밀도 손실 없이 변환된다. (_s64 범위는 충분하다)
	_s64 deltaMicro = (elapsedCount * 1000000LL) / m_Frequency;

	// 중단점/창 이동 등으로 생긴 비정상적으로 큰 dt는 250ms로 자른다.
	// 이렇게 하지 않으면 "위치 += 속도 * dt"에서 물체가 순간이동한다.
	const _s64 kMaxDeltaMicro_v = 250000;
	if (deltaMicro > kMaxDeltaMicro_v)
	{
		deltaMicro = kMaxDeltaMicro_v;
	}

	m_TotalMicro += deltaMicro;
	m_Delta = jc::TimeSpan(deltaMicro);
	m_Total = jc::TimeSpan(m_TotalMicro);

	// 하위 호환용 float 캐시 (기존 튜토리얼이 사용)
	m_DeltaTime = static_cast<_f32>(static_cast<double>(deltaMicro) / 1000000.0);
	m_TotalTime = static_cast<_f32>(static_cast<double>(m_TotalMicro) / 1000000.0);

	// FPS 집계: 1초 동안 그린 프레임 수를 센다.
	m_FpsTimer += m_DeltaTime;
	m_FpsFrameCount += 1;
	if (m_FpsTimer >= 1.0f)
	{
		m_Fps = static_cast<_f32>(m_FpsFrameCount) / m_FpsTimer;
		m_FpsTimer = 0.0f;
		m_FpsFrameCount = 0;
	}
}

NS_SGF_END
