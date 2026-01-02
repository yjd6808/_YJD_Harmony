/*
	작성자 : 윤정도
*/

#pragma once

#include <jnet/Host/TcpSession.h>
#include <jnet/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedAccept : public IOCPOverlapped
{
public:
	IOCPOverlappedAccept(TcpSession* _pSession, IOCP* _pIocp);
	~IOCPOverlappedAccept() override;

public:
	void Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey) override;

private:
	TcpSession* acceptedSession_;
};

NS_JNET_END
