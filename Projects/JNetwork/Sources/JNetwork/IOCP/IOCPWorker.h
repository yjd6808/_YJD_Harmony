/*
	작성자 : 윤정도
	
	IOCPWorker 쓰레드는 IOCPWorkerManager에서만 생성하고 다룰 수 있다.
*/

#pragma once

#include <JNetwork/IOCP/IOCP.h>
#include <JNetwork/Worker.h>

NS_JNET_BEGIN

class IOCPWorker : public Worker
{
public:
	IOCPWorker(IOCP* iocp);
	~IOCPWorker() override;

	void Run(void* param = nullptr) override;
	void JoinWait(JCore::WaitHandle* waitHandle) override;
	void Join() override;

	void WorkerThread(void* param) override;
private:
	IOCP* m_pIocp;

	friend class WorkerManager;
	friend struct IOCPPostOrder;
};

NS_JNET_END