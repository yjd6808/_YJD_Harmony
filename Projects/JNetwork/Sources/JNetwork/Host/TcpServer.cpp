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
	const JCore::MemoryPoolAbstractPtr& bufferAllocator,
	ServerEventListener* eventListener,
	int maxConn,
	int sessionRecvBufferSize, 
	int sessionSendBufferSize
)
	: Server(iocp)
	, m_iSessionRecvBufferSize(sessionRecvBufferSize)
	, m_iSessionSendBufferSize(sessionSendBufferSize)
	, m_spBufferAllocator(bufferAllocator)
	, m_pEventListener(eventListener)
	, m_pContainer(dbg_new SessionContainer(maxConn))
{
	TcpServer::Initialize();
}

TcpServer::~TcpServer() {
	TcpServer::Stop();
	DeleteSafe(m_pContainer);
}

TcpSession* TcpServer::CreateSession() {
	return dbg_new TcpSession(this, m_spIocp, m_spBufferAllocator, m_iSessionRecvBufferSize, m_iSessionSendBufferSize);
}

void TcpServer::SessionDisconnected(TcpSession* session) {
	m_pEventListener->OnDisconnected(session);

	// 세션 재사용... 이거땜에 State를 Atomic으로 변경함.
	// 서버가 다른 쓰레드에서 Stop을 실행하는 순간
	// IOCP 쓰레드들이 서버의 State를 정확하게 관측하도록 하기위함.
	// 이렇게 체크한번만 해주면 쓰레드 세이프하게 재사용할 수 있다. (맞겠지?)
	if (m_eState != eListening) {
		_NetLogDebug_("IOCP 서버가 리스닝 상태가 아닙니다. 세션 재사용을 하지 않습니다.");
		return;
	}

	session->Initialize();
	session->AcceptWait();

	if (!session->AcceptAsync()) {
		_NetLogDebug_("세션을 재사용 실패");
	}
}

void TcpServer::SessionConnected(TcpSession* session) {
	m_pEventListener->OnConnected(session);
}

void TcpServer::SessionConnectFailed(TcpSession* session, Int32U errorCode) {
	m_pEventListener->OnConnectFailed(session, errorCode);
}

void TcpServer::SessionSent(TcpSession* session, ISendPacket* sentPacket, Int32UL receivedBytes) {
	m_pEventListener->OnSent(session, sentPacket, receivedBytes);
}

void TcpServer::SessionReceived(TcpSession* session, ICommand* command) {
	m_pEventListener->OnReceived(session, command);
}

void TcpServer::BroadcastAsync(ISendPacket* packet) {
	if (m_eState != eListening) {
		_NetLogError_("리스닝 상태가 아니므로 브로드캐스트를 수행할 수 없습니다.");
		return;
	}

	packet->AddRef();

	m_pContainer->ForEachConnected([packet](Session* session) {
		session->SendAsync(packet);
	});

	packet->Release();
}

void TcpServer::Initialize() {

	if (CreateSocket(TransportProtocol::TCP) == false) {
		_NetLogError_("TCP 서버 소켓 생성 실패");
		return;
	}

	if (ConnectIocp() == false) {
		_NetLogError_("TCP 서버 IOCP 연결 실패");
		return;
	}

	m_eState = eInitailized;
}



bool TcpServer::Start(const IPv4EndPoint& localEndPoint) {
	if (m_eState != eInitailized) {
		_NetLogError_("서버가 초기화 상태여야 시작할 수 있습니다.");
		return false;
	}
	
	if (m_Socket.Option().SetReuseAddrEnabled(true) == SOCKET_ERROR) {
		_NetLogWarn_("서버 소켓 SetReuseAddrEnabled(true) 실패");
	}

	if (m_Socket.Bind(localEndPoint) == SOCKET_ERROR) {
		_NetLogError_("서버 소켓 Bind 실패 (%d)", Winsock::LastError());
		return false;
	}

	if (m_Socket.Listen() == SOCKET_ERROR) {
		_NetLogError_("서버 소켓 리슨 실패 (%d)", Winsock::LastError());
		return false;
	}

	const int iMaxConn = m_pContainer->MaxConnection();

	// 세션을 미리 생성해놓고 연결 대기 상태로 둠
	for (int i = 0; i < iMaxConn; i++) {
		TcpSession* session = CreateSession(); 

		session->AcceptWait();

		if (!session->AcceptAsync()) {
			m_pContainer->DisconnectAllSessions();
			m_pContainer->Clear();
			return false;
		}
		m_pContainer->AddSession(session);
	}


	m_pEventListener->OnStarted();
	return bool(m_eState = eListening);
}


bool TcpServer::Stop() {
	
	if (m_eState == eStopped)
		return true;

	_NetLogDebug_("서버를 정지합니다.");

	// 제일먼저 세팅해줘야한다.
	m_eState = eStopped;

	// 강종 진행: GetQueuedCompletionStatus에서 995번에러를 뱉음(I / O operation has been aborted)
	m_pContainer->DisconnectAllSessions();

	if (m_Socket.Close() == SOCKET_ERROR) {
		_NetLogError_("서버 소켓을 닫는데 실패했습니다. (%d)", Winsock::LastError());
	}

	m_Socket.Invalidate();

	// 동적할당된 세션들을 모두 해제해주자.
	m_pContainer->Clear();
	m_pEventListener->OnStopped();
	return true;
}



NS_JNET_END
