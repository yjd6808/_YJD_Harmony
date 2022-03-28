/*
	�ۼ��� : ������
	
	IOCPWorker ������� IOCPWorkerManager������ �����ϰ� �ٷ� �� �ִ�.
*/

#pragma once

#include <Windows.h>
#include <JNetwork/IOCP/IOCP.h>
#include <JNetwork/IOCP/Worker.h>

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