#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedReceive.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSend.h>

#include <MSWSock.h>



namespace JNetwork {
	TcpSession::TcpSession(IOCP* iocp, TcpServerEventListener* listener) :
	m_pServerEventListener(listener),
	m_eState(State::Uninitialized),
	m_ClientSocket(INVALID_SOCKET),
	m_pIocp(iocp)
{
}

TcpSession::~TcpSession() = default;

bool TcpSession::Disconnect() {
	if (CheckState(State::Disconnected)) {
		return false;
	}

	m_ClientSocket.ShutdownBoth();
	m_ClientSocket.Close();
	m_eState = State::Disconnected;
	m_pServerEventListener->OnDisconnected(this);
	
	return true;
}


bool TcpSession::SendAsync(ISendPacket* packet) {
	if (packet->GetPacketLength() >= MAX_MSS - PACKET_HEADER_SIZE) {
		Winsock::Message("송신할 패킷의 크기를 제대로 잡아주세요");
		return false;
	}

	packet->AddRef();
	WSABUF buf = packet->GetWSABuf();
	Int32UL uiSendBytes = 0;
	IOCPOverlapped* pOverlapped = new IOCPOverlappedSend(this, m_pIocp, packet);

	const int iResult = m_ClientSocket.SendEx(&buf, &uiSendBytes, pOverlapped);
	if (iResult == SOCKET_ERROR) {
		if (Winsock::LastError() != WSA_IO_PENDING) {
			Winsock::WinsockMessage("SendAsync() 실패");
			pOverlapped->Release();
			packet->Release();
			return false;
		} 
	}
	
	return true;
}

bool TcpSession::ReceiveAsync() {
	WSABUF buf = m_ReceiveBuffer.GetRemainBuffer();
	Int32UL uiReceivedBytes = 0;
	IOCPOverlapped* pOverlapped = new IOCPOverlappedReceive(this, m_pIocp);

	const int iResult = m_ClientSocket.ReceiveEx(&buf, &uiReceivedBytes, pOverlapped);
	if (iResult == SOCKET_ERROR) {
		if (Winsock::LastError() != WSA_IO_PENDING) {
			Winsock::WinsockMessage("ReceiveAsync() 실패");
			pOverlapped->Release();
			return false;
		}
	}

	return true;
}

bool TcpSession::AcceptAsync(SOCKET hListeningSock, LPOVERLAPPED pOverlapped) {
	DWORD receivedBytes = 0;

	if (m_ClientSocket.AcceptEx(
		hListeningSock,
		m_ReceiveBuffer.GetRemainBuffer().buf,
		TEST_DUMMY_PACKET_SIZE,	// TcpClient에서 테스트 더미 패킷을 보내기 땜에 8로 세팅
		&receivedBytes,
		pOverlapped
	) == FALSE) {

		if (Winsock::LastError() != WSA_IO_PENDING) {
			Winsock::WinsockMessage("세션 AcceptEx 실패");
			((IOCPOverlapped*)pOverlapped)->Release();
			return false;
		}
	}

	return true;
}

bool TcpSession::Initialize() {
	if (m_ClientSocket.IsValid()) {
		m_ClientSocket.Close();
	}

	m_ClientSocket = Socket::CreateTcpV4(true);

	if (!m_ClientSocket.IsValid())
		return false;

	m_eState = State::Initailized;
	m_ReceiveBuffer.Clear();
	return true;
}

void TcpSession::AcceptWait() {
	m_eState = State::AcceptWait;
}

bool TcpSession::Accepted(SOCKET listeningSocket, Int32UL receivedBytes) {
	char* pReads = m_ReceiveBuffer.Peek<char*>();
	// AcceptEx 함수  호출 후 연결된 소켓에 대해서 로컬 주소와 리모트 주소를 가져올 수 있도록 업데이트 해준다.
	// 이걸 실행하지 않으면 해당 소켓에 바인딩된 로컬 주소와 리모트 주소를 못가져옴
	//    = getsockname(), getpeername() 안먹힘
	if (m_ClientSocket.Option().SetUpdateAcceptContext(listeningSocket) == SOCKET_ERROR) {
		return false;
	}

	
	m_ClientSocket.AcceptExResult(pReads, TEST_DUMMY_PACKET_SIZE, &m_LocalEndPoint, &m_RemoteEndPoint);
	m_eState = State::Accepted;
	m_pServerEventListener->OnConnected(this);

	return true;
}

void TcpSession::Received(Int32UL receivedBytes) {
	m_ReceiveBuffer.MoveWritePos(receivedBytes);

	
	for (;;) {
		// 패킷의 헤더 크기만큼 데이터를 수신하지 않았으면 모일때까지 기달
		if (m_ReceiveBuffer.GetReadableBufferSize() < PACKET_HEADER_SIZE)
			return;

		// 패킷 헤더 길이 + 패킷 길이 만큼 수신하지 않았으면 다시 모일때까지 기다린다.
		const IRecvPacket* packet = m_ReceiveBuffer.Peek<IRecvPacket*>();
		if (m_ReceiveBuffer.GetReadableBufferSize() < (PACKET_HEADER_SIZE + packet->GetPacketLength())) {
			return;
		}

		m_ReceiveBuffer.MoveReadPos(PACKET_HEADER_SIZE);

		for (int i = 0; i < packet->GetCommandCount(); i++) {
			ICommand* pCmd = m_ReceiveBuffer.Peek<ICommand*>();

			// 세션은 TcpServerEventListener로 커맨드 전달
			// 클라는 TcpClientEventListener로 커맨드 전달
			NotifyCommand(pCmd);

			if (m_ReceiveBuffer.MoveReadPos(pCmd->GetCommandLen()) == false) {
				Winsock::Message("커맨드 크기가 이상합니다.");
				m_ReceiveBuffer.Clear();
				return;
			}
		}

		if (m_ReceiveBuffer.GetReadPos() == m_ReceiveBuffer.GetWritePos()) {
			// 만약 수신한 데이터를 모두 읽었으면 포지션을 그냥 0으로 옮긴다.
			m_ReceiveBuffer.Clear();
		} else {
			// 읽은 위치만큼은 이제 다시 쓰일일이 없으므로 버퍼를 앞으로 당긴다. 
			// WritePos 이후로 데이터를 쌓을 수 있도록하기 위해
			m_ReceiveBuffer.Pop(m_ReceiveBuffer.GetReadPos(), true);
		}
	}
}

void TcpSession::NotifyCommand(ICommand* cmd) {
	m_pServerEventListener->OnReceived(this, cmd);
}

void TcpSession::Sent(ISendPacket* sentPacket, Int32UL sentBytes) {
	m_pServerEventListener->OnSent(this, sentPacket, sentBytes);
}


}

