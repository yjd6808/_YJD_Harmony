/*
	작성자 : 윤정도
*/

#pragma once

#include "jnet/Host/TcpSession.h"
#include "jnet/IOCPOverlapped/IOCPOverlapped.h"

NS_JNET_BEGIN

class IOCPOverlappedAccept : public IOCPOverlapped
#ifdef USE_OVERLAPPED_STATIC_POOL
	, public jc::ObjectPool<IOCPOverlappedAccept>
#endif
{
public:
	IOCPOverlappedAccept(TcpSession* _pSession, IOCP* _pIocp);
	~IOCPOverlappedAccept() override;

public:
	virtual void Process(BOOL _result, _u32l _bytesTransferred, NULLABLE IOCPPostOrder* _pCompletionKey, IOCPWorker* _pWorker) override;
	virtual void ReleaseAction() override 
	{ 
		delete this; 
	}

private:
	TcpSession* acceptedSession_;
};

NS_END
