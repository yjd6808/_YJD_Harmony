/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSend.h>
#include <JNetwork/Packet/Packet.h>
#include <JNetwork/Host/TcpServer.h>

NS_JNET_BEGIN

IOCPOverlappedSend::IOCPOverlappedSend(Session* session, IOCP* iocp, ISendPacket* sentPacket)
	: IOCPOverlapped(iocp, Type::Send)
	, m_pSender(session)
	, m_pSentPacket(sentPacket)
{
}

IOCPOverlappedSend::~IOCPOverlappedSend() = default;

void IOCPOverlappedSend::Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hSentSock = m_pSender->SocketHandle();

	if (IsFailed(hSentSock, result, numberOfBytesTransffered) || numberOfBytesTransffered == 0) {
		m_pSender->Disconnect();
		return;
	}

	m_pSender->Sent(m_pSentPacket, numberOfBytesTransffered);
	m_pSentPacket->Release();
}

NS_JNET_END