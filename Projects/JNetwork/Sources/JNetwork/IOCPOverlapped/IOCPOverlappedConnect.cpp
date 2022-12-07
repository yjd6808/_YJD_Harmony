/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedConnect.h>
#include <JNetwork/Host/TcpServer.h>

namespace JNetwork {

IOCPOverlappedConnect::IOCPOverlappedConnect(TcpSession* session, IOCP* iocp, ISendPacket* sentPacket) :
	IOCPOverlapped(iocp, Type::Send),
	m_pConnectedSession(session),
	m_pSentPacket(sentPacket)
{
}

IOCPOverlappedConnect::~IOCPOverlappedConnect() = default;

void IOCPOverlappedConnect::Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hConnectedSock = m_pConnectedSession->Socket().Handle();

	if (IsFailed(hConnectedSock, result, numberOfBytesTransffered)) {
		m_pConnectedSession->Disconnect();
		return;
	}

	m_pConnectedSession->Connected();

	if (m_pSentPacket) {
		m_pConnectedSession->Sent(m_pSentPacket, numberOfBytesTransffered);
		m_pSentPacket->Release();
	}

	if (m_pConnectedSession->ReceiveAsync() == false) {
		m_pConnectedSession->Disconnect();
	}
}

} // namespace JNetwork