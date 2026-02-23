/*
 * 작성자 : 윤정도
 */

#pragma once

#include <jnet/WorkerGroup.h>


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
	jc::Vector<_u32> GetWorkThreadIdList();
	State GetState() const { return state_; }

	bool Connect(_whandle _handle, ULONG_PTR _completionKey) const;
	BOOL GetStatus(_u32l* _pNumberOfBytesTransffered, PULONG_PTR _pCompletionKey, LPOVERLAPPED* _ppOverlapped) const;
	BOOL Post(_u32l _numberOfBytesTransferred, ULONG_PTR _completionKey, LPOVERLAPPED _pOverlapped) const;

	void SetName(const jc::String& _name);
	const jc::String& GetName() const { return name_; }

	static constexpr const char* TypeName() { return "IOCP"; }

protected:
	State state_;
	_whandle iocpHandle_;
	_u32l threadCount_;
	WorkerGroup* workerManager_;
	jc::AtomicInt pendingOverlappedCount_; // TODO: IOCP에서 팬딩 카운트를 기록하면 경합이 심하지 않을까?
	jc::NormalLock workerManagerLock_;
	jc::String name_;
	// 현재 I/O 완료를 대기중인 오버랩 수를 기록한다.
	// IOCP를 종료할 때 이 팬딩 카운트가 0이 되면 IOCP 쓰레드를 해제하도록 한다.
};

using IOCPPtr = jc::SharedPtr<IOCP>;
using IOCPWPtr = jc::WeakPtr<IOCP>;

NS_END
