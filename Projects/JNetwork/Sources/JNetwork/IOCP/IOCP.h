#pragma once

#include <JNetwork/IOCP/WorkerManager.h>

#define IOCP_POST_ORDER_TERMINATE	0x01
#define IOCP_POST_ORDER_PAUSE		0x02

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

	bool Connect(HANDLE handle, ULONG_PTR completionKey);
	BOOL GetStatus(LPDWORD numberOfBytesTransffered, PULONG_PTR completionKey, LPOVERLAPPED* ppOverlapped);
	BOOL Post(DWORD dwNumberOfBytesTransferred, ULONG_PTR dwCompletionKey, LPOVERLAPPED pOverlapped);
private:
	State m_eState;
	HANDLE m_hIOCP;
	DWORD m_iThreadCount;
	WorkerManager* m_pWorkerManager;

	friend class IOCPManager;
};

}

