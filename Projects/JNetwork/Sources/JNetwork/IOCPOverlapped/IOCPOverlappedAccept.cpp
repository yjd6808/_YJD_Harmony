/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedAccept.h>
#include <JNetwork/Host/TcpServer.h>

NS_JNET_BEGIN

IOCPOverlappedAccept::IOCPOverlappedAccept(TcpSession* session, IOCP* iocp) :
	IOCPOverlapped(iocp, Type::Accept),
	m_pAcceptedSession(session)
{
	m_pAcceptedSession->AddPendingCount();
	NetLog("Accept 오버랩피트 생성 (%d)\n", m_pIocp->GetPendingCount());
}

IOCPOverlappedAccept::~IOCPOverlappedAccept() {
	m_pAcceptedSession->DecreasePendingCount();
	NetLog("Accept 오버랩피트 소멸 (%d)\n", m_pIocp->GetPendingCount());
}

void IOCPOverlappedAccept::Process(BOOL result, Int32UL bytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hAcceptedSock = m_pAcceptedSession->SocketHandle();
	Int32U uiErrorCode = 0;
	if (IsFailed(hAcceptedSock, result, bytesTransffered, uiErrorCode)) {
		m_pAcceptedSession->Disconnect();
		return;
	}

	if (m_pAcceptedSession->Accepted(bytesTransffered)) {
		if (m_pAcceptedSession->ConnectIocp() == false || m_pAcceptedSession->RecvAsync() == false) {
			m_pAcceptedSession->Disconnect();
		}
		return;
	}

	m_pAcceptedSession->Disconnect();
}

NS_JNET_END