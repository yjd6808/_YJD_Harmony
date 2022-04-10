/*
 * �ۼ��� : ������
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Packet.h>
#include <JNetwork/Host/TcpClient.h>

#include <JNetwork/IOCPOverlapped/IOCPOverlappedConnect.h>

using namespace JCore;

namespace JNetwork {

TcpClient::TcpClient() :
	TcpSession(new IOCP()),
	m_pClientEventListener(nullptr) {
}

TcpClient::~TcpClient() {

	m_pIocp->Join();

	if (m_pIocp->Destroy() == false) {
		Winsock::Message("IOCP ������ �����Ͽ����ϴ�.");
	}

	DeleteSafe(m_pIocp);
}

int TcpClient::DefaultIocpThreadCount() const {
	// �⺻������ ������ ������ŭ �����ϵ��� ����.
	// dwNumberOfProcessors �̸����� �ھ� ������ �����ϱ� ��� CPU ������ ������� �Ѵ�.
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return (int)info.dwNumberOfProcessors;
}

bool TcpClient::ConnectAsync(const IPv4EndPoint& destination) {
	if (CheckState(State::Connected)) {
		return false;
	}

	if (!Winsock::IsInitialized()) {
		Winsock::Message("���� �ʱ�ȭ�� ���� ���ּ���. Winsock::Initialize()");
		return false;
	}

	if (m_pClientEventListener == nullptr) {
		Winsock::Message("�̺�Ʈ �����ʸ� �������ּ���.");
		return false;
	}

	if (!Initialize()) {
		Winsock::AssertWinsockMessage("Ŭ���̾�Ʈ �ʱ�ȭ�� �����Ͽ����ϴ�.");
		return false;
	}

	// ConnectEx�� ����ϱ� ���ؼ� Ŭ���̾�Ʈ���� ���ε��� ������Ѵ�.
	if (m_ClientSocket.BindAny() == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("Ŭ���̾�Ʈ Any ���ε��� �����Ͽ����ϴ�.");
		return false;
	}

	if (m_pIocp->Create(DefaultIocpThreadCount()) == false) {
		Winsock::AssertWinsockMessage("Ŭ���̾�Ʈ IOCP ���� ����");
		return false;
	}

	if (m_pIocp->Connect(reinterpret_cast<HANDLE>(m_ClientSocket.Handle()), NULL) == false) {
		Winsock::AssertWinsockMessage("Ŭ���̾�Ʈ ������ IOCP�� �����ϴµ� �����Ͽ����ϴ�.");
		return false;
	}

	m_pIocp->Run();

	if (m_ClientSocket.Option().SetReuseAddrEnabled(true) == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("Ŭ���̾�Ʈ ���� SetReuseAddrEnabled(true) ����");
		return false;
	}

	if (m_ClientSocket.Option().SetNonBlockingEnabled(true) == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("Ŭ���̾�Ʈ ���� SetNonBlockingEnabled(true) ����");
		return false;
	}


	ConnectWait();

	// ���� �� ���� ������ ���� �׽�Ʈ
	// ��Ŷ�� ��� ������ Process���� �����ϵ��� �Ѵ�.
	DWORD dwSentBytes = 0;
	auto* dummyPacket = new Packet<Command<int>, Command<int>>;
	dummyPacket->Get<0>()->SetCommand(1);
	dummyPacket->Get<0>()->Value = 2;
	dummyPacket->Get<1>()->SetCommand(3);
	dummyPacket->Get<1>()->Value = 4;

	m_RemoteEndPoint = destination;

	IOCPOverlapped* pOverlapped = new IOCPOverlappedConnect(this, m_pIocp, dummyPacket);
	if (m_ClientSocket.ConnectEx(destination, pOverlapped, dummyPacket->GetWSABuf().buf, TEST_DUMMY_PACKET_SIZE, &dwSentBytes) == FALSE) {

		if (Winsock::LastError() != WSA_IO_PENDING) {
			Winsock::AssertWinsockMessage("���� ���ӿ� �����Ͽ����ϴ�.");
			Disconnect();
			pOverlapped->Release();
			m_RemoteEndPoint = IPv4EndPoint{IPv4Address::Any(), 0};
			return false;
		}
	}

	
	return true;
}

void TcpClient::Pause() {
	// NOT IMPLEMENT, UNUSED
	// ���� ���� ���� �� ����. ���� ����
}

void TcpClient::Resume() {
	// NOT IMPLEMENT, UNUSED
	// ���� ���� ���� �� ����. ���� ����
}

bool TcpClient::Disconnect() {
	CriticalSectionLockGuard guard(m_Lock);
	if (CheckState(State::Disconnected)) {
		return true;
	}

	m_ReceiveBuffer.Clear();
	m_ClientSocket.ShutdownBoth();		// ConnectWait ���¿��� �õ��ϴ� ��� ������ ����. �׳� ��������.

	if (m_ClientSocket.Close() == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("Ŭ���̾�Ʈ ���� Close() ����");
		return false;
	}

	m_eState = State::Disconnected;
	m_pClientEventListener->OnDisconnected();

	m_RemoteEndPoint = IPv4EndPoint{ IPv4Address::Any(), 0 };
	m_LocalEndPoint = IPv4EndPoint{ IPv4Address::Any(), 0 };
	return true;
}

void TcpClient::SetEventListener(TcpClientEventListener* listener) {
	if (!CheckState(State::Uninitialized) && !CheckState(State::Disconnected)) {
		Winsock::Message("������ ���� ���� �Ǵ� ������ ���������� ������ ������ �� �� �ֽ��ϴ�.");
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

	// �����ֱ⸶�� "�� ����ּ�" ����
	if (m_ClientSocket.Option().SetKeepAliveEnabled(true) == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("Ŭ���̾�Ʈ ���� Keep Alive Ȱ��ȭ ����");
	}

	// ���� ������ ���� Nagle �˰����� ���ش�.
	if (m_ClientSocket.Option().SetNagleEnabled(false) == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("Ŭ���̾�Ʈ ���� Nagle ��Ȱ��ȭ ����");
	}

	// Ŭ���̾�Ʈ�� ������ ������.
	// �۽� ���ۿ� �ִ� �����͸� ��� ������ �����ϰ� ������ �� �ֵ���
	if (m_ClientSocket.Option().SetLingerEnabled(false) == SOCKET_ERROR) {
		Winsock::AssertWinsockMessage("Ŭ���̾�Ʈ ���� ���� Ÿ�Ӿƿ� ���� ����");
	}

	m_LocalEndPoint = m_ClientSocket.GetLocalEndPoint();
	
	m_pClientEventListener->OnConnected();
}


}

