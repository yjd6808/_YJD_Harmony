/*
 * 작성자: 윤정도
 * 생성일: 3/29/2023 6:49:15 PM
 * =====================
 *
 */


#pragma once

#include <jc/Time.h>
#include <jc/Pattern/NonCopyableh.h>

NS_JC_BEGIN
struct PulserStatistics
{
	PulserStatistics()
	: sleepIntervalLast_(0)
	, totalSleepTime_(0)
	, pulseCount_(0)
	{
	}

	_u32 sleepIntervalLast_; // 마지막으로 실제로 Sleep한 시간
	_u32 totalSleepTime_; // 얼마나 잤는지 (누계)
	_u32 pulseCount_; // 몇번 Sleep에서 깨어났는지 0으로 돌아가도록 unsigned로둠

	void Reset()
	{
		pulseCount_ = 0;
		sleepIntervalLast_ = 0;
		totalSleepTime_ = 0;
	}
};


class Pulser
{
	struct WaitorAbstract
	{
		WaitorAbstract(Pulser& _pulser)
		: pulser_(_pulser)
		{
		}

		virtual ~WaitorAbstract() = default;
		virtual TimeSpan Wait() = 0;

	protected:
		Pulser& pulser_;
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

	Pulser(_u32 _intervalMiliseconds, Mode _mode = eSliceCycle,  PulserStatistics* _pStatistics = nullptr);
	~Pulser();

	void Start();
	TimeSpan Wait();

	_u32 interval_;
	TWatch watch_;
	PulserStatistics* pStatistics_;

private:
	struct SliceWaitor final : WaitorAbstract
	{
		SliceWaitor(Pulser& _pulser)
		: WaitorAbstract(_pulser)
		{
		}

		TimeSpan Wait() override;
	};

	struct ExactWaitor final : WaitorAbstract
	{
		ExactWaitor(Pulser& _pulser)
		: WaitorAbstract(_pulser)
		{
		}

		TimeSpan Wait() override;
	};

	WaitorAbstract* pWaitor_;
};


NS_END
