/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 6:03:54 PM
 * =====================
 *
 */


#pragma once

#include <jnet/Host/Session.h>
#include <jnet/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedSendTo : public IOCPOverlapped
{
public:
	IOCPOverlappedSendTo(Session* _pSender, IOCP* _pIocp, IPacket* _pSentPacket);
	~IOCPOverlappedSendTo() override;

public:
	void Process(BOOL _result, _u32l _bytesTransferred, IOCPPostOrder* _pCompletionKey) override;

private:
	Session* sender_;
	IPacket* sentPacket_;
};

NS_END
