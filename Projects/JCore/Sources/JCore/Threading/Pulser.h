/*
 * 작성자: 윤정도
 * 생성일: 3/29/2023 6:49:15 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Time.h>
#include <JCore/Pattern/NonCopyableh.h>

NS_JC_BEGIN

struct PulserStatistics
{
	PulserStatistics()
		: SleepIntervalLast(0)
		, TotalSleepTime(0)
		, PulseCount(0)
	{}

	Int32U SleepIntervalLast;		// 마지막으로 실제로 Sleep한 시간
	Int32U TotalSleepTime;			// 얼마나 잤는지 (누계)
	Int32U PulseCount;				// 몇번 Sleep에서 깨어났는지 0으로 돌아가도록 unsigned로둠

	void Reset() {
		PulseCount = 0;
		SleepIntervalLast = 0;
		TotalSleepTime = 0;
	}
};



class Pulser
{
	struct WaitorAbstract
	{
		WaitorAbstract(Pulser& pulser) : m_Pulser(pulser) {}
		virtual ~WaitorAbstract() = default;
		virtual TimeSpan Wait() = 0;
	protected:
		Pulser& m_Pulser;
	};
public:
	using TWatch = StopWatch<StopWatchMode::System>;

	// 인터벌 = 깨어나는 주기
	// 워크타임 = 이전 Wait 호출 후 현재 Wait 호출까지 걸린시간
	// 펄스타임 = 인터벌 - 워크타임

	enum Mode
	{
		// 워크타임이 인터벌보다 길어져버린 경우 곧바로 깨어남
		// 인터벌: 500ms
		// 워크타임: 600ms
		// 슬립타임: 0ms (워크타임이 인터벌보다 크므로 Sleep을 수행하지 않음)
		// 장점: 슬립타임 0이 자주 발생하는 경우 렉이 자주 걸릴 수 있으므로 올바른 인터벌 설정이 중요함.
		eSliceCycle,

		// 워크타임이 인터벌보다 길어져버린 경우 인터벌 주기에 맞춰서 깨어남
		// 인터벌: 500ms
		// 워크타임: 600ms
		// 슬립타임: 400ms (인터벌 * 2 - 워크타임)
		// 장점: 슬립타임이 거의 항상 존재하므로 쾌적함.
		eExactCycle		
	};

	Pulser(Int32U intervalMiliseconds, Mode mode = eSliceCycle, JCORE_IN PulserStatistics* statistics = nullptr);
	~Pulser();

	void Start();
	TimeSpan Wait();

	Int32U Interval;
	TWatch Watch;
	PulserStatistics* Statistics;
private:
	struct SliceWaitor final : WaitorAbstract
	{
		SliceWaitor(Pulser& pulser) : WaitorAbstract(pulser) {}
		TimeSpan Wait() override;
	};

	struct ExactWaitor final : WaitorAbstract
	{
		ExactWaitor(Pulser& pulser) : WaitorAbstract(pulser) {}
		TimeSpan Wait() override;
	};

	WaitorAbstract* m_pWaitor;
};




NS_JC_END