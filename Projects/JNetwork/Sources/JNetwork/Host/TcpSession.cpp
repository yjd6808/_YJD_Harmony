#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/Host/TcpSession.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedReceive.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSend.h>

#include <MSWSock.h>



namespace JNetwork {
	TcpSession::TcpSession(IOCP* iocp, TcpServerEventListener* listener) :
	m_pIocp(iocp),
	m_eState(State::Uninitialized),
	m_ClientSocket(INVALID_SOCKET),
	m_pServerEventListener(listener)
{
}

TcpSession::~TcpSession() {
}

bool TcpSession::Disconnect() {
	if (CheckState(State::Disconnected)) {
		return false;
	}

	m_ClientSocket.ShutdownBoth();
	m_eState = State::Disconnected;
	m_pServerEventListener->OnDisconnected(this);
	m_ClientSocket.Close();
	
	return true;
}


bool TcpSession::SendAsync(IPacket* packet) {
	WSABUF buf = packet->GetWSABuf();
	Int32UL uiSendBytes = 0;
	IOCPOverlapped* pOverlapped = new IOCPOverlappedSend(this, m_pIocp, packet);

	int iResult = m_ClientSocket.SendEx(&buf, &uiSendBytes, pOverlapped);
	if (iResult == SOCKET_ERROR) {
		if (Winsock::LastError() != WSA_IO_PENDING) {
			Winsock::WinsockMessage("SendAsync() ����");
			pOverlapped->Release();
			return false;
		} 
	}
	
	return true;
}

bool TcpSession::ReceiveAsync() {
	WSABUF buf = m_ReceiveBuffer.GetRemainBuffer();
	Int32UL uiReceivedBytes = 0;
	IOCPOverlapped* pOverlapped = new IOCPOverlappedReceive(this, m_pIocp);

	int iResult = m_ClientSocket.ReceiveEx(&buf, &uiReceivedBytes, pOverlapped);
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

	// AcceptEx �Լ� ȣ�� �� ����� ���Ͽ� ���ؼ� ���� �ּҿ� ����Ʈ �ּҸ� ������ �� �ֵ��� ������Ʈ ���ش�.
	// �̰� �������� ������ �ش� ���Ͽ� ���ε��� ���� �ּҿ� ����Ʈ �ּҸ� ��������
	//    = getsockname(), getpeername() �ȸ���
	if (m_ClientSocket.Option().SetUpdateAcceptContext(listeningSocket) == SOCKET_ERROR) {
		return false;
	}

	m_ClientSocket.AcceptExResult(pReads, TEST_DUMMY_PACKET_SIZE, &m_LocalEndPoint, &m_RemoteEndPoint);
	m_eState = State::Accepted;
	m_pServerEventListener->OnConnected(this);

	if (receivedBytes > 0) {
		Received(receivedBytes);
	}

	return true;
}

void TcpSession::Received(Int32UL receivedBytes) {
	m_ReceiveBuffer.MoveWritePos(receivedBytes);
	m_pServerEventListener->OnReceived(this, receivedBytes);

	
	if (m_ReceiveBuffer.GetReadPos() == m_ReceiveBuffer.GetWritePos()) {
		// ���� ������ �����͸� ��� �о����� �������� �׳� 0���� �ű��.
		m_ReceiveBuffer.Clear();
	} else {
		// ���� ��ġ��ŭ�� ���� �ٽ� �������� �����Ƿ� ���۸� ������ ����. WritePos ���ķ� �����͸� ���� �� �ֵ����ϱ� ����
		m_ReceiveBuffer.Pop(m_ReceiveBuffer.GetReadPos(), true);
	}
}

void TcpSession::Sent(IPacket* sentPacket, Int32UL sentBytes) {
	m_pServerEventListener->OnSent(this, sentPacket, sentBytes);
}


}

