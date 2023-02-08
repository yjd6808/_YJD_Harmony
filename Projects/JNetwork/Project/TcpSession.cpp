/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 11:26:19 AM
 * =====================
 *
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/Host/TcpServer.h>


#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecv.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedAccept.h>

NS_JNET_BEGIN

TcpSession::TcpSession(TcpServer* server, const IOCPPtr &iocp, int recvBufferSize, int sendBufferSize)
	: Session(iocp, recvBufferSize, sendBufferSize)
	, m_pServer(server)
{
	if (!Winsock::IsInitialized()) {
		DebugAssertMsg(false, "윈속 초기화를 먼저 해주세요. Winsock::Initialize()");
	}

	if (CreateSocket(TransportProtocol::TCP) == false) {
		DebugAssertMsg(false, "TCP 소켓 생성에 실패했습니다.");
	}
}

bool TcpSession::AcceptAsync() {
	Int32UL receivedBytes = 0;

	SOCKET hListeningSock = m_pServer->Socket().Handle();
	IOCPOverlapped* pOverlapped = new IOCPOverlappedAccept(this, m_spIocp.GetPtr());

	if (m_Socket.AcceptEx(
		hListeningSock,
		m_pRecvBuffer->GetRemainBuffer().buf,
		TEST_DUMMY_PACKET_SIZE,	// TcpClient에서 테스트 더미 패킷을 보내기 땜에 8로 세팅
		&receivedBytes,
		pOverlapped
	) == FALSE) {

		Int32U uiError = Winsock::LastError();
		if (uiError != WSA_IO_PENDING) {
			DebugAssertMsg(false, "세션 AcceptEx 실패 (%d:%s)", uiError, Winsock::LastErrorMessage().Source());
			pOverlapped->Release();
			return false;
		}
	}

	return true;
}

void TcpSession::AcceptWait() {
	m_eSessionState = SessionState::eAcceptWait;
}

bool TcpSession::Accepted(Int32UL receivedBytes) {
	char* pReads = m_pRecvBuffer->Peek<char*>();
	// AcceptEx 함수  호출 후 연결된 소켓에 대해서 로컬 주소와 리모트 주소를 가져올 수 있도록 업데이트 해준다.
	// 이걸 실행하지 않으면 해당 소켓에 바인딩된 로컬 주소와 리모트 주소를 못가져옴
	//    = getsockname(), getpeername() 안먹힘
	if (m_Socket.Option().SetUpdateAcceptContext(m_pServer->Socket().Handle()) == SOCKET_ERROR) {
		return false;
	}

	m_Socket.AcceptExResult(pReads, TEST_DUMMY_PACKET_SIZE, &m_LocalEndPoint, &m_RemoteEndPoint);
	m_eSessionState = SessionState::eAccepted;
	m_pServer->SessionAccepted(this);
	return true;
}

void TcpSession::NotifyCommand(ICommand* cmd) {
	m_pServer->SessionReceived(this, cmd);
}

void TcpSession::Connected() {
	DebugAssertMsg(false, "또잉? 이게 왜 실행되 기뉴 특전대");
}


void TcpSession::Disconnected() {
	m_pServer->SessionDisconnected(this);
}

void TcpSession::Sent(ISendPacket* sentPacket, Int32UL sentBytes) {
	m_pServer->SessionSent(this, sentPacket, sentBytes);
}

NS_JNET_END
