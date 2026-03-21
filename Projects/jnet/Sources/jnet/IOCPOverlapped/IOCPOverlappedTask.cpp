/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 6:04:09 PM
 * =====================
 *
 */

#include "jnet/IOCPOverlapped/IOCPOverlappedTask.h"
#include "jnet/IOCP/IOCPWorker.h"

USING_NS_JNET;


//////////////////////////////////////////////////////////////////////////////////////////
void IOCPOverlappedTask::Process(BOOL _result, _u32l _numberOfBytesTransferred, NULLABLE IOCPPostOrder* _pCompletionKey, IOCPWorker* _pWorker)
{
	(void)_numberOfBytesTransferred;
	(void)_pCompletionKey;

	TIOCPTask* pTask = task_.GetPtr();
	_u32 errorCode;

	if (IsFailed(_result, errorCode))
	{
		pTask->OnFailed(errorCode);
		pTask->state_ = IOCPTaskState::eFinished;
		pTask->waitHandle_.Signal();
		ProcessFinally();
		return;
	}

	pTask->Call();
	pTask->state_ = IOCPTaskState::eReady;
	pTask->waitHandle_.Signal();

	if (pTask->pContinuousTask_ != nullptr)
	{
		pTask->pContinuousTaskLock_->Lock();
		if (pTask->pContinuousTask_ != nullptr)
		{
			pTask->pContinuousTask_->Start();
		}
		pTask->pContinuousTaskLock_->Unlock();
	}

	if (pIocp_->IsPollingMode())
	{
		_pWorker->EnqueueTask(task_);
	}
	else
	{
		pIocp_->OnTaskCompleted(pTask);
	}

	pTask->state_ = IOCPTaskState::eFinished;
	ProcessFinally();
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPOverlappedTask::ProcessFinally()
{
	TIOCPTask* pTask = task_.GetPtr();

	if (pTask)
	{
		pTask->CallFinally();
	}
}

