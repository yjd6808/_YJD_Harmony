/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 6:03:54 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/UdpClient.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

class IOCPOverlappedSendTo : public IOCPOverlapped
{
public:
	IOCPOverlappedSendTo(UdpClient* sender, IOCP* iocp, ISendPacket* sentPacket);
	~IOCPOverlappedSendTo() override;
public:
	void Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) override;
private:
	UdpClient* m_pSender;
	ISendPacket* m_pSentPacket;
};

NS_JNET_END
