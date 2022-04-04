#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Packet.h>
#include <JNetwork/Host/TcpClient.h>

#include <JNetwork/IOCPOverlapped/IOCPOverlappedConnect.h>


namespace JNetwork {

TcpClient::TcpClient() :
	TcpSession(new IOCP()),
	m_pClientEventListener(nullptr) {
}

TcpClient::~TcpClient() {

	m_pIocp->Join();

	if (m_pIocp->Destroy() == false) {
		Winsock::Message("IOCP 삭제에 실패하였습니다.");
	}

	DeleteSafe(m_pIocp);
}

int TcpClient::DefaultIocpThreadCount() const {
	// 기본적으로 쓰레드 개수만큼 생성하도록 하자.
	// dwNumberOfProcessors 이름땜에 코어 갯수로 착각하기 쉬운데 CPU 쓰레드 갯수라고 한다.
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return (int)info.dwNumberOfProcessors;
}

bool TcpClient::ConnectAsync(const IPv4EndPoint& localEndPoint) {
	if (CheckState(State::Connected)) {
		return false;
	}

	if (!Winsock::IsInitialized()) {
		Winsock::Message("윈속 초기화를 먼저 해주세요. Winsock::Initialize()");
		return false;
	}

	if (m_pClientEventListener == nullptr) {
		Winsock::Message("이벤트 리스너를 설정해주세요.");
		return false;
	}

	if (!Initialize()) {
		Winsock::AssertWinsockMessage("클라이언트 초기화에 실패하였습니다.");
		return false;
	}

	// ConnectEx를 사용하기 위해서 클라이언트더라도 바인딩을 해줘야한다.
	if (m_ClientSocket.BindAny() == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("클라이언트 Any 바인딩에 실패하였습니다.");
		return false;
	}

	if (m_pIocp->Create(DefaultIocpThreadCount()) == false) {
		Winsock::AssertWinsockMessage("클라이언트 IOCP 생성 실패");
		return false;
	}

	if (m_pIocp->Connect(reinterpret_cast<HANDLE>(m_ClientSocket.Handle()), NULL) == false) {
		Winsock::AssertWinsockMessage("클라이언트 소켓을 IOCP에 연결하는데 실패하였습니다.");
		return false;
	}

	m_pIocp->Run();

	if (m_ClientSocket.Option().SetReuseAddrEnabled(true) == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("클라이언트 소켓 SetReuseAddrEnabled(true) 실패");
		return false;
	}

	if (m_ClientSocket.Option().SetNonBlockingEnabled(true) == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("클라이언트 소켓 SetNonBlockingEnabled(true) 실패");
		return false;
	}


	ConnectWait();

	// 연결 후 곧장 데이터 전송 테스트
	// 패킷은 모두 오버랩 Process에서 해제하도록 한다.
	DWORD dwSentBytes = 0;
	auto* dummyPacket = new Packet<Command<int>, Command<int>>;
	dummyPacket->Get<0>()->SetCommand(1);
	dummyPacket->Get<0>()->Value = 2;
	dummyPacket->Get<1>()->SetCommand(3);
	dummyPacket->Get<1>()->Value = 4;

	IOCPOverlapped* pOverlapped = new IOCPOverlappedConnect(this, m_pIocp, dummyPacket);
	if (m_ClientSocket.ConnectEx(localEndPoint, pOverlapped, dummyPacket->GetWSABuf().buf, TEST_DUMMY_PACKET_SIZE, &dwSentBytes) == FALSE) {

		if (Winsock::LastError() != WSA_IO_PENDING) {
			Winsock::AssertWinsockMessage("서버 접속에 실패하였습니다.");
			Disconnect();
			pOverlapped->Release();
			return false;
		}
	}

	
	return true;
}

void TcpClient::Pause() {
	// NOT IMPLEMENT, UNUSED
	// 딱히 쓸일 없을 것 같다. 구현 안함
}

void TcpClient::Resume() {
	// NOT IMPLEMENT, UNUSED
	// 딱히 쓸일 없을 것 같다. 구현 안함
}

bool TcpClient::Disconnect() {
	if (CheckState(State::Disconnected)) {
		return true;
	}

	m_ReceiveBuffer.Clear();
	m_ClientSocket.ShutdownBoth();		// ConnectWait 상태에서 시도하는 경우 오류를 뱉음. 그냥 무시하자.

	if (m_ClientSocket.Close() == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("클라이언트 소켓 Close() 실패");
		return false;
	}

	m_eState = State::Disconnected;
	m_pClientEventListener->OnDisconnected();
	return true;
}

void TcpClient::SetEventListener(TcpClientEventListener* listener) {
	if (!CheckState(State::Uninitialized) && !CheckState(State::Disconnected)) {
		Winsock::Message("연결이 끊긴 상태 또는 서버와 연결전에만 리스너 설정을 할 수 있습니다.");
		return;
	}

	m_pClientEventListener = listener;
}

void TcpClient::ConnectWait() {
	m_eState = State::ConnectWait;
}

void TcpClient::NotifyCommand(ICommand* cmd) {
	m_pClientEventListener->OnReceived(cmd);
}

void TcpClient::Sent(ISendPacket* sentPacket, Int32UL sentBytes) {
	m_pClientEventListener->OnSent(sentPacket, sentBytes);
}


void TcpClient::Connected() {
	m_eState = State::Connected;

	if (m_ClientSocket.Option().SetKeepAliveEnabled(true) == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("클라이언트 소켓 Keep Alive 활성화 실패");
		return;
	}

	if (m_ClientSocket.Option().SetNagleEnabled(false) == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("클라이언트 소켓 Nagle 비활성화 실패");
		return;
	}

	// 린저 ON / 타임아웃 0 연결끊으면 버퍼 데이터 바로 버리고 종료
	if (m_ClientSocket.Option().SetLingerTimeout(0) == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("클라이언트 소켓 린저 타임아웃 설정 실패");
		return;
	}

	m_RemoteEndPoint = m_ClientSocket.GetRemoteEndPoint();
	m_LocalEndPoint = m_ClientSocket.GetLocalEndPoint();
	m_pClientEventListener->OnConnected();
}


}

