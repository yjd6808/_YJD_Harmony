/*
	작성자 : 윤정도
	
	IOCPWorker 쓰레드는 IOCPWorkerManager에서만 생성하고 다룰 수 있다.
*/

#pragma once

#include "jc/Container/ListQueue.h"
#include "jc/Sync/NormalLock.h"

#include "jnet/IOCP/IOCP.h"
#include "jnet/IOCP/IOCPTaskAbstract.h"
#include "jnet/Worker.h"

NS_JNET_BEGIN

class IOCPTaskAbstract;
class IOCPWorker : public Worker
{
public:
	IOCPWorker(IOCP* _pIocp);
	~IOCPWorker() override;

	void Run(void* _pParam = nullptr) override;
	void JoinWait(jc::WaitHandle* _pWaitHandle) override;
	void Join() override;

	void WorkerThread(void* _pParam) override;

	void EnqueueTask(const IOCPTaskAbstractPtr& _pTask);
	void PopAllTasksWithSwap(jc::Vector<IOCPTaskAbstractPtr>& _v);
	bool HasTask() const { return hasTask_.Load(); }

private:
	IOCP* iocp_;
	jc::ListQueue<IOCPTaskAbstractPtr>* taskQueue_;
	jc::ListQueue<IOCPTaskAbstractPtr>* swapTaskQueue_;
	jc::NormalLock taskQueueLock_;
	jc::AtomicBool hasTask_;

	friend class WorkerManager;
	friend struct IOCPPostOrder;
};

NS_END
