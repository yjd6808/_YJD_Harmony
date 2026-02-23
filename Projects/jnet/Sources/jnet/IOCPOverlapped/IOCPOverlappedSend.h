/*
 * 작성자 : 윤정도
 */

#pragma once


#include <jnet/Host/Session.h>
#include <jnet/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedSend : public IOCPOverlapped
{
public:
	IOCPOverlappedSend(Session* _pSession, IOCP* _pIocp, IPacket* _pSentPacket);
	~IOCPOverlappedSend() override;

public:
	void Process(BOOL _result, _u32l _bytesTransferred, IOCPPostOrder* _pCompletionKey) override;

private:
	Session* sender_;
	IPacket* sentPacket_;
};

NS_END
