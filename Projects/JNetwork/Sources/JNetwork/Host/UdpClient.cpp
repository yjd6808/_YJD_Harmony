/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 2:33:47 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Host/UdpClient.h>

NS_JNET_BEGIN

USING_NS_JC;

UdpClient::UdpClient(
	const IOCPPtr& iocp,
	const MemoryPoolAbstractPtr& bufferAllocator,
	PacketParser* parser,
	int recvBufferSize,
	int sendBufferSize
)
	: Session(iocp, bufferAllocator, parser, recvBufferSize, sendBufferSize)
	, m_pEventListener(nullptr)
{
	UdpClient::Initialize();
}

UdpClient::~UdpClient() {
	Disconnect();
	WaitForZeroPending();

	JCORE_DELETE_SAFE(m_pEventListener);
}


void UdpClient::Initialize() {
	Session::Initialize();

	if (CreateSocket(TransportProtocol::UDP, NonblokingSocket) == false) {
		DebugAssertMsg(false, "UDP 소켓 생성에 실패했습니다. (%u)", Winsock::LastError());
	}

	if (ConnectIocp() == false) {
		DebugAssertMsg(false, "IOCP 연결 실패 (%d)", ::GetLastError());
	}

}

void UdpClient::FlushSendBuffer() {
	CommandBufferPacket* pWrappedPacket = GetCommandBufferForSending();
	if (pWrappedPacket) SendToAsync(pWrappedPacket, m_RemoteEndPoint);
}

void UdpClient::SetRemoteEndpoint(const IPv4EndPoint& remoteEp) {
	m_RemoteEndPoint = remoteEp;
}

void UdpClient::Connected() {
	// UDP는 연결이라는 개념이 존재하지 않는다. 이 함수는 아무데서도 호출하지 않음
	// 추후 ReliableUDP를 구현하게된다면 활용할 듯?
	m_Socket.State = Socket::eBinded;

	if (m_pEventListener)
		m_pEventListener->OnConnected(this);
}

void UdpClient::ConnectFailed(Int32U errorCode) {
	// UDP는 연결이라는 개념이 존재하지 않는다. 이 함수는 아무데서도 호출하지 않음
	// 추후 ReliableUDP를 구현하게된다면 활용할 듯?

	if (m_pEventListener)
		m_pEventListener->OnConnectFailed(this, errorCode);
}

void UdpClient::Disconnected(Int32U errorCode) {
	// UDP는 연결이 끊긴다는 개념이 존재하지 않는다. 이 함수는 아무데서도 호출하지 않음
	// 추후 ReliableUDP를 구현하게된다면 활용할 듯?
	if (m_pEventListener)
		m_pEventListener->OnDisconnected(this, errorCode);
}

void UdpClient::NotifyCommand(ICommand* cmd) {
	if (m_pEventListener)
		m_pEventListener->OnReceived(this, cmd);
}

void UdpClient::NotifyPacket(RecvedCommandPacket* packet) {
	if (m_pEventListener)
		m_pEventListener->OnReceived(this, packet);
}

void UdpClient::NotifyRaw(char* data, int len) {
	if (m_pEventListener)
		m_pEventListener->OnReceivedRaw(this, data, len);
}

void UdpClient::Sent(IPacket* sentPacket, Int32UL sentBytes) {
	if (m_pEventListener)
		m_pEventListener->OnSent(this, sentPacket, sentBytes);
}

NS_JNET_END
