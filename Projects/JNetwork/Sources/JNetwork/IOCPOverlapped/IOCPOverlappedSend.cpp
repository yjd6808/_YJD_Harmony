/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSend.h>
#include <JNetwork/Packet/SendPacket.h>
#include <JNetwork/Host/TcpServer.h>

NS_JNET_BEGIN

IOCPOverlappedSend::IOCPOverlappedSend(Session* session, IOCP* iocp, ISendPacket* sentPacket)
	: IOCPOverlapped(iocp, Type::Send)
	, m_pSender(session)
	, m_pSentPacket(sentPacket)
{
}

IOCPOverlappedSend::~IOCPOverlappedSend() = default;

void IOCPOverlappedSend::Process(BOOL result, Int32UL bytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hSentSock = m_pSender->SocketHandle();
	Int32U uiErrorCode = 0;

	if (IsFailed(hSentSock, result, bytesTransffered, uiErrorCode) || bytesTransffered == 0) {
		m_pSender->Disconnect();
		return;
	}

	m_pSender->Sent(m_pSentPacket, bytesTransffered);
	m_pSentPacket->Release();
}

NS_JNET_END