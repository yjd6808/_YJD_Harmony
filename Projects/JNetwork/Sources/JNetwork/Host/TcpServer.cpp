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
		// �⺻������ ������ ������ŭ �����ϵ��� ����.
		// dwNumberOfProcessors �̸����� �ھ� ������ �����ϱ� ��� CPU ������ ������� �Ѵ�.
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		return (int)info.dwNumberOfProcessors; 
	}


	bool TcpServer::Start(const IPv4EndPoint& localEndPoint) {
		if (m_eState == State::Paused || m_eState == State::Running) {
			return false;
		}

		if (!Winsock::IsInitialized()) {
			Winsock::Message("���� �ʱ�ȭ�� ���� ���ּ���. Winsock::Initialize()");
			return false;
		}

		if (m_pEventListener == nullptr) {
			Winsock::AssertWinsockMessage("�̺�Ʈ �����ʸ� �������ּ���.");
			return false;
		}


		m_ServerSocket = Socket::CreateTcpV4(true);

		if (!m_ServerSocket.IsValid()) {
			Winsock::AssertWinsockMessage("���� ���� Create ����");
			return false;
		}

		if (m_ServerSocket.Option().SetReuseAddrEnabled(true) == SOCKET_ERROR) {
			Winsock::AssertWinsockMessage("���� ���� SetReuseAddrEnabled(true) ����");
		}

		if (m_ServerSocket.Bind(localEndPoint) == SOCKET_ERROR) {
			Winsock::AssertWinsockMessage("���� ���� Bind ����");
			return false;
		}

		if (m_ServerSocket.Listen() == SOCKET_ERROR) {
			Winsock::AssertWinsockMessage("���� ���� Listen ����");
			return false;
		}

		if (m_ServerSocket.Option().SetNonBlockingEnabled(true) == SOCKET_ERROR) {
			Winsock::AssertWinsockMessage("���� ���� SetNonBlockingEnabled(true) ����");
		}

		if (m_pIocp->Create(DefaultIocpThreadCount()) == false) {
			Winsock::AssertWinsockMessage("���� IOCP ���� ����");
			return false;
		}

		if (m_pIocp->Connect(reinterpret_cast<HANDLE>(m_ServerSocket.Handle()), NULL) == false) {
			Winsock::AssertWinsockMessage("���������� IOCP�� �����ϴµ� �����Ͽ����ϴ�.");
			return false;
		}

		
		m_pIocp->Run();	// IOCP ��Ŀ ������ ����
		m_pContainer->Clear();

		// ������ �̸� �����س��� ���� ��� ���·� ��
		for (int i = 0; i < MaxConnection(); i++) {
			TcpSession* session = new TcpSession(m_pIocp, m_pEventListener);
			const SOCKET hListeningSock = this->Socket().Handle();

			if (!session->Initialize()) {
				Winsock::AssertWinsockMessage("���� �ʱ�ȭ ����");
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

	// [������ ���� ���Ḧ �����ϱ� ���� ������ ���� �帧]
	// 1. ������ ������ ��� �����ش�.
	//   --> ���������� ��� ���з� �������� ���̴�. ��, IOCP���� ������� Pending I/O���� �ϳ��� �Ϸ�Ǹ鼭 IOCP�� ���ǵ� PendingCount�� 1�� �ٿ��� ���̴�.
	//   --> �̶� ���� ���� ���Ḧ �����ϴ� main �����忡�� m_pIocp->Join�� �����ع����� ��ٷ� �ݺ����� ���� ���ͼ� ó�� �Ǿ���� I/O���� ���õȴ�.
	// 2. �׷��� Pending ������ Overlapped I/O�� ������ Ȯ���ؼ� �� ���������� ��� �����Ҵ��� �����Ǿ� IOCP�� PendingCount�� 0�� �ɶ����� ��ٸ���.
	// 3. Pending I/O���� ��� ó�� �Ǿ���. ������ ���� IOCP �ڵ�� ���� ������ ����Ǿ��ְ� ������ ���� �ƴϹǷ� GetQueuedCompletionStatus���� ������� ���°� �ȴ�.
	// 4. �̶� m_pIocp->Join()���� PostQueuedCompletionStatus�� ��Ŀ �������� �ݺ����� �������Ͷ�� ����� ������.
	// 5. ���� �����ϰ� IOCP �ڵ��� �������ش�.
	// 6. ���� ���ϵ� �����ϰ� �ݾ��ش�.
	// 7. �����̳��� ���ǵ��� ��� �������ش�.
	bool TcpServer::Stop() {
		m_pContainer->DisconnectAllSessions();	// ���ǵ��� ����������. �̰� iocp join���� �����ϸ� GetQueuedCompletionStatus���� 995�������� ���� (I/O operation has been aborted)

		
		// PendingCount�� 0�� �ɶ����� ��ٸ���. (Busy Waiting)
		while (m_pIocp->GetPendingCount() != 0) {
		}

		// ��Ŀ ������鿡�� Post I/O�� �����ؼ� ��Ŀ �����尡 joinable ���°� �ǵ��� �ݺ����� Ż���������
		m_pIocp->Join();

		// IOCP �ڵ��� ����������.
		if (m_pIocp->Destroy() == false) {
			Winsock::Message("IOCP ������ �����Ͽ����ϴ�.");
			return false;
		}

		// ���� ������ �ݾ�����.
		if (m_ServerSocket.Close() == SOCKET_ERROR) {
			Winsock::AssertWinsockMessage("���� ������ �ݴµ� �����Ͽ����ϴ�.");
			return false;
		}

		// �����Ҵ�� ���ǵ��� ��� ����������.
		m_pContainer->Clear();

		// [[[ ���� �Ϻ� ���� ]]]
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
			Winsock::Message("�̺�Ʈ �����ʴ� ������ ����/�Ͻ� �������� ���¿����� ������ �Ұ����մϴ�.");
			return;
		}

		m_pEventListener = listener;
	}

}

