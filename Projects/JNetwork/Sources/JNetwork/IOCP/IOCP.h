#pragma once

#include <JNetwork/WorkerManager.h>

#define IOCP_POST_ORDER_TERMINATE	0x01
#define IOCP_POST_ORDER_PAUSE		0x02
#define IOCP_POST_ORDER_ERROR	   -0x01

namespace JNetwork {

class IOCPWorker;
class IOCP
{
public:
	enum class State
	{
		Uninitialized,
		Initialized,
		Running,
		Paused,
		Joined,
		Destroyed
	};

public:
	IOCP();
	~IOCP();

	bool Create(int threadCount);
	bool Destroy();
	void Run();
	void Pause();
	void Resume();
	void Join();

	void AddPendingCount()				{ m_iPendingOverlappedCount++;}
	void DecreasePendingCount()			{ m_iPendingOverlappedCount--;}
	int GetPendingCount()				{ return m_iPendingOverlappedCount;}

	bool Connect(HANDLE handle, ULONG_PTR completionKey) const;
	BOOL GetStatus(LPDWORD numberOfBytesTransffered, PULONG_PTR completionKey, LPOVERLAPPED* ppOverlapped) const;
	BOOL Post(DWORD dwNumberOfBytesTransferred, ULONG_PTR dwCompletionKey, LPOVERLAPPED pOverlapped) const;
private:
	State m_eState;
	HANDLE m_hIOCP;
	DWORD m_iThreadCount;
	WorkerManager* m_pWorkerManager;
	std::atomic<int> m_iPendingOverlappedCount = 0;

	friend class IOCPManager;

	// ���� I/O �ϷḦ ������� ������ ���� ����Ѵ�.
	// IOCP�� ������ �� �� �ҵ� ī��Ʈ�� 0�� �Ǹ� IOCP �����带 �����ϵ��� �Ѵ�.
	
};

}

