/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSend.h>
#include <JNetwork/Host/TcpServer.h>

NS_JNET_BEGIN

IOCPOverlappedSend::IOCPOverlappedSend(TcpSession* session, IOCP* iocp, ISendPacket* sentPacket) :
	IOCPOverlapped(iocp, Type::Send),
	m_pSentSession(session),
	m_pSentPacket(sentPacket)
{
}

IOCPOverlappedSend::~IOCPOverlappedSend() = default;

void IOCPOverlappedSend::Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hSentSock = m_pSentSession->Socket().Handle();

	if (IsFailed(hSentSock, result, numberOfBytesTransffered) || numberOfBytesTransffered == 0) {
		m_pSentSession->Disconnect();
		return;
	}

	m_pSentSession->Sent(m_pSentPacket, numberOfBytesTransffered);
	m_pSentPacket->Release();
}

NS_JNET_END