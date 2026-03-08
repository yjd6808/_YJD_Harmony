/*
 * 작성자 : 윤정도
 */

#pragma once

#include <jnet/Host/Session.h>
#include <jnet/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedRecv : public IOCPOverlapped
#ifdef USE_OVERLAPPED_STATIC_POOL
	, public jc::ObjectPool<IOCPOverlappedRecv>
#endif
{
public:
	IOCPOverlappedRecv(Session* _pSession, IOCP* _pIocp);
	~IOCPOverlappedRecv() override;

public:
	void Process(BOOL _result, _u32l _bytesTransferred, NULLABLE IOCPPostOrder* _pCompletionKey, IOCPWorker* _pWorker) override;

private:
	Session* receivedSession_;
};

NS_END
