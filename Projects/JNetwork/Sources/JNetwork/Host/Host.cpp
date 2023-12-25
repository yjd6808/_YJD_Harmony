/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 1:41:58 PM
 * =====================
 *
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Config.h>
#include <JNetwork/Host/Host.h>


NS_JNET_BEGIN

bool Host::CreateSocket(TransportProtocol protocol, bool nonBlocking) {
	if (m_Socket.IsValid()) {
		m_Socket.Close();
	}

	m_Socket = Socket::CreateV4(protocol, true);

	if (!m_Socket.IsValid())
		return false;

	if (m_Socket.Option().SetNonBlockingEnabled(nonBlocking) == SOCKET_ERROR) {
		DebugAssertMsg(false, "논블로킹 소켓 설정 실패 (%u)", Winsock::LastError());
	}

	/*
	 * 처음 소켓을 생성 후 송/수신 버퍼 크기 변경전 Send/Recv 버퍼 크기 모두 65535로 나옴.
	 * 버퍼크기를 수정해주면 내가 변경해준 값으로 올바르게 나온걸 확인했음
	 *
	 * 버퍼링 여부는 JNetwork/Config.h 파일 참고
	 */

	if (DisableSendBuffering && m_Socket.Option().SetSendBufferSize(0) == SOCKET_ERROR) {
		DebugAssertMsg(false, "소켓 %s 버퍼링 비활성화 실패 (%u)", TransmissionName(Transmission::Send), Winsock::LastError());
	}

	if (DisableRecvBuffering && m_Socket.Option().SetRecvBufferSize(0) == SOCKET_ERROR) {
		DebugAssertMsg(false, "소켓 %s 버퍼링 비활성화 실패 (%u)", TransmissionName(Transmission::Recv), Winsock::LastError());
	}


	return true;
}


bool Host::ConnectIocp() {

	if (m_spIocp->Connect(reinterpret_cast<WinHandle>(SocketHandle()), NULL) == false) {
		return false;
	}

	return m_bIocpConnected = true;
}


NS_JNET_END