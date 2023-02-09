/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedAccept.h>

NS_JNET_BEGIN

TcpServer::TcpServer(
	const IOCPPtr& iocp, 
	TcpServerEventListener* eventListener, 
	int sessionRecvBufferSize, 
	int sessionSendBufferSize, 
	int maxConn
)
	: Server(iocp)
	, m_pEventListener(eventListener)
	, m_iSessionRecvBufferSize(sessionRecvBufferSize)
	, m_iSessionSendBufferSize(sessionSendBufferSize)
	, m_pContainer(new SessionContainer(maxConn))
{
	TcpServer::Initialize();
}

TcpServer::~TcpServer() {
	TcpServer::Stop();
	DeleteSafe(m_pContainer);
}


void TcpServer::SessionDisconnected(TcpSession* session) {
	m_pEventListener->OnDisconnected(session);
}

void TcpServer::SessionConnected(TcpSession* session) {
	m_pEventListener->OnConnected(session);
}

void TcpServer::SessionSent(TcpSession* session, ISendPacket* sentPacket, Int32UL receivedBytes) {
	m_pEventListener->OnSent(session, sentPacket, receivedBytes);
}

void TcpServer::SessionReceived(TcpSession* session, ICommand* command) {
	m_pEventListener->OnReceived(session, command);
}

void TcpServer::Initialize() {

	if (CreateSocket(TransportProtocol::TCP) == false) {
		DebugAssertMsg(false, "TCP 서버 소켓 생성 실패");
	}

	if (ConnectIocp() == false) {
		DebugAssertMsg(false, "TCP 서버 IOCP 연결 실패");
	}

	m_eState = eInitailized;
}



bool TcpServer::Start(const IPv4EndPoint& localEndPoint) {
	if (m_eState != eInitailized) {
		DebugAssertMsg(false, "서버가 초기화 상태여야 시작할 수 있습니다.");
		return false;
	}
	

	if (m_Socket.Option().SetReuseAddrEnabled(true) == SOCKET_ERROR) {
		DebugAssertMsg(false, "서버 소켓 SetReuseAddrEnabled(true) 실패");
	}

	if (m_Socket.Bind(localEndPoint) == SOCKET_ERROR) {
		DebugAssertMsg(false, "서버 소켓 Bind 실패");
		return false;
	}

	if (m_Socket.Listen() == SOCKET_ERROR) {
		DebugAssertMsg(false, "서버 소켓 Listen 실패");
		return false;
	}

	// 세션을 미리 생성해놓고 연결 대기 상태로 둠
	for (int i = 0; i < MaxConnection(); i++) {
		TcpSession* session = new TcpSession(this, m_spIocp, m_iSessionRecvBufferSize, m_iSessionSendBufferSize);

		session->AcceptWait();

		if (!session->AcceptAsync()) {
			break;
		}
		m_pContainer->AddSession(session);
	}


	m_pEventListener->OnStarted();
	return bool(m_eState = eListening);
}


bool TcpServer::Stop() {

	if (m_eState == eStopped)
		return true;

	// 강종 진행: GetQueuedCompletionStatus에서 995번에러를 뱉음(I / O operation has been aborted)
	m_pContainer->DisconnectAllSessions();

	if (m_Socket.Close() == SOCKET_ERROR) {
		DebugAssertMsg(false, "서버 소켓을 닫는데 실패하였습니다.");
		return false;
	}
	m_Socket.Invalidate();

	// 동적할당된 세션들을 모두 해제해주자.
	m_pContainer->Clear();

	m_eState = eStopped;
	m_pEventListener->OnStopped();
	return true;
}



NS_JNET_END
