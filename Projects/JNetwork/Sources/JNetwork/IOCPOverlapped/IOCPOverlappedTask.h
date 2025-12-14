/*
	작성자 : 윤정도
*/

#pragma once


#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>
#include <JNetwork/IOCP/IOCPTask.h>

NS_JNET_BEGIN

template <typename T>
class IOCPOverlappedTask : public IOCPOverlapped
{
	using TIOCPTask = IOCPTask<T>;
	using TIOCPTaskPtr = JCore::SharedPtr<IOCPTask<T>>;
	using TFnTask = FnTask<T>;

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

		pTask->fnTask_(*pTask->result_);
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

		if (pTask->fnFinally_)
		{
			pTask->fnFinally_(*pTask->result_);
		}
	}

private:
	TIOCPTaskPtr task_;
};

NS_JNET_END
