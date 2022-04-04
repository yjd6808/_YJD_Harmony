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
		Winsock::Message("�۽��� ��Ŷ�� ũ�⸦ ����� ����ּ���");
		return false;
	}

	packet->AddRef();
	WSABUF buf = packet->GetWSABuf();
	Int32UL uiSendBytes = 0;
	IOCPOverlapped* pOverlapped = new IOCPOverlappedSend(this, m_pIocp, packet);

	const int iResult = m_ClientSocket.SendEx(&buf, &uiSendBytes, pOverlapped);
	if (iResult == SOCKET_ERROR) {
		if (Winsock::LastError() != WSA_IO_PENDING) {
			Winsock::WinsockMessage("SendAsync() ����");
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
			Winsock::WinsockMessage("ReceiveAsync() ����");
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
		TEST_DUMMY_PACKET_SIZE,	// TcpClient���� �׽�Ʈ ���� ��Ŷ�� ������ ���� 8�� ����
		&receivedBytes,
		pOverlapped
	) == FALSE) {

		if (Winsock::LastError() != WSA_IO_PENDING) {
			Winsock::WinsockMessage("���� AcceptEx ����");
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
	// AcceptEx �Լ�  ȣ�� �� ����� ���Ͽ� ���ؼ� ���� �ּҿ� ����Ʈ �ּҸ� ������ �� �ֵ��� ������Ʈ ���ش�.
	// �̰� �������� ������ �ش� ���Ͽ� ���ε��� ���� �ּҿ� ����Ʈ �ּҸ� ��������
	//    = getsockname(), getpeername() �ȸ���
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
		// ��Ŷ�� ��� ũ�⸸ŭ �����͸� �������� �ʾ����� ���϶����� ���
		if (m_ReceiveBuffer.GetReadableBufferSize() < PACKET_HEADER_SIZE)
			return;

		// ��Ŷ ��� ���� + ��Ŷ ���� ��ŭ �������� �ʾ����� �ٽ� ���϶����� ��ٸ���.
		const IRecvPacket* packet = m_ReceiveBuffer.Peek<IRecvPacket*>();
		if (m_ReceiveBuffer.GetReadableBufferSize() < (PACKET_HEADER_SIZE + packet->GetPacketLength())) {
			return;
		}

		m_ReceiveBuffer.MoveReadPos(PACKET_HEADER_SIZE);

		for (int i = 0; i < packet->GetCommandCount(); i++) {
			ICommand* pCmd = m_ReceiveBuffer.Peek<ICommand*>();

			// ������ TcpServerEventListener�� Ŀ�ǵ� ����
			// Ŭ��� TcpClientEventListener�� Ŀ�ǵ� ����
			NotifyCommand(pCmd);

			if (m_ReceiveBuffer.MoveReadPos(pCmd->GetCommandLen()) == false) {
				Winsock::Message("Ŀ�ǵ� ũ�Ⱑ �̻��մϴ�.");
				m_ReceiveBuffer.Clear();
				return;
			}
		}

		if (m_ReceiveBuffer.GetReadPos() == m_ReceiveBuffer.GetWritePos()) {
			// ���� ������ �����͸� ��� �о����� �������� �׳� 0���� �ű��.
			m_ReceiveBuffer.Clear();
		} else {
			// ���� ��ġ��ŭ�� ���� �ٽ� �������� �����Ƿ� ���۸� ������ ����. 
			// WritePos ���ķ� �����͸� ���� �� �ֵ����ϱ� ����
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

