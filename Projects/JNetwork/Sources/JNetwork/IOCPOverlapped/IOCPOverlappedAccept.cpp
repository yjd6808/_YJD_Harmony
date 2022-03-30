/*
	작성자 : 윤정도
	
*/

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedAccept.h>
#include <JNetwork/Host/TcpServer.h>

namespace JNetwork {

IOCPOverlappedAccept::IOCPOverlappedAccept(TcpServer* tcpServer, TcpSession* session, IOCP* iocp) :
	IOCPOverlapped(iocp, Type::Accept),
	m_pAcceptedSession(session),
	m_pListeningServer(tcpServer)
{
}

IOCPOverlappedAccept::~IOCPOverlappedAccept() = default;

void IOCPOverlappedAccept::Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hAcceptedSock = m_pAcceptedSession->Socket().Handle();
	const SOCKET hListeningSock = m_pListeningServer->Socket().Handle();

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