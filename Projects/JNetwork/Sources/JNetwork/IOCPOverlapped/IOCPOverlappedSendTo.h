/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 6:03:54 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedSendTo : public IOCPOverlapped
{
public:
	IOCPOverlappedSendTo(Session* _pSender, IOCP* _pIocp, IPacket* _pSentPacket);
	~IOCPOverlappedSendTo() override;

public:
	void Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey) override;

private:
	Session* sender_;
	IPacket* sentPacket_;
};

NS_JNET_END
