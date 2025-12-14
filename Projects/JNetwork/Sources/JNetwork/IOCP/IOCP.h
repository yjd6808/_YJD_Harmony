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
		Initialized,
		Running,
		Joined,
		Destroyed
	};

public:
	IOCP(int _threadCount);
	virtual ~IOCP();

	void Destroy();
	void Run();
	void Join();

	void AddPendingCount() { ++pendingOverlappedCount_; }
	void DecreasePendingCount() { --pendingOverlappedCount_; }
	int GetPendingCount() { return pendingOverlappedCount_; }
	void WaitForZeroPending();
	JCore::Vector<Int32U> GetWorkThreadIdList();
	State GetState() const { return state_; }

	bool Connect(WinHandle _handle, ULONG_PTR _completionKey) const;
	BOOL GetStatus(Int32UL* _pNumberOfBytesTransffered, PULONG_PTR _pCompletionKey, LPOVERLAPPED* _ppOverlapped) const;
	BOOL Post(Int32UL _numberOfBytesTransferred, ULONG_PTR _completionKey, LPOVERLAPPED _pOverlapped) const;

	void SetName(const JCore::String& _name);
	const JCore::String& GetName() const { return name_; }

	static constexpr const char* TypeName() { return "IOCP"; }

protected:
	State state_;
	WinHandle iocpHandle_;
	Int32UL threadCount_;
	WorkerGroup* workerManager_;
	JCore::AtomicInt pendingOverlappedCount_; // TODO: IOCP에서 팬딩 카운트를 기록하면 경합이 심하지 않을까?
	JCore::NormalLock workerManagerLock_;
	JCore::String name_;
	// 현재 I/O 완료를 대기중인 오버랩 수를 기록한다.
	// IOCP를 종료할 때 이 팬딩 카운트가 0이 되면 IOCP 쓰레드를 해제하도록 한다.
};

using IOCPPtr = JCore::SharedPtr<IOCP>;
using IOCPWPtr = JCore::WeakPtr<IOCP>;

NS_JNET_END
