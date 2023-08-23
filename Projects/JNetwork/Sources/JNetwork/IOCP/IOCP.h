/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JNetwork/WorkerGroup.h>


#define IOCP_POST_ORDER_TERMINATE	0x01
#define IOCP_POST_ORDER_ERROR	   -0x01

NS_JNET_BEGIN

class IOCPWorker;
class IOCP
{
public:
	enum class State
	{
		eInitialized,
		eRunning,
		eJoined,
		eDestroyed
	};

public:
	IOCP(int threadCount);
	virtual ~IOCP();

	void Destroy();
	void Run();
	void Join();

	void	AddPendingCount()				{ ++m_iPendingOverlappedCount;}
	void	DecreasePendingCount()			{ --m_iPendingOverlappedCount;}
	int		GetPendingCount()				{ return m_iPendingOverlappedCount;}
	void	WaitForZeroPending();
	JCore::Vector<Int32U> GetWorkThreadIdList();

	bool Connect(WinHandle handle, ULONG_PTR completionKey) const;
	BOOL GetStatus(Int32UL* numberOfBytesTransffered, PULONG_PTR completionKey, LPOVERLAPPED* ppOverlapped) const;
	BOOL Post(Int32UL dwNumberOfBytesTransferred, ULONG_PTR dwCompletionKey, LPOVERLAPPED pOverlapped) const;

	void SetName(const JCore::String& name);
	const JCore::String& GetName() const { return m_szName; }

	static constexpr const char* TypeName() { return "IOCP"; }
protected:
	State m_eState;
	WinHandle m_hIOCP;
	Int32UL m_uiThreadCount;
	WorkerGroup* m_pWorkerManager;
	JCore::AtomicInt m_iPendingOverlappedCount;		// TODO: IOCP에서 팬딩 카운트를 기록하면 경합이 심하지 않을까?
	JCore::NormalLock m_WorkerManagerLock;
	JCore::String m_szName;
	// 현재 I/O 완료를 대기중인 오버랩 수를 기록한다.
	// IOCP를 종료할 때 이 팬딩 카운트가 0이 되면 IOCP 쓰레드를 해제하도록 한다.
};

using IOCPPtr = JCore::SharedPtr<IOCP>;
using IOCPWPtr = JCore::WeakPtr<IOCP>;

NS_JNET_END

