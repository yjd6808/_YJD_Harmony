/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/Host/TcpClient.h>

#include <JNetwork/Packet/SendPacket.h>
#include <JNetwork/Packet/RecvPacket.h>

#include <JNetwork/IOCPOverlapped/IOCPOverlappedConnect.h>

using namespace JCore;

NS_JNET_BEGIN

TcpClient::TcpClient(
	const IOCPPtr& iocp,
	const JCore::MemoryPoolAbstractPtr& bufferAllocator,
	int sendBuffSize, 
	int recvBufferSize)
	: Session(iocp, bufferAllocator, sendBuffSize, recvBufferSize)
	, m_pEventListener(nullptr) {

	TcpClient::Initialize();
}

TcpClient::~TcpClient() {
	Disconnect();
	WaitForZeroPending();
	JCORE_DELETE_SAFE(m_pEventListener);
}

static StaticPacket<GenericCommand<int>, GenericCommand<int>>* GenerateTestDummyPacket() {
	auto* dummyPacket = dbg_new StaticPacket<GenericCommand<int>, GenericCommand<int>>;
	dummyPacket->Get<0>()->SetCommand(1);
	dummyPacket->Get<0>()->Value = 2;
	dummyPacket->Get<1>()->SetCommand(3);
	dummyPacket->Get<1>()->Value = 4;
	dummyPacket->AddRef();
	return dummyPacket;
}

void TcpClient::Initialize() {

	Session::Initialize();

	if (CreateSocket(TransportProtocol::TCP) == false) {
		DebugAssertMsg(false, "TCP 소켓 생성에 실패했습니다. (%u)", Winsock::LastError());
	}

	if (ConnectIocp() == false) {
		DebugAssertMsg(false, "IOCP에 연결하는데 실패했습니다. (%u)", Winsock::LastError());
	}
}


// https://stackoverflow.com/questions/46045434/winsock-c-connect-timeout
// select로 타임아웃 연결 구현가능
bool TcpClient::Connect(const IPv4EndPoint& remoteAddr, int timeoutMiliseconds) {

	if (!m_Socket.IsValid()) {
		_NetLogError_("연결에 실패했습니다. INVALID_SOCKET 입니다.", Winsock::LastError());
		return false;
	}


	if (m_Socket.Option().SetNonBlockingEnabled(true) == SOCKET_ERROR) {
		const Int32U uiErrorCode = Winsock::LastError();
		_NetLogError_("연결에 실패했습니다. 논블로킹 소켓 전환실패 (%u)", uiErrorCode);
		return false;
	}

	if (m_Socket.Connect(remoteAddr) == SOCKET_ERROR) {

		const Int32U uiErrorCode = Winsock::LastError();

		if (uiErrorCode != WSAEWOULDBLOCK) {
			_NetLogError_("연결에 실패했습니다. (%u)", uiErrorCode);
			return false;
		}

		fd_set setWrite, setException;

		FD_ZERO(&setWrite);
		FD_ZERO(&setException);

		FD_SET(m_Socket.Handle, &setWrite);
		FD_SET(m_Socket.Handle, &setException);

		timeval timeout;
		timeout.tv_sec = timeoutMiliseconds / 1000;
		timeout.tv_usec = (timeoutMiliseconds % 1000) * 1000;

		const int iSelectRet = select(0, nullptr, &setWrite, &setException, timeoutMiliseconds == 0 ? nullptr : &timeout);
		if (iSelectRet <= 0) {
			// 이때 연결을 시도 중(10037)일 수도 있지만 시간이 지났으므로 타임아웃 처리
			m_Socket.Close();
			m_eState = eDisconnected;
			Initialize();
			WSASetLastError(WSAETIMEDOUT);

			if (m_pEventListener)
				m_pEventListener->OnConnectFailed(this, WSAETIMEDOUT);
			return false;
		}

		if (FD_ISSET(m_Socket.Handle, &setException)) {
			const int err = m_Socket.Option().GetErrorCode();
			m_Socket.Close();
			m_eState = eDisconnected;
			Initialize();
			WSASetLastError(err);

			if (m_pEventListener)
				m_pEventListener->OnConnectFailed(this, err);

			return false;
		}
	}

	m_RemoteEndPoint = remoteAddr;
	Connected();
#if TEST_DUMMY_PACKET_TRANSFER
	// 연결 후 곧장 데이터 전송 테스트
	if (SendAsync(GenerateTestDummyPacket())) {
		
	}
#endif

	return RecvAsync();
}


