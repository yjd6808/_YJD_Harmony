/*
	작성자 : 윤정도
	
	IOCPWorker 쓰레드는 IOCPWorkerManager에서만 생성하고 다룰 수 있다.
*/

#pragma once

#include <Windows.h>
#include <JNetwork/IOCP/IOCP.h>
#include <JNetwork/Worker.h>

namespace JNetwork {

class IOCPWorker : public Worker
{
protected:
	IOCPWorker(IOCP* iocp);
	~IOCPWorker() override;

	void Run(void* param = nullptr) override;
	void JoinWait(HANDLE waitHandle) override;
	void Join() override;
	void Pause(HANDLE waitHandle) override;
	void Resume() override;

	void WorkerThread(void* param) override;
private:
	std::thread m_Thread;
	IOCP* m_pIocp;

	friend class WorkerManager;
	friend class IOCPPostOrder;
};

} // namespace JNetwork