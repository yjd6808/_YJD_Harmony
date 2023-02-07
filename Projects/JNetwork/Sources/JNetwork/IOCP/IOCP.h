/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Primitives/Atomic.h>
#include <JNetwork/WorkerManager.h>

#define IOCP_POST_ORDER_TERMINATE	0x01
#define IOCP_POST_ORDER_ERROR	   -0x01

NS_JNET_BEGIN

class IOCPWorker;
class IOCP
{
public:
	enum class State
	{
		Uninitialized,
		Initialized,
		Running,
		Joined,
		Destroyed
	};

public:
	IOCP();
	~IOCP();

	bool Create(int threadCount);
	bool Destroy();
	void Run();
	void Join();

	void AddPendingCount()				{ ++m_iPendingOverlappedCount;}
	void DecreasePendingCount()			{ --m_iPendingOverlappedCount;}
	int GetPendingCount()				{ return m_iPendingOverlappedCount;}

	bool Connect(WinHandle handle, ULONG_PTR completionKey) const;
	BOOL GetStatus(Int32UL* numberOfBytesTransffered, PULONG_PTR completionKey, LPOVERLAPPED* ppOverlapped) const;
	BOOL Post(Int32UL dwNumberOfBytesTransferred, ULONG_PTR dwCompletionKey, LPOVERLAPPED pOverlapped) const;
private:
	State m_eState;
	WinHandle m_hIOCP;
	Int32UL m_uiThreadCount;
	WorkerManager* m_pWorkerManager;
	JCore::AtomicInt m_iPendingOverlappedCount;

	// 현재 I/O 완료를 대기중인 오버랩 수를 기록한다.
	// IOCP를 종료할 때 이 팬딩 카운트가 0이 되면 IOCP 쓰레드를 해제하도록 한다.
};

NS_JNET_END

