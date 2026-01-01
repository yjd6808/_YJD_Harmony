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
	: m_pThreadPool(dbg_new ThreadPool(_threadCount))
	, m_SchedulingThread(JCORE_CALLBACK_0(Scheduler::SchedulingRoutine, this))
	, m_eState(State::Running)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Scheduler::~Scheduler()
{
	JCORE_DELETE_SAFE(m_pThreadPool);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::AddFirstTask(SchedulerTask* _pTask)
{
	{
		NormalLockGuard guard(m_Lock);
		if (m_eState != State::Running)
		{
			jc_assert_msg(m_eState == State::Running, "스케쥴러가 Running상태가 아닌데 Task삽입을 시도했습니다.");
			delete _pTask;
			return;
		}

		AddTaskRaw(_pTask);
	}
	m_CondVar.NotifyOne();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::AddTask(SchedulerTask* _pTask)
{
	{
		NormalLockGuard guard(m_Lock);
		AddTaskRaw(_pTask);
	}
	m_CondVar.NotifyOne();
}

//////////////////////////////////////////////////////////////////////////////////////////
int Scheduler::WaitingTaskCount()
{
	NormalLockGuard guard(m_Lock);
	return WaitingTaskListCountRaw();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::Join(JoinStrategy _strategy)
{
	{
		NormalLockGuard guard(m_Lock);
		m_eState = _strategy == JoinStrategy::WaitOnlyRunningTask ? State::JoinWaitOnlyRunningTask : State::JoinWaitAllTasks;
	}

	m_CondVar.NotifyOne();
	m_SchedulingThread.Join();
	m_pThreadPool->Join(ConverToThreadPoolStrategy(_strategy));

	{
		// NextCall이 있는 Task들의 경우 때문에 Notify이전에 ClearWaitingTaskListRaw()를 호출해서
		// 대기중인 작업들을 정리해서는 안된다.
		NormalLockGuard guard(m_Lock);
		ClearWaitingTaskListRaw();
		m_ScheduledTaskMap.Clear();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::AddTaskRaw(SchedulerTask* _pTask)
{
	Int64U atTick = _pTask->At().Tick;
	if (!m_tmWaitTasks.Exist(atTick))
	{
		m_tmWaitTasks.Insert(atTick, dbg_new TaskList{ _pTask });
	}
	else
	{
		m_tmWaitTasks[atTick]->PushBack(_pTask);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Scheduler::HaveEarlierTask(const DateTime& _waitUntil)
{
	TaskList* pPendingTasks;
	if (m_tmWaitTasks.TryGetFirstValue(pPendingTasks))
	{
		return pPendingTasks->At(0)->At() < _waitUntil;
	}
	return false;
}

void Scheduler::SchedulingRoutine() {
	TaskList* pvPendingTasks;						// 아직 시간이 되지 않아 실행대기중인 작업
	TaskList vScheduledTasks;						// 시간이 만료되어 실행되어야할 작업

	bool bExit = false;
	bool bHaveExecutableTasks = false;				// 실행가능한 작업이 있는 경우
	bool bHaveWaitTasks = false;
	DateTime dtWaitUntil = 0;
	Int64U uiExecutableTaskLimitTime = 0;
	
	

	auto fnWait = [&]()->bool {
		bExit = false;
		bHaveWaitTasks = m_tmWaitTasks.Size();
		bHaveExecutableTasks = HaveExecutableTaskRaw(&uiExecutableTaskLimitTime);
		const bool bWaitUntil = dtWaitUntil.Tick != 0; // 시그널이 올때까지 대기해야하는 상태(Wait)인지 아니면 특정 시각까지 대기해야하는 상태인지(WaitUntil)

		if (m_eState == State::JoinWaitAllTasks) {
			bExit = !bHaveWaitTasks && !bHaveExecutableTasks && m_ScheduledTaskMap.Size() <= 0; // 대기 작업과 스케쥴링 중인 작업이 없을 경우
		} else if (m_eState == State::JoinWaitOnlyRunningTask) {
			bExit = true;
		}

		

		if (!bWaitUntil) {
			// Wait 상태인 경우 작업 유무를 확인하여 WaitUntil로 재전환 될 수 있도록 한다.
			return bHaveWaitTasks || bExit || bHaveExecutableTasks;
		}

		const bool bHaveEarlierTask = HaveEarlierTask(dtWaitUntil); //현재 WaitUntil 까지 대기중인 시간보다 더빠른 작업이 있는 경우 더 빠른 시각으로 WaitUntil 재적용
		return bHaveEarlierTask || bExit || bHaveExecutableTasks;
	};

	
	NormalLockGuard guard(m_Lock);

	for (;;) {
		const bool bHasInitTasks = m_tmWaitTasks.TryGetFirstValue(pvPendingTasks);
		if (bHasInitTasks) {
			dtWaitUntil = pvPendingTasks->At(0)->At();
			m_CondVar.WaitUntil(guard, dtWaitUntil, Move(fnWait));
		} else {
			dtWaitUntil.Tick = 0;
			m_CondVar.Wait(guard, Move(fnWait));
		}

		if (bExit) {
			break;
		}

		ExecuteTasks(vScheduledTasks, bHaveExecutableTasks ? &uiExecutableTaskLimitTime : nullptr);
		uiExecutableTaskLimitTime = 0;
	}

	m_eState = State::Joined;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::ExecuteTasks(TaskList& _scheduledTasks, const Int64U* _pExecutableTaskLimitTime)
{
	const int taskCount = PopTasks(_scheduledTasks, _pExecutableTaskLimitTime);
	
	for (int index = 0; index < taskCount; ++index)
	{
		SchedulerTask* pTask = _scheduledTasks[index];
		m_ScheduledTaskMap.Insert(pTask, pTask);

		m_pRunningTask = pTask;
		m_pThreadPool->Run([pTask, this]
		{
			pTask->CallCallback();

			if (!pTask->CanNextCall())
			{
				{
					NormalLockGuard guard(m_Lock);
					m_ScheduledTaskMap.Remove(pTask);
				}

				delete pTask;
				m_CondVar.NotifyOne();
				return;
			}

			AddTask(pTask);
		});
	}

	m_pRunningTask = nullptr;
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

	if (m_tmWaitTasks.Size() == 0 || _pExecutableTaskLimitTime == nullptr)
	{
		return 0;
	}

	// 현재 시각을 기준으로 시간이 만료된 작업들을 가져온다.
	auto iterator = m_tmWaitTasks.Begin();

	while (iterator->HasNext())
	{
		auto& currentValue = iterator->Current();

		const Int64U& expiredTaskKey = currentValue.key_;
		const TaskList* pExpiredTaskList = currentValue.value_;

		if (expiredTaskKey >= *_pExecutableTaskLimitTime)
		{
			break;
		}

		const bool removed = m_tmWaitTasks.RemoveByIterator(iterator);
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
	m_tmWaitTasks.ForEachValue([&count](TaskList* pTaskList)
	{
		count += pTaskList->Size();
	});
	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Scheduler::HaveExecutableTaskRaw(IN_OUT Int64U* _pExecutableTaskLimitTime)
{
	if (m_tmWaitTasks.Size() == 0)
	{
		return false;
	}

	Int64U firstElementAt;
	const DateTime now = DateTime::Now();
	const Int64U* pNotExpiredKey = m_tmWaitTasks.UpperBoundKey(now.Tick); // 아직 시간이 만료되지 않은 첫 원소

	if (pNotExpiredKey)
		*_pExecutableTaskLimitTime = *pNotExpiredKey;
	else 
		*_pExecutableTaskLimitTime = MaxInt64U_v;

	if (m_tmWaitTasks.TryGetFirstKey(firstElementAt) && firstElementAt < *_pExecutableTaskLimitTime)
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::ClearWaitingTaskListRaw()
{
	auto iterator = m_tmWaitTasks.Begin();
	while (iterator->HasNext())
	{
		TaskList* pList = iterator->Current().value_;
		pList->ForEachDelete();
		delete pList;
		m_tmWaitTasks.RemoveByIterator(iterator);
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

NS_JC_END
