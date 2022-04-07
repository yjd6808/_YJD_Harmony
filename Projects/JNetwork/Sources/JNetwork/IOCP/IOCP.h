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

	// 현재 I/O 완료를 대기중인 오버랩 수를 기록한다.
	// IOCP를 종료할 때 이 팬딩 카운트가 0이 되면 IOCP 쓰레드를 해제하도록 한다.
	
};

}

