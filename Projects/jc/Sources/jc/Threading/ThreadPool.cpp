/*
 * 작성자: 윤정도
 * 생성일: 6/8/2023 2:49:34 PM
 * =====================
 *
 */

#include <jc/Core.h>
#include <jc/Threading/ThreadPool.h>

NS_JC_BEGIN
// =============================================================================================
// TaskContext
// =============================================================================================
const char* TaskContext::ToStateString(int _state)
{
	if (_state == eRunningWait) return "RunningWait";
	else if (_state == eRunning) return "Running";
	else if (_state == eFinished) return "Finished";
	else if (_state == eCancelled) return "Cancelled";
	return "none";
}

//////////////////////////////////////////////////////////////////////////////////////////
void TaskContext::Run()
{
	{
		TASKPOOL_LOG("%s 실행 Begin", m_DebugName.Source());
		NormalLockGuard guard(m_CtxLock);
		if (m_eState == eCancelled)
		{
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

//////////////////////////////////////////////////////////////////////////////////////////
void TaskContext::Cancel()
{
	{
		NormalLockGuard guard(m_CtxLock);
		m_eState = eCancelled;
	}
	m_CtxCondVar.NotifyAll();
}


// =============================================================================================
// TaskThread
// =============================================================================================
TaskThread::TaskThread(
	ConditionVariable& _poolCv, 
	ConditionVariable& _joinCv, 
	NormalLock& _poolLock, 
	int& _poolState,
	TaskQueue& _poolTaskQueue, 
	int _code)
//////////////////////////////////////////////////////////////////////////////////////////
: RunnableThread()
, m_PoolCondVar(_poolCv)
, m_JoinCondVar(_joinCv)
, m_PoolLock(_poolLock)
, m_ePoolState(_poolState)
, m_qPoolWaitingTasks(_poolTaskQueue)
, m_iCode(_code)
, m_bJoinWait(false)
{
}

TaskThread::~TaskThread()
{
}

void TaskThread::CancelRunningTask()
{
	if (m_spRunningTask != nullptr)
	{
		NormalLockGuard guard(m_Lock);
		if (m_spRunningTask != nullptr)
		{
			m_spRunningTask->Cancel();
			m_spRunningTask = nullptr;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void TaskThread::WorkerThread()
{
	int iCount = 0;
	bool bExit = false;
	TaskContextPtr spRunningTask;

	for (;;)
	{
		if (spRunningTask != nullptr)
		{
			{
				NormalLockGuard guard(m_Lock);
				m_spRunningTask = spRunningTask;
			}
			spRunningTask->Run();
			spRunningTask = nullptr;
		}

		if (bExit)
			break;

		NormalLockGuard guard(m_PoolLock);
		m_PoolCondVar.Wait(guard, [this, &iCount, &bExit]
		{
			iCount = m_qPoolWaitingTasks.Size();
			bExit = false;

			if (m_ePoolState == ThreadPool::State::eJoinWaitAll)
			{
				bExit = iCount <= 0;
			}
			else if (m_ePoolState == ThreadPool::State::eJoinWaitOnlyRunningTask)
			{
				bExit = true;
			}

			return bExit || iCount > 0;
		});

		m_qPoolWaitingTasks.TryDequeue(spRunningTask);
	}

	TASKPOOL_LOG("쓰레드 %d 종료됨", m_iCode);
	m_bJoinWait = true;
	m_JoinCondVar.NotifyOne();
}


// =============================================================================================
// ThreadPool// =============================================================================================

ThreadPool::ThreadPool(int _poolSize)
: threads_(_poolSize)
, state_(eRunning)
{
	for (int i = 0; i < _poolSize; ++i)
	{
		TaskThreadPtr spThread = MakeShared<TaskThread>(condVar_, joinCondVar_, lock_, state_, waitingTasks_, i);
		threads_.PushBack(spThread);
		spThread->Start();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ThreadPool::Join(JoinStrategy _strategy)
{
	{
		NormalLockGuard guard(lock_);
		state_ = _strategy == JoinStrategy::WaitOnlyRunningTask ? eJoinWaitOnlyRunningTask : eJoinWaitAll;

		if (_strategy == JoinStrategy::WaitOnlyRunningTask)
		{
			while (!waitingTasks_.IsEmpty())
			{
				TaskContextPtr& spTask = waitingTasks_.Front();
				spTask->Cancel();
				waitingTasks_.Dequeue();
			}
		}

		// for (int i = 0; i < m_vThreads.Size(); ++i) {
		// 		m_vThreads[i]->CancelRunningTask();	// 이미 실행중인 작업은 완료될때까지 기다리도록 하는게 나을듯?
		// }
	}
	condVar_.NotifyAll();

	{
		NormalLockGuard guard(lock_);
		for (int i = 0; i < threads_.Size(); ++i)
		{
			TASKPOOL_LOG("조인1-%d 시작", i);
			TaskThread* pThread = threads_[i].GetPtr();
			joinCondVar_.Wait(guard, [pThread] { return pThread->IsJoinWait(); });
			pThread->Join();
			TASKPOOL_LOG("조인1-%d 완료", i);
		}
		threads_.Clear();
		state_ = eJoined;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
int ThreadPool::WaitingTaskCount()
{
	NormalLockGuard guard(lock_);
	return waitingTasks_.Size();
}

NS_END
