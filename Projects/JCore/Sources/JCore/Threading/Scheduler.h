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

#include <JCore/Container/HashMap.h>
#include <JCore/Container/TreeMap.h>

#include <JCore/Threading/ThreadPool.h>
#include <JCore/Threading/SchedulerTask.h>

NS_JC_BEGIN

class Scheduler
{
	using TaskList = Vector<SchedulerTask*>;
public:
	enum class JoinStrategy
	{
		WaitAllTasks,			// 대기중인 작업이 완료될떄가지 기다림
		WaitOnlyRunningTask		// 실행중인 작업이 1회 정상완료 될떄까지만 기다림 (여러횟수 모두 기다리는게 아님)
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
	static constexpr void ValidateCallbackType() { static_assert(IsSchedulerTaskCallback_v<RemoveConstReference_t<TCallback>>, "... TCallback is not SchedulerTaskCallBack type (std::function<void(SchedulerTask*)>)"); }

	template <typename TCallback>
	void IntervalAt(TCallback&& callback, DateTime at, TimeSpan interval, Int32U repeat = INFINITE) {
		ValidateCallbackType<TCallback>();

		AddFirstTask(dbg_new SchedulerTaskRepeat{
			at,
			interval,
			Forward<TCallback>(callback),
			repeat
		});
	}

	template <typename TCallback>
	void Interval(TCallback&& callback, TimeSpan interval, Int32U repeat = INFINITE) {
		ValidateCallbackType<TCallback>();

		AddFirstTask(dbg_new SchedulerTaskRepeat{
			DateTime::Now(),
			interval,
			Forward<TCallback>(callback),
			repeat
		});
	}

	template <typename TCallback>
	void OnceAt(TCallback&& callback, DateTime at) {
		ValidateCallbackType<TCallback>();

		AddFirstTask(dbg_new SchedulerTaskOnce{
			at,
			Forward<TCallback>(callback)
		});
	}

	template <typename TCallback>
	void Once(TCallback&& callback) {
		ValidateCallbackType<TCallback>();

		AddFirstTask(dbg_new SchedulerTaskOnce{
			DateTime::Now(),
			Forward<TCallback>(callback)
		});
	}

	void AddFirstTask(SchedulerTask* task);
	void AddTask(SchedulerTask* task);
	int WaitingTaskCount();
	bool HasRunningTask() { return m_pRunningTask != nullptr; }
	void Join(JoinStrategy strategy = JoinStrategy::WaitOnlyRunningTask);
protected:
	void AddTaskRaw(SchedulerTask* task);
	bool HaveEarlierTask(const DateTime& waitUntil);
	void SchedulingRoutine();
	void ExecuteTasks(TaskList& scheduledTasks, const Int64U* executableTaskLimitTime);
	int PopTasks(JCORE_OUT Vector<SchedulerTask*>& executableTasks, const Int64U* executableTaskLimitTime);
	int WaitingTaskListCountRaw();
	bool HaveExecutableTaskRaw(JCORE_IN_OUT Int64U* executableTaskLimitTime = nullptr);
	void ClearWaitingTaskListRaw();

	static ThreadPool::JoinStrategy ConverToThreadPoolStrategy(JoinStrategy strategy);

	TreeMap<Int64U, TaskList*> m_tmWaitTasks;
	ConditionVariable m_CondVar;
	NormalLock m_Lock;
	ThreadPool* m_pThreadPool;
	Thread m_SchedulingThread;
	Atomic<SchedulerTask*> m_pRunningTask;
	HashMap<SchedulerTask*, SchedulerTask*> m_ScheduledTaskMap;			// 대기 작업맵에서 Pop되어 스케쥴링 진행중인 작업들 목록;
	State m_eState;
};


NS_JC_END
