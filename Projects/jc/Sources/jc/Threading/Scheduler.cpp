/*
 * 작성자: 윤정도
 * 생성일: 6/13/2023 4:54:05 AM
 * =====================
 *
 */


#include <jc/Core.h>
#include <jc/Limit.h>
#include <jc/Threading/Scheduler.h>

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
Scheduler::Scheduler(int _threadCount)
	: pThreadPool_(dbg_new ThreadPool(_threadCount))
	, schedulingThread_(JC_CALLBACK_0(Scheduler::SchedulingRoutine, this))
	, state_(State::Running)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Scheduler::~Scheduler()
{
	// Ensure graceful shutdown if Join was not called explicitly.
	if (state_ != State::Joined)
	{
		Join(JoinStrategy::WaitAllTasks);
	}

	JC_DELETE_SAFE(pThreadPool_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::AddFirstTask(SchedulerTask* _pTask)
{
	{
		NormalLockGuard guard(lock_);
		if (state_ != State::Running)
		{
			jc_assert_msg(state_ == State::Running, "스케쥴러가 Running상태가 아닌데 Task삽입을 시도했습니다.");
			delete _pTask;
			return;
		}

		AddTaskRaw(_pTask);
	}
	condVar_.NotifyOne();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::AddTask(SchedulerTask* _pTask)
{
	{
		NormalLockGuard guard(lock_);
		AddTaskRaw(_pTask);
	}
	condVar_.NotifyOne();
}

//////////////////////////////////////////////////////////////////////////////////////////
int Scheduler::WaitingTaskCount()
{
	NormalLockGuard guard(lock_);
	return WaitingTaskListCountRaw();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::Join(JoinStrategy _strategy)
{
	{
		NormalLockGuard guard(lock_);
		state_ = _strategy == JoinStrategy::WaitOnlyRunningTask ? State::JoinWaitOnlyRunningTask : State::JoinWaitAllTasks;
	}

	condVar_.NotifyOne();
	schedulingThread_.Join();
	pThreadPool_->Join(ConverToThreadPoolStrategy(_strategy));

	{
		// NextCall이 있는 Task들의 경우 때문에 Notify이전에 ClearWaitingTaskListRaw()를 호출해서
		// 대기중인 작업들을 정리해서는 안된다.
		NormalLockGuard guard(lock_);
		ClearWaitingTaskListRaw();
		scheduledTaskMap_.Clear();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::AddTaskRaw(SchedulerTask* _pTask)
{
	Int64U atTick = _pTask->At().Tick;
	if (!waitTasksMap_.Exist(atTick))
	{
		waitTasksMap_.Insert(atTick, dbg_new TaskList{ _pTask });
	}
	else
	{
		waitTasksMap_[atTick]->PushBack(_pTask);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Scheduler::HaveEarlierTask(const DateTime& _waitUntil)
{
	TaskList* pPendingTasks;
	if (waitTasksMap_.TryGetFirstValue(pPendingTasks))
	{
		return pPendingTasks->At(0)->At() < _waitUntil;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::SchedulingRoutine() {
	TaskList* pvPendingTasks;                        // 아직 시간이 되지 않아 실행대기중인 작업
	TaskList vScheduledTasks;                        // 시간이 만료되어 실행되어야할 작업

	bool bExit = false;
	bool bHaveExecutableTasks = false;               // 실행가능한 작업이 있는 경우
	bool bHaveWaitTasks = false;
	DateTime dtWaitUntil = 0;
	Int64U uiExecutableTaskLimitTime = 0;
	
	auto fnWait = [&]()->bool {
		bExit = false;
		bHaveWaitTasks = waitTasksMap_.Size();
		bHaveExecutableTasks = HaveExecutableTaskRaw(&uiExecutableTaskLimitTime);
		const bool bWaitUntil = dtWaitUntil.Tick != 0; // 시그널이 올때까지 대기해야하는 상태(Wait)인지 아니면 특정 시각까지 대기해야하는 상태인지(WaitUntil)

		if (state_ == State::JoinWaitAllTasks) {
			bExit = !bHaveWaitTasks && !bHaveExecutableTasks && scheduledTaskMap_.Size() <= 0; // 대기 작업과 스케쥴링 중인 작업이 없을 경우
		} else if (state_ == State::JoinWaitOnlyRunningTask) {
			bExit = true;
		}

		

		if (!bWaitUntil) {
			// Wait 상태인 경우 작업 유무를 확인하여 WaitUntil로 재전환 될 수 있도록 한다.
			return bHaveWaitTasks || bExit || bHaveExecutableTasks;
		}

		const bool bHaveEarlierTask = HaveEarlierTask(dtWaitUntil); //현재 WaitUntil 까지 대기중인 시간보다 더빠른 작업이 있는 경우 더 빠른 시각으로 WaitUntil 재적용
		return bHaveEarlierTask || bExit || bHaveExecutableTasks;
	};

	
	NormalLockGuard guard(lock_);

	for (;;) {
		const bool bHasInitTasks = waitTasksMap_.TryGetFirstValue(pvPendingTasks);
		if (bHasInitTasks) {
			dtWaitUntil = pvPendingTasks->At(0)->At();
			condVar_.WaitUntil(guard, dtWaitUntil, Move(fnWait));
		} else {
			dtWaitUntil.Tick = 0;
			condVar_.Wait(guard, Move(fnWait));
		}

		if (bExit) {
			break;
		}

		ExecuteTasks(vScheduledTasks, bHaveExecutableTasks ? &uiExecutableTaskLimitTime : nullptr);
		uiExecutableTaskLimitTime = 0;
	}

	state_ = State::Joined;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::ExecuteTasks(TaskList& _scheduledTasks, const Int64U* _pExecutableTaskLimitTime)
{
	const int taskCount = PopTasks(_scheduledTasks, _pExecutableTaskLimitTime);
	
	for (int index = 0; index < taskCount; ++index)
	{
		SchedulerTask* pTask = _scheduledTasks[index];
		scheduledTaskMap_.Insert(pTask, pTask);

		++runningTaskCount_;
		pThreadPool_->Run([pTask, this]
		{
			pTask->CallCallback();

			if (!pTask->CanNextCall())
			{
				{
					NormalLockGuard guard(lock_);
					scheduledTaskMap_.Remove(pTask);
				}

				delete pTask;
				--runningTaskCount_;
				condVar_.NotifyOne();
			}
			else
			{
				AddTask(pTask);
				--runningTaskCount_;
			}
		});
	}
}

/**
 * \brief 실행가능한 작업 목록을 얻는다.
 * \param executableTasks 실행가능한 작업들
 * \param executableTaskLimitTime 실행가능한 작업들이 있는지 여부 및 대기 작업 목록에서 만료 시간을 의미한다. (nullptr == 실행가능한 작업이 없음)
 * \return 실행가능한 작업 수
 */
//////////////////////////////////////////////////////////////////////////////////////////
int Scheduler::PopTasks(OUT Vector<SchedulerTask*>& _executableTasks, const Int64U* _pExecutableTaskLimitTime)
{
	_executableTasks.Clear();

	if (waitTasksMap_.Size() == 0 || _pExecutableTaskLimitTime == nullptr)
	{
		return 0;
	}

	// 현재 시각을 기준으로 시간이 만료된 작업들을 가져온다.
	auto iterator = waitTasksMap_.Begin();

	while (iterator->HasNext())
	{
		auto& currentValue = iterator->Current();

		const Int64U& expiredTaskKey = currentValue.key_;
		const TaskList* pExpiredTaskList = currentValue.value_;

		if (expiredTaskKey >= *_pExecutableTaskLimitTime)
		{
			break;
		}

		const bool removed = waitTasksMap_.RemoveByIterator(iterator);
		jc_assert(removed);

		for (int index = 0; index < pExpiredTaskList->Size(); ++index)
		{
			_executableTasks.PushBack(pExpiredTaskList->At(index));
		}

		delete pExpiredTaskList;
	}

	const int poppedSize = _executableTasks.Size();
	return poppedSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Scheduler::WaitingTaskListCountRaw()
{
	int count = 0;
	waitTasksMap_.ForEachValue([&count](TaskList* _pTaskList)
	{
		count += _pTaskList->Size();
	});
	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Scheduler::HaveExecutableTaskRaw(IN_OUT Int64U* _pExecutableTaskLimitTime)
{
	if (waitTasksMap_.Size() == 0)
	{
		return false;
	}

	Int64U firstElementAt;
	const DateTime now = DateTime::Now();
	const Int64U* pNotExpiredKey = waitTasksMap_.UpperBoundKey(now.Tick); // 아직 시간이 만료되지 않은 첫 원소

	if (pNotExpiredKey)
		*_pExecutableTaskLimitTime = *pNotExpiredKey;
	else 
		*_pExecutableTaskLimitTime = MaxInt64U_v;

	if (waitTasksMap_.TryGetFirstKey(firstElementAt) && firstElementAt < *_pExecutableTaskLimitTime)
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::ClearWaitingTaskListRaw()
{
	auto iterator = waitTasksMap_.Begin();
	while (iterator->HasNext())
	{
		TaskList* pList = iterator->Current().value_;
		pList->ForEachDelete();
		delete pList;
		waitTasksMap_.RemoveByIterator(iterator);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
ThreadPool::JoinStrategy Scheduler::ConverToThreadPoolStrategy(JoinStrategy _strategy)
{
	switch (_strategy)
	{
		case JoinStrategy::WaitAllTasks:		return ThreadPool::JoinStrategy::WaitAllTasks;
		case JoinStrategy::WaitOnlyRunningTask: return ThreadPool::JoinStrategy::WaitOnlyRunningTask;
	}

	return ThreadPool::JoinStrategy::WaitOnlyRunningTask;
}

NS_END
