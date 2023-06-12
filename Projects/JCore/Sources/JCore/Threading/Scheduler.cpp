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

void Scheduler::AddTask(SchedulerTask* task) {
	const Int64U uiTick = task->At().Tick;
	{
		NormalLockGuard guard(m_Lock);
		if (m_eState != State::Running) {
			DebugAssertMsg(m_eState == State::Running, "스케쥴러가 Running상태가 아닌데 Task삽입을 시도했습니다.");
			delete task;
			return;
		}

		if (!m_tmTasks.Exist(uiTick)) {
			m_tmTasks.Insert(uiTick, dbg_new TaskList{ task });
		}
		else {
			m_tmTasks[uiTick]->PushBack(task);
		}
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
	m_pThreadPool->Join();

	{
		// NextCall이 있는 Task들의 경우 때문에 Notify이전에 ClearWaitingTaskListRaw()를 호출해서
		// 대기중인 작업들을 정리해서는 안된다.
		NormalLockGuard guard(m_Lock);
		ClearWaitingTaskListRaw();
	}
}

void Scheduler::SchedulingRoutine() {
	TaskList* pvPendingTasks;			// 아직 시간이 되지 않아 실행대기중인 작업
	TaskList vScheduledTasks;			// 시간이 만료되어 실행되어야할 작업

	bool bExit = false;
	bool bHasRemainTasks = false;

	auto fnWait = [this, &bExit, &bHasRemainTasks]()->bool {
		bExit = false;
		bHasRemainTasks = m_tmTasks.Size() > 0;

		if (m_eState == State::JoinWaitAllTasks) {
			bExit = bHasRemainTasks ? false : true;
		}
		else if (m_eState == State::JoinWaitOnlyRunningTask) {
			bExit = true;
		}

		return bExit || bHasRemainTasks;
	};

	NormalLockGuard guard(m_Lock);

	for (;;) {
		const bool bHasInitTasks = m_tmTasks.TryGetFirstValue(pvPendingTasks);
		if (bHasInitTasks) {
			m_CondVar.WaitUntil(guard, pvPendingTasks->At(0)->At(), Move(fnWait));
		}
		else {
			m_CondVar.Wait(guard, Move(fnWait));
		}

		if (bExit) {
			break;
		}

		ExecuteTasks(guard, vScheduledTasks);
	}

	m_eState = State::Joined;
}

void Scheduler::ExecuteTasks(NormalLockGuard& guard, TaskList& scheduledTasks) {
	const int iTaskCount = PopTasks(scheduledTasks);

	for (int i = 0; i < iTaskCount; ++i) {
		SchedulerTask* task = scheduledTasks[i];
		m_pRunningTask = task;
		guard.Unlock();
		task->CallCallback();
		guard.Lock();

		if (!task->CanNextCall()) {
			delete task;
			continue;
		}

		Int64U uiNextCallTime = task->NextCallTime().Tick;
		if (!m_tmTasks.Exist(uiNextCallTime)) {
			m_tmTasks.Insert(uiNextCallTime, dbg_new TaskList{ task });
		}
		else {
			m_tmTasks[uiNextCallTime]->PushBack(task);
		}
	}

	m_pRunningTask = nullptr;
}

int Scheduler::PopTasks(JCORE_OUT Vector<SchedulerTask*>& tasks) {
	tasks.Clear();

	if (m_tmTasks.Size() == 0) {
		return 0;
	}

	// 현재 시각을 기준으로 시간이 만료된 작업들을 가져온다.
	auto cur = m_tmTasks.Begin();
	Int64U* pNotExpiredKey = m_tmTasks.UpperBoundKey(DateTime::Now().Tick); // 아직 시간이 만료되지 않은 첫 원소

	Int64U uiNotExpiredKey = MaxInt64U_v;
	if (pNotExpiredKey) uiNotExpiredKey = *pNotExpiredKey;


	while (cur->HasNext()) {
		auto& curValue = cur->Current();

		const Int64U& expiredTaskKey = curValue.Key;
		const TaskList* expiredTaskList = curValue.Value;

		if (expiredTaskKey >= uiNotExpiredKey) {
			break;
		}

		const bool bRemoved = m_tmTasks.RemoveByIterator(cur);
		DebugAssert(bRemoved);

		for (int i = 0; i < expiredTaskList->Size(); ++i) {
			tasks.PushBack(expiredTaskList->At(i));
		}

		delete expiredTaskList;
	}

	const int iPoppedSize = tasks.Size();
	return iPoppedSize;
}

int Scheduler::WaitingTaskListCountRaw() {
	int iCount = 0;
	m_tmTasks.ForEachValue([&iCount](TaskList* taskList) { iCount += taskList->Size(); });
	return iCount;
}

void Scheduler::ClearWaitingTaskListRaw() {
	auto it = m_tmTasks.Begin();
	while (it->HasNext()) {

		TaskList* pList = it->Current().Value;
		pList->ForEach([](auto task) { delete task; });
		delete pList;
		m_tmTasks.RemoveByIterator(it);
	}
}

NS_JC_END
