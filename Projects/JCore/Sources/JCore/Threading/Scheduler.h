/*
 * 작성자: 윤정도
 * 생성일: 6/13/2023 4:53:47 AM
 * =====================
 *
 * [필요한 기능 - 모두 구현완료]
 * 특정 시각기준으로 일정시간 간격으로 호출
 * 현재 시각기준으로 일정시간 간격으로 호출
 * 특정 시각기준으로 한번만 호출
 *
 */


#pragma once

#include <JCore/Time.h>
#include <JCore/Functional.h>
#include <JCore/Container/TreeMap.h>
#include <JCore/Threading/ThreadPool.h>

NS_JC_BEGIN

class SchedulerTask;
using SchedulerTaskCallback = Action<SchedulerTask*>;
class SchedulerTask
{
public:
	virtual ~SchedulerTask() = default;
	virtual void OnScheduled(SchedulerTask* task) {}
	virtual DateTime At() const = 0;
	virtual TimeSpan Interval() const = 0;
	virtual DateTime NextCallTime() const = 0;
	virtual bool CanNextCall() const = 0;
	virtual bool HasCallback() const { return false; }
	virtual void CallCallback() { return OnScheduled(this); }
};

template <typename TCallback>
constexpr bool IsSchedulerTaskCallback_v = IsCallable_v<TCallback> &&
IsSameType_v<CallableSignatureReturn_t<TCallback>, void> &&
IsSameType_v<CallableSignatureParam_t<TCallback>, Detail::ParameterPack<SchedulerTask*>>;

class SchedulerTaskOnce : public SchedulerTask
{
public:
	template <typename TCallback, DefaultEnableIf_t<IsSchedulerTaskCallback_v<TCallback>>>
	SchedulerTaskOnce(DateTime at, TCallback&& callback)
		: m_At(at)
		, m_Callback(Forward<TCallback>(callback))
	{}


	DateTime At() const override { return m_At; }
	TimeSpan Interval() const override { return 0; }
	DateTime NextCallTime() const override { return m_At; }
	bool CanNextCall() const override { return false; }
	void CallCallback() override { m_Callback(this); }
private:
	DateTime m_At;
	SchedulerTaskCallback m_Callback;
};

class SchedulerTaskRepeat : public SchedulerTask
{
public:
	template <typename TCallback>
	SchedulerTaskRepeat(DateTime at, TimeSpan interval, TCallback&& callback, Int32U repeat = INFINITE)
		: m_At(at)
		, m_Interval(interval)
		, m_Callback(Forward<TCallback>(callback))
		, m_uiCurRepeat(0)
		, m_uiMaxRepeat(repeat)
	{}

	DateTime At() const override { return m_At; }
	TimeSpan Interval() const override { return m_Interval; }
	DateTime NextCallTime() const override { return m_At + m_Interval; }
	bool CanNextCall() const override { return m_uiCurRepeat < m_uiMaxRepeat; }
	void CallCallback() override {
		m_At.Tick = DateTime::Now().Tick;
		m_uiCurRepeat += 1;
		m_Callback(this);
	}
private:
	DateTime m_At;
	TimeSpan m_Interval;
	SchedulerTaskCallback m_Callback;
	Int32U m_uiCurRepeat;
	Int32U m_uiMaxRepeat;
};


class Scheduler
{
	using TaskList = Vector<SchedulerTask*>;
public:
	enum class JoinStrategy
	{
		WaitAllTasks,			// 대기중인 작업이 완료될떄가지 기다림
		WaitOnlyRunningTask		// 실행중인 작업만 기다림
	};

	enum class State
	{
		Running,
		JoinWaitAllTasks,
		JoinWaitOnlyRunningTask,
		Joined
	};

	Scheduler(int threadCount);
	~Scheduler() noexcept;

	template <typename TCallback>
	static constexpr void ValidateCallbackType() { static_assert(IsSchedulerTaskCallback_v<RemoveQulifierReference_t<TCallback>>, "... TCallback is not SchedulerTaskCallBack type (std::function<void(SchedulerTask*)>)"); }

	template <typename TCallback>
	void IntervalAt(TCallback&& callback, DateTime at, TimeSpan interval, Int32U repeat = INFINITE) {
		ValidateCallbackType<TCallback>();

		AddTask(dbg_new SchedulerTaskRepeat{
			at,
			interval,
			Forward<TCallback>(callback),
			repeat
		});
	}

	template <typename TCallback>
	void Interval(TCallback&& callback, TimeSpan interval, Int32U repeat = INFINITE) {
		ValidateCallbackType<TCallback>();

		AddTask(dbg_new SchedulerTaskRepeat{
			DateTime::Now(),
			interval,
			Forward<TCallback>(callback),
			repeat
		});
	}

	template <typename TCallback>
	void OnceAt(TCallback&& callback, DateTime at) {
		ValidateCallbackType<TCallback>();

		AddTask(dbg_new SchedulerTaskOnce{
			at,
			Forward<TCallback>(callback)
		});
	}

	template <typename TCallback>
	void Once(TCallback&& callback) {
		ValidateCallbackType<TCallback>();

		AddTask(dbg_new SchedulerTaskOnce{
			DateTime::Now(),
			Forward<TCallback>(callback)
		});
	}

	void AddTask(SchedulerTask* task);
	int WaitingTaskCount();
	bool HasRunningTask() { return m_pRunningTask != nullptr; }
	void Join(JoinStrategy strategy = JoinStrategy::WaitOnlyRunningTask);
protected:
	void SchedulingRoutine();
	void ExecuteTasks(NormalLockGuard& guard, TaskList& scheduledTasks);
	int PopTasks(JCORE_OUT Vector<SchedulerTask*>& tasks);
	int WaitingTaskListCountRaw();
	void ClearWaitingTaskListRaw();

	TreeMap<Int64U, TaskList*> m_tmTasks;
	ConditionVariable m_CondVar;
	NormalLock m_Lock;
	ThreadPool* m_pThreadPool;
	Thread m_SchedulingThread;
	Atomic<SchedulerTask*> m_pRunningTask;
	State m_eState;
};


NS_JC_END
