/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecv.h>
#include <JNetwork/Host/TcpServer.h>

NS_JNET_BEGIN

IOCPOverlappedRecv::IOCPOverlappedRecv(Session* session, IOCP* iocp) :
	IOCPOverlapped(iocp, Type::Receive),
	m_pReceivedSession(session)
{
}



void IOCPOverlappedRecv::Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hReceiveSock = m_pReceivedSession->Socket().Handle();

	if (IsFailed(hReceiveSock, result, numberOfBytesTransffered) || numberOfBytesTransffered == 0) {
		m_pReceivedSession->Disconnect();
		return;
	}

	m_pReceivedSession->Received(numberOfBytesTransffered);

	if (m_pReceivedSession->RecvAsync() == false) {
		m_pReceivedSession->Disconnect();
	}
}

NS_JNET_END