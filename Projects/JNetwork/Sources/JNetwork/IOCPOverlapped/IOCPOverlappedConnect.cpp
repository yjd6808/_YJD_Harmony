/*
	작성자 : 윤정도
	
*/

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedConnect.h>
#include <JNetwork/Host/TcpServer.h>

namespace JNetwork {

IOCPOverlappedConnect::IOCPOverlappedConnect(TcpSession* session, IOCP* iocp, IPacket* sentPacket) :
	m_pConnectedSession(session),
	m_pSentPacket(sentPacket),
	IOCPOverlapped(iocp, Type::Send)
{
}

IOCPOverlappedConnect::~IOCPOverlappedConnect() {

}

void IOCPOverlappedConnect::Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	SOCKET hConnectedSock = m_pConnectedSession->Socket().Handle();

	if (IsFailed(hConnectedSock, result, numberOfBytesTransffered)) {
		m_pConnectedSession->Disconnect();
		return;
	}

	m_pConnectedSession->Connected();

	if (m_pSentPacket) {
		m_pConnectedSession->Sent(m_pSentPacket, numberOfBytesTransffered);
		m_pSentPacket->Release();
	}
}

} // namespace JNetwork