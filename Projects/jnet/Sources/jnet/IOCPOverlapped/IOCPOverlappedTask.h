/*
	작성자 : 윤정도
*/

#pragma once


#include <jnet/IOCPOverlapped/IOCPOverlapped.h>
#include <jnet/IOCP/IOCPTaskState.h>

NS_JNET_BEGIN

class IOCPOverlappedTask : public IOCPOverlapped
{
	using TIOCPTask = IOCPTaskAbstract;
	using TIOCPTaskPtr = jc::SharedPtr<TIOCPTask>;

public:
	IOCPOverlappedTask(IOCP* _pIocp, TIOCPTaskPtr _pIocpTask)
	: IOCPOverlapped(_pIocp, Type::Task)
	, task_(_pIocpTask)
	{
	}

	void Process(BOOL _result, _u32l _numberOfBytesTransferred, NULLABLE IOCPPostOrder* _pCompletionKey, IOCPWorker* _pWorker) override;
	void ProcessFinally();

	const TIOCPTaskPtr& GetTask() const { return task_; }

private:
	TIOCPTaskPtr task_;
};

NS_END
