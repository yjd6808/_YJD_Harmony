/*
	�ۼ��� : ������
	
	�׳� �ܼ��� ���� �۾������� ����ϴ� ��Ŀ ������
*/

#pragma once

#include <Windows.h>
#include <JNetwork/Worker.h>

class BackgroundWorker : public JNetwork::Worker
{
protected:
	BackgroundWorker();
	~BackgroundWorker() override;

	void Run(void* param = nullptr) override;
	void JoinWait(HANDLE waitHandle) override;
	void Join() override;
	void Pause(HANDLE waitHandle) override;
	void Resume() override;

	void WorkerThread(void* param) override;
private:
	std::thread m_Thread;

	friend class WorkerManager;
	friend class IOCPPostOrder;
};

