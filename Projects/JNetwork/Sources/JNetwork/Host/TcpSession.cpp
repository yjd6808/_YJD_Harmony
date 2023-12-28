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
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSendTo.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecvFrom.h>

NS_JNET_BEGIN

TcpSession::TcpSession(
	TcpServer* server, 
	const IOCPPtr& iocp, 
	const JCore::MemoryPoolAbstractPtr& bufferAllocator,
	PacketParser* parser,
	int recvBufferSize, 
	int sendBufferSize
)
	: Session(iocp, bufferAllocator, parser, recvBufferSize, sendBufferSize)
	, m_pServer(server)
{
	TcpSession::Initialize();
}

void TcpSession::Initialize() {
	Session::Initialize();


	if (CreateSocket(TransportProtocol::TCP, NonblokingSocket) == false) {
		DebugAssertMsg(false, "TCP 소켓 생성에 실패했습니다. (%u)", Winsock::LastError());
	}
}


bool TcpSession::AcceptAsync() {
	Int32UL receivedBytes = 0;

	SOCKET hListeningSock = m_pServer->SocketHandle();
	IOCPOverlapped* pOverlapped = dbg_new IOCPOverlappedAccept(this, m_spIocp.GetPtr());


	if (m_Socket.AcceptEx(
		hListeningSock,
		m_spRecvBuffer->GetRemainBuffer().buf,
#if TEST_DUMMY_PACKET_TRANSFER
		TEST_DUMMY_PACKET_SIZE,	// TcpClient에서 테스트 더미 패킷을 보내기 땜에 8로 세팅
#else
		0,
#endif
		&receivedBytes,
		pOverlapped
	) == FALSE) {

		Int32U uiError = Winsock::LastError();
		if (uiError != WSA_IO_PENDING) {
			_NetLogWarn_("세션 AcceptEx 실패 (%d:%s)", uiError, Winsock::LastErrorMessageUTF8().Source());
			pOverlapped->Release();
			return false;
		}
	}

	return true;
}

void TcpSession::AcceptWait() {
	m_eState = State::eAcceptWait;
}

bool TcpSession::Accepted(Int32UL receivedBytes) {
	
	// AcceptEx 함수  호출 후 연결된 소켓에 대해서 로컬 주소와 리모트 주소를 가져올 수 있도록 업데이트 해준다.
	// 이걸 실행하지 않으면 해당 소켓에 바인딩된 로컬 주소와 리모트 주소를 못가져옴
	//    = getsockname(), getpeername() 안먹힘
	if (m_Socket.Option().SetUpdateAcceptContext(m_pServer->SocketHandle()) == SOCKET_ERROR) {
		return false;
	}

	char* pReads = m_spRecvBuffer->Peek<char*>();
#if TEST_DUMMY_PACKET_TRANSFER
	m_Socket.AcceptExResult(pReads, TEST_DUMMY_PACKET_SIZE, &m_LocalEndPoint, &m_RemoteEndPoint);
#else
	m_Socket.AcceptExResult(pReads, 0, &m_LocalEndPoint, &m_RemoteEndPoint);
#endif

	return true;
}

void TcpSession::NotifyCommand(ICommand* cmd) {
	m_pServer->SessionReceived(this, cmd);
}

void TcpSession::NotifyPacket(RecvedCommandPacket* recvPacket) {
	m_pServer->SessionReceived(this, recvPacket);
}

void TcpSession::NotifyRaw(char* data, int len) {
	m_pServer->SessionReceivedRaw(this, data, len);
}

void TcpSession::Connected() {
	m_eState = State::eAccepted;
	OnConnected();
	m_pServer->SessionConnected(this);
}

void TcpSession::ConnectFailed(Int32U errorCode) {
	m_pServer->SessionConnectFailed(this, errorCode);
}

void TcpSession::Disconnected(Int32U errorCode) {
	OnDisconnected();
	m_pServer->SessionDisconnected(this, errorCode);
}

void TcpSession::Sent(IPacket* sentPacket, Int32UL sentBytes) {
	m_pServer->SessionSent(this, sentPacket, sentBytes);
}

NS_JNET_END
