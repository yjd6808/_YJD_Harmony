/*
 * 작성자 : 윤정도
 */

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
			DebugAssertMessage(false, "윈속 초기화를 먼저 해주세요. Winsock::Initialize()");
			return false;
		}

		if (m_pEventListener == nullptr) {
			DebugAssertMessage(false, "이벤트 리스너를 설정해주세요.");
			return false;
		}


		m_ServerSocket = Socket::CreateTcpV4(true);

		if (!m_ServerSocket.IsValid()) {
			DebugAssertMessage(false, "서버 소켓 Create 실패");
			return false;
		}

		if (m_ServerSocket.Option().SetReuseAddrEnabled(true) == SOCKET_ERROR) {
			DebugAssertMessage(false, "서버 소켓 SetReuseAddrEnabled(true) 실패");
		}

		if (m_ServerSocket.Bind(localEndPoint) == SOCKET_ERROR) {
			DebugAssertMessage(false, "서버 소켓 Bind 실패");
			return false;
		}

		if (m_ServerSocket.Listen() == SOCKET_ERROR) {
			DebugAssertMessage(false, "서버 소켓 Listen 실패");
			return false;
		}

		if (m_ServerSocket.Option().SetNonBlockingEnabled(true) == SOCKET_ERROR) {
			DebugAssertMessage(false, "서버 소켓 SetNonBlockingEnabled(true) 실패");
		}

		if (m_pIocp->Create(DefaultIocpThreadCount()) == false) {
			DebugAssertMessage(false, "서버 IOCP 생성 실패");
			return false;
		}

		// https://docs.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-acceptex
		// AcceptEx 사용시 리슨 소켓을 IOCP와 연결해줘야 클라이언트 접속을 통보받을 수 있다.
		if (m_pIocp->Connect(reinterpret_cast<WinHandle>(m_ServerSocket.Handle()), NULL) == false) {
			DebugAssertMessage(false, "서버소켓을 IOCP에 연결하는데 실패하였습니다.");
			return false;
		}

		
		m_pIocp->Run();	// IOCP 워커 쓰레드 실행
		m_pContainer->Clear();

		// 세션을 미리 생성해놓고 연결 대기 상태로 둠
		for (int i = 0; i < MaxConnection(); i++) {
			TcpSession* session = new TcpSession(m_pIocp, m_pEventListener);
			const SOCKET hListeningSock = this->Socket().Handle();

			if (!session->Initialize()) {
				DebugAssertMessage(false, "세션 초기화 실패");
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

	// [안전한 서버 종료를 수행하기 위한 정도의 생각 흐름]
	// 1. 세션의 연결을 모두 끊어준다.
	//   --> 오버랩들이 모두 실패로 빠져나올 것이다. 즉, IOCP에서 대기중인 Pending I/O들이 하나씩 완료되면서 IOCP에 정의된 PendingCount를 1씩 줄여줄 것이다.
	//   --> 이때 만약 서버 종료를 수행하는 main 쓰레드에서 m_pIocp->Join을 수행해버리면 곧바로 반복문을 빠져 나와서 처리 되어야할 I/O들이 무시된다.
	// 2. 그래서 Pending 상태인 Overlapped I/O의 갯수를 확인해서 이 오버랩들이 모두 동적할당이 해제되어 IOCP의 PendingCount가 0이 될때까지 기다린다.
	// 3. Pending I/O들이 모두 처리 되었다. 하지만 아직 IOCP 핸들과 서버 소켓은 연결되어있고 해제된 것이 아니므로 GetQueuedCompletionStatus에서 대기중인 상태가 된다.
	// 4. 이때 m_pIocp->Join()으로 PostQueuedCompletionStatus로 워커 쓰레드의 반복문을 빠져나와라고 명령을 보낸다.
	// 5. 이제 안전하게 IOCP 핸들을 해제해준다.
	// 6. 서버 소켓도 안전하게 닫아준다.
	// 7. 컨테이너의 세션들을 모두 정리해준다.
	bool TcpServer::Stop() {
		m_pContainer->DisconnectAllSessions();	// 세션들을 정리해주자. 이걸 iocp join보다 먼저하면 GetQueuedCompletionStatus에서 995번에러를 뱉음 (I/O operation has been aborted)

		
		// PendingCount가 0이 될때까지 기다린다. (Busy Waiting)
		while (m_pIocp->GetPendingCount() != 0) {
		}

		// 워커 쓰레드들에게 Post I/O를 전달해서 워커 쓰레드가 joinable 상태가 되도록 반복문을 탈출시켜주자
		m_pIocp->Join();

		// IOCP 핸들을 해제해주자.
		if (m_pIocp->Destroy() == false) {
			DebugAssertMessage(false, "IOCP 삭제에 실패하였습니다.");
			return false;
		}

		// 서버 소켓을 닫아주자.
		if (m_ServerSocket.Close() == SOCKET_ERROR) {
			DebugAssertMessage(false, "서버 소켓을 닫는데 실패하였습니다.");
			return false;
		}

		// 동적할당된 세션들을 모두 해제해주자.
		m_pContainer->Clear();

		// [[[ 서버 완벽 중지 ]]]
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
		m_eState = State::Running;
	}

	void TcpServer::SetEventListener(TcpServerEventListener* listener) {
		if (m_eState == State::Running || m_eState == State::Paused) {
			DebugAssertMessage(false, "이벤트 리스너는 서버가 실행/일시 정지중인 상태에서는 변경이 불가능합니다.");
			return;
		}

		m_pEventListener = listener;
	}

}

