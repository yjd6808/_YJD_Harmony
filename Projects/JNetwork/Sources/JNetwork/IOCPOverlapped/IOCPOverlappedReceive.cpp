/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedReceive.h>
#include <JNetwork/Host/TcpServer.h>

NS_JNET_BEGIN

IOCPOverlappedReceive::IOCPOverlappedReceive(TcpSession* session, IOCP* iocp) :
	IOCPOverlapped(iocp, Type::Receive),
	m_pReceivedSession(session)
{
}

IOCPOverlappedReceive::~IOCPOverlappedReceive() = default;

void IOCPOverlappedReceive::Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hReceiveSock = m_pReceivedSession->Socket().Handle();

	if (IsFailed(hReceiveSock, result, numberOfBytesTransffered) || numberOfBytesTransffered == 0) {
		m_pReceivedSession->Disconnect();
		return;
	}

	m_pReceivedSession->Received(numberOfBytesTransffered);

	if (m_pReceivedSession->ReceiveAsync() == false) {
		m_pReceivedSession->Disconnect();
	}
}

NS_JNET_END