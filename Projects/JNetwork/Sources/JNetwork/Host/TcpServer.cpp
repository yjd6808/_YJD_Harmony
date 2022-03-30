#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedAccept.h>



namespace JNetwork {
	TcpServer::TcpServer(int maxConn) :
		m_pEventListener(nullptr),
		m_pContainer(new TcpSessionContainer(maxConn)),
		m_pIocp(new IOCP()) {

	}

	TcpServer::~TcpServer() {
		DeleteSafe(m_pContainer);
		DeleteSafe(m_pIocp);
	}

	int TcpServer::DefaultIocpThreadCount() const {
		// 기본적으로 쓰레드 개수만큼 생성하도록 하자.
		// dwNumberOfProcessors 이름땜에 코어 갯수로 착각하기 쉬운데 CPU 쓰레드 갯수라고 한다.
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		return (int)info.dwNumberOfProcessors; 
	}


	bool TcpServer::Start(const IPv4EndPoint& localEndPoint) {
		if (m_eState == State::Paused || m_eState == State::Running) {
			return false;
		}

		if (!Winsock::IsInitialized()) {
			Winsock::Message("윈속 초기화를 먼저 해주세요. Winsock::Initialize()");
			return false;
		}

		if (m_pEventListener == nullptr) {
			Winsock::AssertWinsockMessage("이벤트 리스너를 설정해주세요.");
			return false;
		}


		m_ServerSocket = Socket::CreateTcpV4(true);

		if (!m_ServerSocket.IsValid()) {
			Winsock::AssertWinsockMessage("서버 소켓 Create 실패");
			return false;
		}

		if (m_ServerSocket.Option().SetReuseAddrEnabled(true) == SOCKET_ERROR) {
			Winsock::AssertWinsockMessage("서버 소켓 SetReuseAddrEnabled(true) 실패");
		}

		if (m_ServerSocket.Bind(localEndPoint) == SOCKET_ERROR) {
			Winsock::AssertWinsockMessage("서버 소켓 Bind 실패");
			return false;
		}

		if (m_ServerSocket.Listen() == SOCKET_ERROR) {
			Winsock::AssertWinsockMessage("서버 소켓 Listen 실패");
			return false;
		}

		if (m_ServerSocket.Option().SetNonBlockingEnabled(true) == SOCKET_ERROR) {
			Winsock::AssertWinsockMessage("서버 소켓 SetNonBlockingEnabled(true) 실패");
		}

		if (m_pIocp->Create(DefaultIocpThreadCount()) == false) {
			Winsock::AssertWinsockMessage("서버 IOCP 생성 실패");
			return false;
		}

		if (m_pIocp->Connect(reinterpret_cast<HANDLE>(m_ServerSocket.Handle()), NULL) == false) {
			Winsock::AssertWinsockMessage("서버소켓을 IOCP에 연결하는데 실패하였습니다.");
			return false;
		}

		
		m_pIocp->Run();	// IOCP 워커 쓰레드 실행
		m_pContainer->Clear();

		// 세션을 미리 생성해놓고 연결 대기 상태로 둠
		for (int i = 0; i < MaxConnection(); i++) {
			TcpSession* session = new TcpSession(m_pIocp, m_pEventListener);
			const SOCKET hListeningSock = this->Socket().Handle();

			if (!session->Initialize()) {
				Winsock::AssertWinsockMessage("세션 초기화 실패");
				return false;
			}

			session->AcceptWait();
			if (!session->AcceptAsync(hListeningSock, new IOCPOverlappedAccept(this, session, m_pIocp))) {
				break;
			}
			m_pContainer->AddSession(session);
		}

		m_pEventListener->OnStarted();
		return bool(m_eState = State::Running);
	}

	bool TcpServer::Stop() {
		m_pIocp->Join();		// IOCP를 먼저 정리하여 워커쓰레드들의 작동을 멈춰주자.
		m_pContainer->Clear();	// 세션들을 정리해주자. 이걸 iocp join보다 먼저하면 GetQueuedCompletionStatus에서 995번에러를 뱉음 (I/O operation has been aborted)
		
		if (m_pIocp->Destroy() == false) {
			Winsock::Message("IOCP 삭제에 실패하였습니다.");
			return false;
		}

		if (m_ServerSocket.Close() == SOCKET_ERROR) {
			Winsock::AssertWinsockMessage("서버 소켓을 닫는데 실패하였습니다.");
			return false;
		}

		m_eState = State::Stopped;
		m_pEventListener->OnStopped();
		return true;
	}

	void TcpServer::Pause() {
		m_pIocp->Pause();
		m_pEventListener->OnPaused();
		m_eState = State::Paused;
	}

	void TcpServer::Resume() {
		m_pIocp->Resume();
		m_pEventListener->OnResume();
		m_eState = State::Paused;
	}

	void TcpServer::SetEventListener(TcpServerEventListener* listener) {
		if (m_eState == State::Running || m_eState == State::Paused) {
			Winsock::Message("이벤트 리스너는 서버가 실행/일시 정지중인 상태에서는 변경이 불가능합니다.");
			return;
		}

		m_pEventListener = listener;
	}

}

