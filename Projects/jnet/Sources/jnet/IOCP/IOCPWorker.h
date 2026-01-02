/*
	작성자 : 윤정도
	
	IOCPWorker 쓰레드는 IOCPWorkerManager에서만 생성하고 다룰 수 있다.
*/

#pragma once

#include <jnet/IOCP/IOCP.h>
#include <jnet/Worker.h>

NS_JNET_BEGIN

class IOCPWorker : public Worker
{
public:
	IOCPWorker(IOCP* _pIocp);
	~IOCPWorker() override;

	void Run(void* _pParam = nullptr) override;
	void JoinWait(jc::WaitHandle* _pWaitHandle) override;
	void Join() override;

	void WorkerThread(void* _pParam) override;

private:
	IOCP* iocp_;

	friend class WorkerManager;
	friend struct IOCPPostOrder;
};

NS_JNET_END
