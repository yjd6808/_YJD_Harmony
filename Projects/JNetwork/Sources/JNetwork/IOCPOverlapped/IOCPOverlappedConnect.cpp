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
	m_pClient(client),
	m_pSentPacket(sentPacket)
{
	NetLog("Connect 오버랩피트 생성 (%d)\n", m_pIocp->GetPendingCount());
}

IOCPOverlappedConnect::~IOCPOverlappedConnect() {
	NetLog("Connect 오버랩피트 소멸 (%d)\n", m_pIocp->GetPendingCount());
}

void IOCPOverlappedConnect::Process(BOOL result, Int32UL bytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hConnectedSock = m_pClient->SocketHandle();
	Int32U uiErrorCode = 0;
	if (IsFailed(hConnectedSock, result, bytesTransffered, uiErrorCode)) {
		m_pClient->Disconnect();
		m_pClient->ConnectFailed(uiErrorCode);
		m_pSentPacket->Release();
		return;
	}

	m_pClient->Connected();

	if (m_pSentPacket) {
		m_pClient->Sent(m_pSentPacket, bytesTransffered);
		m_pSentPacket->Release();
	}

	if (m_pClient->RecvAsync() == false) {
		m_pClient->Disconnect();
	}
}

NS_JNET_END