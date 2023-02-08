/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 6:04:09 PM
 * =====================
 *
 */




#include <JNetwork/Network.h>
#include <JNetwork/Packet/Packet.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSendTo.h>

NS_JNET_BEGIN

IOCPOverlappedSendTo::IOCPOverlappedSendTo(UdpClient* sender, IOCP* iocp, ISendPacket* sentPacket)
	: IOCPOverlapped(iocp, Type::SendTo)
	, m_pSender(sender)
	, m_pSentPacket(sentPacket)
{
}

IOCPOverlappedSendTo::~IOCPOverlappedSendTo() = default;

void IOCPOverlappedSendTo::Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hSentSock = m_pSender->Socket().Handle();

	if (IsFailed(hSentSock, result, numberOfBytesTransffered) || numberOfBytesTransffered == 0) {
		return;
	}

	m_pSender->Sent(m_pSentPacket, numberOfBytesTransffered);
	m_pSentPacket->Release();
}

NS_JNET_END