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
	ClientEventListener* listener,
	int sendBuffSize, 
	int recvBufferSize)
	: Session(iocp, bufferAllocator, sendBuffSize, recvBufferSize)
	, m_pClientEventListener(listener) {

	TcpClient::Initialize();
}

TcpClient::~TcpClient() {
	Disconnect();
}

void TcpClient::Initialize() {

	Session::Initialize();

	if (CreateSocket(TransportProtocol::TCP) == false) {
		DebugAssertMsg(false, "TCP 소켓 생성에 실패했습니다.");
	}

	if (ConnectIocp() == false) {
		DebugAssertMsg(false, "IOCP에 연결하는데 실패했습니다.");
	}
}


bool TcpClient::ConnectAsync(const IPv4EndPoint& destination) {

	// 초기화된 상태에서만 연결을 진행할 수 있습니다.
	if (m_eState != eInitailized) {
		return false;
	}

	if (m_bIocpConneced == false) {
		DebugAssertMsg(false, "IOCP와 연결해주세요.");
		return false;
	}

	// ConnectEx를 사용하기 위해서 클라이언트더라도 바인딩을 해줘야한다.
	if (Bind({}) == false) {
		DebugAssertMsg(false, "클라이언트 Any 바인딩에 실패하였습니다.");
		return false;
	}

	// 연결 후 곧장 데이터 전송 테스트
	// 패킷은 모두 오버랩 Process에서 해제하도록 한다.
	Int32UL dwSentBytes = 0;
	auto* dummyPacket = new StaticPacket<Command<int>, Command<int>>;
	dummyPacket->Get<0>()->SetCommand(1);
	dummyPacket->Get<0>()->Value = 2;
	dummyPacket->Get<1>()->SetCommand(3);
	dummyPacket->Get<1>()->Value = 4;
    dummyPacket->AddRef();

	IOCPOverlapped* pOverlapped = new IOCPOverlappedConnect(this, m_spIocp.GetPtr(), dummyPacket);
	if (m_Socket.ConnectEx(destination, pOverlapped, dummyPacket->GetWSABuf().buf, TEST_DUMMY_PACKET_SIZE, &dwSentBytes) == FALSE) {
		Int32U uiError = Winsock::LastError();
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
	m_pClientEventListener->OnDisconnected();
}

void TcpClient::NotifyCommand(ICommand* cmd) {
	m_pClientEventListener->OnReceived(cmd);
}

void TcpClient::Sent(ISendPacket* sentPacket, Int32UL sentBytes) {
	m_pClientEventListener->OnSent(sentPacket, sentBytes);
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

	m_pClientEventListener->OnConnected();
}

NS_JNET_END

