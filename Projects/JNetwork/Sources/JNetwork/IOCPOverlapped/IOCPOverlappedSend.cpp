/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSend.h>
#include <JNetwork/Host/TcpServer.h>

#include <JCore/Primitives/RefCountObjectPtr.h>

NS_JNET_BEGIN

IOCPOverlappedSend::IOCPOverlappedSend(Session* session, IOCP* iocp, IPacket* sentPacket)
	: IOCPOverlapped(iocp, Type::Send)
	, m_pSender(session)
	, m_pSentPacket(sentPacket)
{
	m_pSender->AddPendingCount();
}

IOCPOverlappedSend::~IOCPOverlappedSend() {
	m_pSender->DecreasePendingCount();
}

void IOCPOverlappedSend::Process(BOOL result, Int32UL bytesTransffered, IOCPPostOrder* completionKey) {
	JCORE_REF_COUNT_GUARD(m_pSentPacket, false);
	const SOCKET hSentSock = m_pSender->SocketHandle();
	Int32U uiErrorCode = 0;

	if (IsFailed(hSentSock, result, bytesTransffered, uiErrorCode) || bytesTransffered == 0) {
		return;
	}

	m_pSender->Sent(m_pSentPacket, bytesTransffered);
}

NS_JNET_END