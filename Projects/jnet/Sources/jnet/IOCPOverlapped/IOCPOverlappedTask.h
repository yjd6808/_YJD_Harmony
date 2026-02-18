/*
	작성자 : 윤정도
*/

#pragma once


#include <jnet/IOCPOverlapped/IOCPOverlapped.h>
#include <jnet/IOCP/IOCPTaskState.h>

NS_JNET_BEGIN

template <typename T>
class IOCPTask;		// 전방 선언

template <typename T>
class IOCPOverlappedTask : public IOCPOverlapped
{
	using TIOCPTask = IOCPTask<T>;
	using TIOCPTaskPtr = jc::SharedPtr<IOCPTask<T>>;

public:
	IOCPOverlappedTask(IOCP* _pIocp, TIOCPTaskPtr _pIocpTask)
		: IOCPOverlapped(_pIocp, Type::Custom)
		, task_(_pIocpTask)
	{
	}

	void Process(BOOL _result, Int32UL _numberOfBytesTransferred, IOCPPostOrder* _pCompletionKey) override
	{
		(void)_numberOfBytesTransferred;
		(void)_pCompletionKey;

		TIOCPTask* pTask = task_.GetPtr();
		Int32U errorCode;

		if (IsFailed(_result, errorCode))
		{
			pTask->result_->success_ = false;
			pTask->result_->errorCode_ = errorCode;
			pTask->state_ = IOCPTaskState::eFinished;
			ProcessFinally();
			return;
		}

		pTask->Call();
		pTask->state_ = IOCPTaskState::eReady;
		pTask->waitHandle_.Signal();

		pTask->continuousTaskLock_.Lock();
		if (pTask->continuousTask_ != nullptr)
		{
			pTask->continuousTask_->Start();
		}

		pTask->state_ = IOCPTaskState::eFinished;
		pTask->continuousTaskLock_.Unlock();

		ProcessFinally();
	}

	void ProcessFinally()
	{
		TIOCPTask* pTask = task_.GetPtr();

		if (pTask)
		{
			pTask->CallFinally();
		}
	}

private:
	TIOCPTaskPtr task_;
};

NS_END
