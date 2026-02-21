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

#include <jc/Container/HashMap.h>
#include <jc/Container/TreeMap.h>

#include <jc/Threading/ThreadPool.h>
#include <jc/Threading/SchedulerTask.h>

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

	Scheduler(int _threadCount);
	~Scheduler() noexcept;

	template <typename TCallback>
	static constexpr void ValidateCallbackType() { static_assert(IsSchedulerTaskCallback_v<RemoveConstReference_t<TCallback>>, "... TCallback is not SchedulerTaskCallBack type (std::function<void(SchedulerTask*)>)"); }

	template <typename TCallback>
	void IntervalAt(TCallback&& _callback, DateTime _at, TimeSpan _interval, Int32U _repeat = INFINITE) {
		ValidateCallbackType<TCallback>();

		AddFirstTask(dbg_new SchedulerTaskRepeat{
			_at,
			_interval,
			Forward<TCallback>(_callback),
			_repeat
		});
	}

	template <typename TCallback>
	void Interval(TCallback&& _callback, TimeSpan _interval, Int32U _repeat = INFINITE) {
		ValidateCallbackType<TCallback>();

		AddFirstTask(dbg_new SchedulerTaskRepeat{
			DateTime::Now(),
			_interval,
			Forward<TCallback>(_callback),
			_repeat
		});
	}

	template <typename TCallback>
	void OnceAt(TCallback&& _callback, DateTime _at) {
		ValidateCallbackType<TCallback>();

		AddFirstTask(dbg_new SchedulerTaskOnce{
			_at,
			Forward<TCallback>(_callback)
		});
	}

	template <typename TCallback>
	void Once(TCallback&& _callback) {
		ValidateCallbackType<TCallback>();

		AddFirstTask(dbg_new SchedulerTaskOnce{
			DateTime::Now(),
			Forward<TCallback>(_callback)
		});
	}

	void AddFirstTask(SchedulerTask* _pTask);
	void AddTask(SchedulerTask* _pTask);
	int WaitingTaskCount();
	bool HasRunningTask() { return runningTaskCount_ > 0; }
	void Join(JoinStrategy _strategy = JoinStrategy::WaitOnlyRunningTask);
protected:
	void AddTaskRaw(SchedulerTask* _pTask);
	bool HaveEarlierTask(const DateTime& _waitUntil);
	void SchedulingRoutine();
	void ExecuteTasks(TaskList& _scheduledTasks, const Int64U* _pExecutableTaskLimitTime);
	int PopTasks(OUT Vector<SchedulerTask*>& _executableTasks, const Int64U* _pExecutableTaskLimitTime);
	int WaitingTaskListCountRaw();
	bool HaveExecutableTaskRaw(IN_OUT Int64U* _pExecutableTaskLimitTime = nullptr);
	void ClearWaitingTaskListRaw();

	static ThreadPool::JoinStrategy ConverToThreadPoolStrategy(JoinStrategy _strategy);

	TreeMap<Int64U, TaskList*> waitTasksMap_;
	ConditionVariable condVar_;
	NormalLock lock_;
	ThreadPool* pThreadPool_;
	Thread schedulingThread_;
	Atomic<int> runningTaskCount_ = 0;
	HashMap<SchedulerTask*, SchedulerTask*> scheduledTaskMap_;			// 대기 작업맵에서 Pop되어 스케쥴링 진행중인 작업들 목록;
	State state_;
};


NS_END
