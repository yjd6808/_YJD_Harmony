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
	IOCPOverlappedSendTo(Session* sender, IOCP* iocp, IPacket* sentPacket);
	~IOCPOverlappedSendTo() override;
public:
	void Process(BOOL result, Int32UL bytesTransffered, IOCPPostOrder* completionKey) override;
private:
	Session* m_pSender;
	IPacket* m_pSentPacket;
};

NS_JNET_END
