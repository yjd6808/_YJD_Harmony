/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 1:41:58 PM
 * =====================
 *
 */

#include <JNetwork/Network.h>
#include <JNetwork/Host/Host.h>

NS_JNET_BEGIN

bool Host::CreateSocket(TransportProtocol protocol) {
	if (m_Socket.IsValid()) {
		m_Socket.Close();
	}

	m_Socket = Socket::CreateV4(protocol, true);

	if (!m_Socket.IsValid())
		return false;

	if (m_Socket.Option().SetNonBlockingEnabled(true) == SOCKET_ERROR) {
		DebugAssertMsg(false, "논블로킹 소켓 전환 실패");
	}

	return true;
}


bool Host::ConnectIocp() {

	if (m_spIocp->Connect(reinterpret_cast<WinHandle>(SocketHandle()), NULL) == false) {
		return false;
	}

	return m_bIocpConneced = true;
}


NS_JNET_END