bool TcpClient::ConnectAsync(const IPv4EndPoint& destination) {

	// 초기화된 상태에서만 연결을 진행할 수 있습니다.
	if (m_eState != eInitailized) {
		return false;
	}

	m_eState = eConnectWait;

	if (m_bIocpConneced == false) {
		DebugAssertMsg(false, "IOCP와 연결해주세요.");
		return false;
	}

	// ConnectEx를 사용하기 위해서 클라이언트더라도 바인딩을 해줘야한다.
	if (!m_Socket.IsBinded() && Bind({}) == false) {
		return false;
	}

	// 연결 후 곧장 데이터 전송 테스트
	// 패킷은 모두 오버랩 Process에서 해제하도록 한다.
	Int32UL dwSentBytes = 0;
	auto dummyPacket =
#if TEST_DUMMY_PACKET_TRANSFER
	GenerateTestDummyPacket();
#else
	nullptr;
#endif

	IOCPOverlapped* pOverlapped = dbg_new IOCPOverlappedConnect(this, m_spIocp.GetPtr(), dummyPacket);
	if (m_Socket.ConnectEx(destination, pOverlapped,
#if TEST_DUMMY_PACKET_TRANSFER
		dummyPacket->GetWSABuf().buf, 
		TEST_DUMMY_PACKET_SIZE,
#else
		nullptr,
		0,
#endif
		&dwSentBytes) == FALSE) {
		const Int32U uiError = Winsock::LastError();
		if (uiError != WSA_IO_PENDING) {
			DebugAssertMsg(false, "서버 접속에 실패하였습니다. (%u)", uiError);
			Disconnect();
			pOverlapped->Release();
			return false;
		}
	}

	
	return true;
}



void TcpClient::Disconnected() {
	if (m_pEventListener)
		m_pEventListener->OnDisconnected(this);
	Initialize();
}

void TcpClient::NotifyCommand(ICommand* cmd) {
	if (m_pEventListener)
		m_pEventListener->OnReceived(this, cmd);
}

void TcpClient::NotifyPacket(IRecvPacket* packet) {
	if (m_pEventListener)
		m_pEventListener->OnReceived(this, packet);
}

void TcpClient::Sent(ISendPacket* sentPacket, Int32UL sentBytes) {
	if (m_pEventListener)
		m_pEventListener->OnSent(this, sentPacket, sentBytes);
}


void TcpClient::Connected() {
	m_eState = eConnected;

	// 일정주기마다 "나 살아있소" 전송
	if (m_Socket.Option().SetKeepAliveEnabled(true) == SOCKET_ERROR) {
		DebugAssertMsg(false, "클라이언트 소켓 Keep Alive 활성화 실패");
	}

	// 빠른 반응을 위해 Nagle 알고리즘을 꺼준다.
	if (m_Socket.Option().SetNagleEnabled(false) == SOCKET_ERROR) {
		DebugAssertMsg(false, "클라이언트 소켓 Nagle 비활성화 실패");
	}

	// 클라이언트는 린저를 꺼주자.
	// 송신 버퍼에 있는 데이터를 모두 보내고 안전하게 종료할 수 있도록
	if (m_Socket.Option().SetLingerEnabled(false) == SOCKET_ERROR) {
		DebugAssertMsg(false, "클라이언트 소켓 린저 타임아웃 설정 실패");
	}

	if (m_pEventListener)
		m_pEventListener->OnConnected(this);
}

void TcpClient::ConnectFailed(Int32U errorCode) {
	if (m_pEventListener)
		m_pEventListener->OnConnectFailed(this, errorCode);

	Initialize();
}

NS_JNET_END

