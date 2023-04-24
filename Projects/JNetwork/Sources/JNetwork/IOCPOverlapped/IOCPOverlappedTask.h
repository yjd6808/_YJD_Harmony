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
	IOCPOverlappedTask(IOCP* iocp, TIOCPTaskPtr iocpTask)
		: IOCPOverlapped(iocp, Type::Custom)
		, m_spTask(iocpTask)
	{}

	
	void Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) override {

		TIOCPTask* pTask = m_spTask.GetPtr();
		Int32U iErrorCode;

		if (IsFailed(result, iErrorCode)) {
			pTask->m_spResult->Success = false;
			pTask->m_spResult->ErrorCode = iErrorCode;
			pTask->m_eState = IOCPTaskState::eFinished;
			ProcessFinally();
			return;
		}

		pTask->m_fnTask(*pTask->m_spResult);
		pTask->m_eState = IOCPTaskState::eReady;
		pTask->m_WaitHandle.Signal();

		pTask->m_lkContinuousTaskLock.Lock();
		if (pTask->m_spContinuousTask != nullptr)
			pTask->m_spContinuousTask->Start();

		pTask->m_eState = IOCPTaskState::eFinished;
		pTask->m_lkContinuousTaskLock.Unlock();

		
		ProcessFinally();
	}

	void ProcessFinally() {
		TIOCPTask* pTask = m_spTask.GetPtr();

		if (pTask->m_fnFinally)
			pTask->m_fnFinally(*pTask->m_spResult);
	}

private:
	TIOCPTaskPtr m_spTask;
};

NS_JNET_END