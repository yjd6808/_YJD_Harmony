/*
	작성자 : 윤정도
	
*/

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedAccept.h>
#include <JNetwork/Host/TcpServer.h>

namespace JNetwork {

IOCPOverlappedAccept::IOCPOverlappedAccept(TcpServer* tcpServer, TcpSession* session, IOCP* iocp) :
	m_pListeningServer(tcpServer),
	m_pAcceptedSession(session),
	IOCPOverlapped(iocp, Type::Accept)
{
}

IOCPOverlappedAccept::~IOCPOverlappedAccept() {

}

void IOCPOverlappedAccept::Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	SOCKET hAcceptedSock = m_pAcceptedSession->Socket().Handle();
	SOCKET hListeningSock = m_pListeningServer->Socket().Handle();

	if (IsFailed(hAcceptedSock, result, numberOfBytesTransffered)) {
		m_pAcceptedSession->Disconnect();
		return;
	}

	if (m_pAcceptedSession->Accepted(hListeningSock, numberOfBytesTransffered)) {
		if (m_pIocp->Connect(reinterpret_cast<HANDLE>(hAcceptedSock), 0) == false || m_pAcceptedSession->ReceiveAsync() == false) {
			m_pAcceptedSession->Disconnect();
		}
		return;
	}

	m_pAcceptedSession->Disconnect();
}

} // namespace JNetwork