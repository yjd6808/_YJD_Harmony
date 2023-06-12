/*
 * 작성자: 윤정도
 * 생성일: 6/8/2023 2:49:34 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Core.h>
#include <JCore/Threading/ThreadPool.h>

NS_JC_BEGIN

// =============================================================================================
// TaskContext
// =============================================================================================

const char* TaskContext::ToStateString(int state)
{
	if (state == eRunningWait) return "RunningWait";
	else if (state == eRunning) return "Running";
	else if (state == eFinished) return "Finished";
	else if (state == eCancelled) return "Cancelled";
	return "none";
}

void TaskContext::Run() {
	
	{
		TASKPOOL_LOG("%s 실행 Begin", m_DebugName.Source());
		NormalLockGuard guard(m_CtxLock);
		if (m_eState == eCancelled) {
			TASKPOOL_LOG("%s 실행 Cancel 리턴", m_DebugName.Source());
			return;
		}
		m_eState = eRunning;
		RunImpl();
		m_eState = eFinished;	
		// (1)
	}
	
	// 아토믹 변수라서 락안걸고 수행했었는데
	// (1)라인 직후 락이 해제되면서 조건변수 Wait부분의 Predicate 함수 체크를 수행을 바로 하게되고
	// 이때 m_eState = eFinished보다 eState = m_eState의 읽기가 먼저 수행되서 Running상태로 읽어버려서
	// 조건이 불만족하게 되었다. 아직 Wait Predication 체크가 끝나기전인 상태인데 NotifyAll을 호출 해버렸고
	// Notification Miss가 발생하게 되어버림
	//  ==> Race Condition이 발생함.
	// m_eState = eFinished;
	m_CtxCondVar.NotifyAll();
	TASKPOOL_LOG("%s 실행 End", m_DebugName.Source());
}

void TaskContext::Cancel() {
	{
		NormalLockGuard guard(m_CtxLock);
		m_eState = eCancelled;
	}
	m_CtxCondVar.NotifyAll();
}


// =============================================================================================
// TaskThread
// =============================================================================================

TaskThread::TaskThread(ConditionVariable& poolCv, ConditionVariable& joinCv, NormalLock& poolLock, bool& poolStopFlag, TaskQueue& poolTaskQueue, int code)
	: RunnableThread()
	, m_PoolCondVar(poolCv)
	, m_JoinCondVar(joinCv)
	, m_PoolLock(poolLock)
	, m_bPoolStopFlag(poolStopFlag)
	, m_qPoolWaitingTasks(poolTaskQueue)
	, m_bJoinWait(false)
	, m_iCode(code)
{}

TaskThread::~TaskThread()
{
	JCORE_PASS;
}

void TaskThread::CancelRunningTask() {
	if (m_spRunningTask != nullptr) {
		NormalLockGuard guard(m_Lock);
		if (m_spRunningTask != nullptr) {
			m_spRunningTask->Cancel();
			m_spRunningTask = nullptr;
		}
	}
}

void TaskThread::WorkerThread()
{
	int iCount = 0;
	bool bExit = false;
	TaskContextPtr spRunningTask;

	for (;;) {
		if (spRunningTask != nullptr) {
			{
				NormalLockGuard guard(m_Lock);
				m_spRunningTask = spRunningTask;
			}
			spRunningTask->Run();
			spRunningTask = nullptr;
		}

		NormalLockGuard guard(m_PoolLock);
		m_PoolCondVar.Wait(guard, [this, &iCount, &bExit] {
			iCount = m_qPoolWaitingTasks.Size();
			bExit = m_bPoolStopFlag || m_bThisStopFlag;
			return bExit || iCount > 0;
		});

		if (bExit)
			break;

		m_qPoolWaitingTasks.TryDequeue(spRunningTask);
	}

	TASKPOOL_LOG("쓰레드 %d 종료됨", m_iCode);

	m_bJoinWait = true;
	m_JoinCondVar.NotifyOne();
}



// =============================================================================================
// ThreadPool
// =============================================================================================

ThreadPool::ThreadPool(int poolSize)
	: m_vThreads(poolSize)
	, m_eState(eRunning)
	, m_bStopFlag(false) {
	for (int i = 0; i < poolSize; ++i) {
		TaskThreadPtr spThread = MakeShared<TaskThread>(m_CondVar, m_JoinCondVar, m_Lock, m_bStopFlag, m_qWaitingTasks, i);
		m_vThreads.PushBack(spThread);
		spThread->Start();
	}
}

void ThreadPool::Join() {
	m_eState = eJoinWait;
	{
		NormalLockGuard guard(m_Lock);
		m_bStopFlag = true;

		while (!m_qWaitingTasks.IsEmpty()) {
			TaskContextPtr& spTask = m_qWaitingTasks.Front();
			spTask->Cancel();
			m_qWaitingTasks.Dequeue();
		}

		// for (int i = 0; i < m_vThreads.Size(); ++i) {
		// 		m_vThreads[i]->CancelRunningTask();	// 이미 실행중인 작업은 완료될때까지 기다리도록 하는게 나을듯?
		// }

	}
	m_CondVar.NotifyAll();

	{
		NormalLockGuard guard(m_Lock);
		for (int i = 0; i < m_vThreads.Size(); ++i) {
			TASKPOOL_LOG("조인1-%d 시작", i);
			TaskThread* pThread = m_vThreads[i].GetPtr();
			m_JoinCondVar.Wait(guard, [pThread] { return pThread->IsJoinWait(); });
			pThread->Join();
			TASKPOOL_LOG("조인1-%d 완료", i);
		}
		m_vThreads.Clear();
	}

	m_eState = eJoined;
}

int ThreadPool::WaitingTaskCount() {
	NormalLockGuard guard(m_Lock);
	return m_qWaitingTasks.Size();
}

NS_JC_END


