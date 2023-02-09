/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedConnect.h>
#include <JNetwork/Packet/SendPacket.h>
#include <JNetwork/Host/TcpServer.h>

NS_JNET_BEGIN

IOCPOverlappedConnect::IOCPOverlappedConnect(TcpClient* client, IOCP* iocp, ISendPacket* sentPacket) :
	IOCPOverlapped(iocp, Type::Connect),
	m_pConnectedSession(client),
	m_pSentPacket(sentPacket)
{
	NetLog("Connect 오버랩피트 생성 (%d)\n", m_pIocp->GetPendingCount());
}

IOCPOverlappedConnect::~IOCPOverlappedConnect() {
	NetLog("Connect 오버랩피트 소멸 (%d)\n", m_pIocp->GetPendingCount());
}

void IOCPOverlappedConnect::Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hConnectedSock = m_pConnectedSession->SocketHandle();

	if (IsFailed(hConnectedSock, result, numberOfBytesTransffered)) {
		m_pConnectedSession->Disconnect();
		m_pSentPacket->Release();
		return;
	}

	m_pConnectedSession->Connected();

	if (m_pSentPacket) {
		m_pConnectedSession->Sent(m_pSentPacket, numberOfBytesTransffered);
		m_pSentPacket->Release();
	}

	if (m_pConnectedSession->RecvAsync() == false) {
		m_pConnectedSession->Disconnect();
	}
}

NS_JNET_END