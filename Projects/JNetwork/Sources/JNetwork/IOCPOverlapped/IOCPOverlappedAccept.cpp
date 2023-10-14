/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedAccept.h>
#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/Winsock.h>

NS_JNET_BEGIN

IOCPOverlappedAccept::IOCPOverlappedAccept(TcpSession* session, IOCP* iocp) :
	IOCPOverlapped(iocp, Type::Accept),
	m_pAcceptedSession(session)
{
	m_pAcceptedSession->AddPendingCount();
}

IOCPOverlappedAccept::~IOCPOverlappedAccept() {
	m_pAcceptedSession->DecreasePendingCount();
}

void IOCPOverlappedAccept::Process(BOOL result, Int32UL bytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hAcceptedSock = m_pAcceptedSession->SocketHandle();
	Int32U uiErrorCode = 0;
	if (IsFailed(hAcceptedSock, result, bytesTransffered, uiErrorCode)) {
		m_pAcceptedSession->Disconnect();
		m_pAcceptedSession->ConnectFailed(uiErrorCode);
		return;
	}

	if (m_pAcceptedSession->Accepted(bytesTransffered)) {

		// https://learn.microsoft.com/en-us/windows/win32/fileio/createiocompletionport
		if (m_pAcceptedSession->ConnectIocp() == false) {
			m_pAcceptedSession->Disconnect();
			m_pAcceptedSession->ConnectFailed(::GetLastError());		// 오류코드: CreateIoCompletionPort() 함수 실패시
			return;
		}

		if (m_pAcceptedSession->RecvAsync() == false) {
			m_pAcceptedSession->Disconnect();
			m_pAcceptedSession->ConnectFailed(Winsock::LastError());	// 오류코드: WSARecv() 함수 실패시
			return;
		}

		m_pAcceptedSession->Connected();
	} else {
		uiErrorCode = Winsock::LastError();	// Disconnect시에 오류코드가 덮어질 우려가 있으므로 미리받음.

		m_pAcceptedSession->Disconnect();
		m_pAcceptedSession->ConnectFailed(uiErrorCode);	// 오류코드: SetUpdateAcceptContext 실패시
	}
}

NS_JNET_END