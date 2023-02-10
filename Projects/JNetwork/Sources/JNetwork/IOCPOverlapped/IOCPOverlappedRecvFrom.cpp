/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 6:04:33 PM
 * =====================
 *
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecvFrom.h>

NS_JNET_BEGIN


IOCPOverlappedRecvFrom::IOCPOverlappedRecvFrom(UdpClient* client, IOCP* iocp)
	: IOCPOverlapped(iocp, Type::ReceiveFrom)
	, m_pReceiver(client)
{
}

IOCPOverlappedRecvFrom::~IOCPOverlappedRecvFrom() = default;

void IOCPOverlappedRecvFrom::Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) {
	const SOCKET hReceiveSock = m_pReceiver->SocketHandle();

	if (IsFailed(hReceiveSock, result, numberOfBytesTransffered) || numberOfBytesTransffered == 0) {
		return;
	}

	m_pReceiver->Received(numberOfBytesTransffered);

	if (m_pReceiver->RecvFromAsync() == false) {
	}
}




NS_JNET_END
