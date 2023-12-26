/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/Host/SessionContainer.h>

#include <JNetwork/IOCPOverlapped/IOCPOverlappedAccept.h>

#include <JCore/Primitives/RefCountObjectPtr.h>

NS_JNET_BEGIN

TcpServer::TcpServer(const IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator)
	: Server(iocp)
	, m_spBufferAllocator(bufferAllocator)
	, m_pEventListener(nullptr)
	, m_pContainer(nullptr)
{
	TcpServer::Initialize();
}

TcpServer::~TcpServer() {
	TcpServer::Stop();

	JCORE_DELETE_SAFE(m_pContainer);
	JCORE_DELETE_SAFE(m_pEventListener);
}

TcpSession* TcpServer::CreateSession() {
	return dbg_new TcpSession(this, m_spIocp, m_spBufferAllocator, nullptr, 0, 0);
}

// 디폴트 세션 컨테이너, 서버시작전 외부에서 주입해줄 경우 호출안됨
ISessionContainer* TcpServer::CreateSessionContainer() {
	return dbg_new SessionContainer(10);
}

void TcpServer::SessionDisconnected(TcpSession* session) {
	if (m_pEventListener)
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
	if (m_pEventListener)
		m_pEventListener->OnConnected(session);
}

void TcpServer::SessionConnectFailed(TcpSession* session, Int32U errorCode) {
	if (m_pEventListener)
		m_pEventListener->OnConnectFailed(session, errorCode);
}

void TcpServer::SessionSent(TcpSession* session, IPacket* sentPacket, Int32UL receivedBytes) {
	if (m_pEventListener)
		m_pEventListener->OnSent(session, sentPacket, receivedBytes);
}

void TcpServer::SessionReceived(TcpSession* session, ICommand* command) {
	if (m_pEventListener)
		m_pEventListener->OnReceived(session, command);
}

void TcpServer::SessionReceived(TcpSession* session, RecvedCommandPacket* recvPacket) {
	if (m_pEventListener)
		m_pEventListener->OnReceived(session, recvPacket);
}

void TcpServer::SessionReceivedRaw(TcpSession* session, char* data, int len) {
	if (m_pEventListener)
		m_pEventListener->OnReceivedRaw(session, data, len);
}

ISessionContainer* TcpServer::GetSessionContainer() {
	JCORE_LOCK_GUARD(m_Sync);
	return m_pContainer;
}

ServerEventListener* TcpServer::GetEventListener() {
	JCORE_LOCK_GUARD(m_Sync);
	return m_pEventListener;
}

void TcpServer::SetSesssionContainer(ISessionContainer* container) {
	JCORE_LOCK_GUARD(m_Sync);
	m_pContainer = container;
}

void TcpServer::SetEventListener(ServerEventListener* listener) {
	JCORE_LOCK_GUARD(m_Sync);
	m_pEventListener = listener;
}

void TcpServer::Initialize() {
	JCORE_LOCK_GUARD(m_Sync);

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

	struct StartFailNotifyGuard
	{
		StartFailNotifyGuard(TcpServer* server) : Server(server) {}
		~StartFailNotifyGuard() noexcept {
			if (ErrorCode == 0)
				return;

			if (Server->m_pEventListener)
				Server->m_pEventListener->OnStartFailed(ErrorCode);

			Server->OnStartFailed(ErrorCode);
		}

		Int32U ErrorCode = 0;
		TcpServer* Server;
	} Notifier{this};

	JCORE_LOCK_GUARD(m_Sync);

	if (m_eState != eInitailized) {
		_NetLogError_("서버가 초기화 상태여야 시작할 수 있습니다.");
		Notifier.ErrorCode = WSANOTINITIALISED;
		return false;
	}
	
	if (m_Socket.Option().SetReuseAddrEnabled(true) == SOCKET_ERROR) {
		_NetLogWarn_("서버 소켓 SetReuseAddrEnabled(true) 실패");
	}

	if (m_Socket.Bind(localEndPoint) == SOCKET_ERROR) {
		_NetLogError_("%s %s %s 바인드 실패 (%u)", TypeName(), localEndPoint.ToString().Source(), m_Socket.ProtocolName(), Winsock::LastError());
		Notifier.ErrorCode = Winsock::LastError();
		return false;
	}
	_NetLogDebug_("%s %s %s 바인드 완료", TypeName(), localEndPoint.ToString().Source(), m_Socket.ProtocolName());

	if (m_Socket.Listen() == SOCKET_ERROR) {
		_NetLogError_("서버 소켓 리슨 실패 (%d)", Winsock::LastError());
		Notifier.ErrorCode = Winsock::LastError();
		return false;
	}

	if (m_pContainer == nullptr) {
		m_pContainer = CreateSessionContainer();
	}

	m_pContainer->ResetHandleSeq();
	m_pContainer->Clear();
	const int iMaxConn = m_pContainer->Capacity();

	// 세션을 미리 생성해놓고 연결 대기 상태로 둠
	for (int i = 0; i < iMaxConn; i++) {
		TcpSession* session = CreateSession();

		session->OnCreated();
		session->SetHandle(m_pContainer->CreateHandle());
		session->AcceptWait();

		if (!session->AcceptAsync()) {
			Notifier.ErrorCode = Winsock::LastError();
			m_pContainer->DisconnectAll();
			m_pContainer->Clear();
			return false;
		}
		m_pContainer->Add(session);
	}


	if (m_pEventListener)
		m_pEventListener->OnStarted();

	OnStarted();
	return bool(m_eState = eListening);
}


bool TcpServer::Stop() {
	JCORE_LOCK_GUARD(m_Sync);

	if (m_eState == eStopped)
		return true;

	m_eState = eStopped;

	// 강종 진행: GetQueuedCompletionStatus에서 995번에러를 뱉음(I / O operation has been aborted)
	m_pContainer->DisconnectAll();

	if (m_Socket.Close() == SOCKET_ERROR) {
		_NetLogError_("서버 소켓을 닫는데 실패했습니다. (%d)", Winsock::LastError());
	}

	m_Socket.Invalidate();

	// 동적할당된 세션들을 모두 해제해주자.
	m_pContainer->Clear();

	if (m_pEventListener)
		m_pEventListener->OnStopped();

	OnStopped();
	return true;
}



NS_JNET_END
