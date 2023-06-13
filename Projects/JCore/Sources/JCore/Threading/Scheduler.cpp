/*
 * 작성자: 윤정도
 * 생성일: 6/13/2023 4:54:05 AM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Limit.h>
#include <JCore/Threading/Scheduler.h>

NS_JC_BEGIN

Scheduler::Scheduler(int threadCount)
	: m_pThreadPool(dbg_new ThreadPool(threadCount))
	, m_SchedulingThread(std::bind(&Scheduler::SchedulingRoutine, this))
	, m_eState(State::Running)
{}

Scheduler::~Scheduler() {
	JCORE_DELETE_SAFE(m_pThreadPool);
}

void Scheduler::AddFirstTask(SchedulerTask* task) {
	const Int64U uiTick = task->At().Tick;
	{
		NormalLockGuard guard(m_Lock);
		if (m_eState != State::Running) {
			DebugAssertMsg(m_eState == State::Running, "스케쥴러가 Running상태가 아닌데 Task삽입을 시도했습니다.");
			delete task;
			return;
		}

		AddTaskRaw(task);
	}
	m_CondVar.NotifyOne();
}

void Scheduler::AddTask(SchedulerTask* task) {
	
	{
		NormalLockGuard guard(m_Lock);
		AddTaskRaw(task);
	}
	m_CondVar.NotifyOne();
}

int Scheduler::WaitingTaskCount() {
	NormalLockGuard guard(m_Lock);
	return WaitingTaskListCountRaw();
}

void Scheduler::Join(JoinStrategy strategy) {
	{
		NormalLockGuard guard(m_Lock);
		m_eState = strategy == JoinStrategy::WaitOnlyRunningTask ? State::JoinWaitOnlyRunningTask : State::JoinWaitAllTasks;
	}

	m_CondVar.NotifyOne();
	m_SchedulingThread.Join();
	m_pThreadPool->Join(ConverToThreadPoolStrategy(strategy));

	{
		// NextCall이 있는 Task들의 경우 때문에 Notify이전에 ClearWaitingTaskListRaw()를 호출해서
		// 대기중인 작업들을 정리해서는 안된다.
		NormalLockGuard guard(m_Lock);
		ClearWaitingTaskListRaw();
		m_ScheduledTaskMap.Clear();
	}
}

void Scheduler::AddTaskRaw(SchedulerTask* task) {
	Int64U uiAt = task->At().Tick;
	if (!m_tmWaitTasks.Exist(uiAt)) {
		m_tmWaitTasks.Insert(uiAt, dbg_new TaskList{ task });
	} else {
		m_tmWaitTasks[uiAt]->PushBack(task);
	}
}

bool Scheduler::HaveEarlierTask(const DateTime& waitUntil) {
	TaskList* pvPendingTasks;
	if (m_tmWaitTasks.TryGetFirstValue(pvPendingTasks)) {
		return pvPendingTasks->At(0)->At() < waitUntil;
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

void Scheduler::ExecuteTasks(TaskList& scheduledTasks, const Int64U* executableTaskLimitTime) {
	const int iTaskCount = PopTasks(scheduledTasks, executableTaskLimitTime);
	
	for (int i = 0; i < iTaskCount; ++i) {
		SchedulerTask* task = scheduledTasks[i];
		m_ScheduledTaskMap.Insert(task, task);

		m_pRunningTask = task;
		m_pThreadPool->Run([task, this] {
			task->CallCallback();

			if (!task->CanNextCall()) {

				{
					NormalLockGuard guard(m_Lock);
					m_ScheduledTaskMap.Remove(task);
				}

				delete task;
				m_CondVar.NotifyOne();
				return;
			}

			AddTask(task);
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
int Scheduler::PopTasks(JCORE_OUT Vector<SchedulerTask*>& executableTasks, const Int64U* executableTaskLimitTime) {
	executableTasks.Clear();

	if (m_tmWaitTasks.Size() == 0 || executableTaskLimitTime == nullptr) {
		return 0;
	}

	// 현재 시각을 기준으로 시간이 만료된 작업들을 가져온다.
	auto cur = m_tmWaitTasks.Begin();

	while (cur->HasNext()) {
		auto& curValue = cur->Current();

		const Int64U& expiredTaskKey = curValue.Key;
		const TaskList* expiredTaskList = curValue.Value;

		if (expiredTaskKey >= *executableTaskLimitTime) {
			break;
		}

		const bool bRemoved = m_tmWaitTasks.RemoveByIterator(cur);
		DebugAssert(bRemoved);

		for (int i = 0; i < expiredTaskList->Size(); ++i) {
			executableTasks.PushBack(expiredTaskList->At(i));
		}

		delete expiredTaskList;
	}

	const int iPoppedSize = executableTasks.Size();
	return iPoppedSize;
}

int Scheduler::WaitingTaskListCountRaw() {
	int iCount = 0;
	m_tmWaitTasks.ForEachValue([&iCount](TaskList* taskList) { iCount += taskList->Size(); });
	return iCount;
}

bool Scheduler::HaveExecutableTaskRaw(JCORE_IN_OUT Int64U* executableTaskLimitTime) {
	if (m_tmWaitTasks.Size() == 0) {
		return false;
	}

	Int64U uiFirstElementAt;
	const DateTime now = DateTime::Now();
	const Int64U* pNotExpiredKey = m_tmWaitTasks.UpperBoundKey(now.Tick); // 아직 시간이 만료되지 않은 첫 원소

	if (pNotExpiredKey)
		*executableTaskLimitTime = *pNotExpiredKey;
	else 
		*executableTaskLimitTime = MaxInt64U_v;

	if (m_tmWaitTasks.TryGetFirstKey(uiFirstElementAt) && uiFirstElementAt < *executableTaskLimitTime) {
		return true;
	}
	return false;
}

void Scheduler::ClearWaitingTaskListRaw() {
	auto it = m_tmWaitTasks.Begin();
	while (it->HasNext()) {

		TaskList* pList = it->Current().Value;
		pList->ForEach([](auto task) { delete task; });
		delete pList;
		m_tmWaitTasks.RemoveByIterator(it);
	}
}

ThreadPool::JoinStrategy Scheduler::ConverToThreadPoolStrategy(JoinStrategy strategy) {
	switch (strategy) {
		case JoinStrategy::WaitAllTasks:		return ThreadPool::JoinStrategy::WaitAllTasks;
		case JoinStrategy::WaitOnlyRunningTask: return ThreadPool::JoinStrategy::WaitOnlyRunningTask;
	}

	return ThreadPool::JoinStrategy::WaitOnlyRunningTask;
}

NS_JC_END
