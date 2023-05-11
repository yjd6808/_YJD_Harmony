/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecv.h>
#include <JNetwork/Host/TcpServer.h>

NS_JNET_BEGIN

IOCPOverlappedRecv::IOCPOverlappedRecv(Session* session, IOCP* iocp)
	: IOCPOverlapped(iocp, Type::Receive)
	, m_pReceivedSession(session)
{
	m_pReceivedSession->AddPendingCount();
}

IOCPOverlappedRecv::~IOCPOverlappedRecv() {
	m_pReceivedSession->DecreasePendingCount();
}


void IOCPOverlappedRecv::Process(BOOL result, Int32UL bytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hReceiveSock = m_pReceivedSession->SocketHandle();
	Int32U uiErrorCode = 0;
	if (IsFailed(hReceiveSock, result, bytesTransffered, uiErrorCode) || bytesTransffered == 0) {
		m_pReceivedSession->Disconnect();
		m_pReceivedSession->Disconnected();
		return;
	}

	m_pReceivedSession->Received(bytesTransffered);

	// TODO: 리시브 오버랩 재사용 기능 구현
	if (m_pReceivedSession->RecvAsync() == false) {
		m_pReceivedSession->Disconnect();
		m_pReceivedSession->Disconnected();
	}
}

NS_JNET_END