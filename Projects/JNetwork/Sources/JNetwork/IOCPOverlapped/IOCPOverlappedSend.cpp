/*
	작성자 : 윤정도
*/

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSend.h>
#include <JNetwork/Host/TcpServer.h>

namespace JNetwork {

IOCPOverlappedSend::IOCPOverlappedSend(TcpSession* session, IOCP* iocp, IPacket* sentPacket) :
	m_pSentSession(session),
	m_pSentPacket(sentPacket),
	IOCPOverlapped(iocp, Type::Send)
{
}

IOCPOverlappedSend::~IOCPOverlappedSend() {

}

void IOCPOverlappedSend::Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	SOCKET hSentSock = m_pSentSession->Socket().Handle();

	if (IsFailed(hSentSock, result, numberOfBytesTransffered) || numberOfBytesTransffered == 0) {
		m_pSentSession->Disconnect();
		return;
	}

	m_pSentSession->Sent(m_pSentPacket, numberOfBytesTransffered);
	m_pSentPacket->Release();
}

} // namespace JNetwork