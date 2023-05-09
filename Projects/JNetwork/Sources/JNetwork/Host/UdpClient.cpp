/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 2:33:47 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Host/UdpClient.h>

#include <JNetwork/Packet/RecvPacket.h>
#include <JNetwork/Packet/SendPacket.h>

NS_JNET_BEGIN
USING_NS_JC;

UdpClient::UdpClient(
	const IOCPPtr& iocp,
	const JCore::MemoryPoolAbstractPtr& bufferAllocator,
	ClientEventListener* listener, 
	int recvBufferSize, 
	int sendBufferSize
)
	: Session(iocp, bufferAllocator, recvBufferSize, sendBufferSize)
	, m_pClientEventListener(listener)
{
	UdpClient::Initialize();
}

UdpClient::~UdpClient() {
	Disconnect();
}


void UdpClient::Initialize() {
	Session::Initialize();

	if (CreateSocket(TransportProtocol::UDP, true) == false) {
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
	m_pClientEventListener->OnConnected(this);
}

void UdpClient::Disconnected() {
	// UDP는 연결이 끊긴다는 개념이 존재하지 않는다. 이 함수는 아무데서도 호출하지 않음
	// 추후 ReliableUDP를 구현하게된다면 활용할 듯?
	m_pClientEventListener->OnDisconnected(this);
}

void UdpClient::NotifyCommand(ICommand* cmd) {
	m_pClientEventListener->OnReceived(this, cmd);
}

void UdpClient::Sent(ISendPacket* sentPacket, Int32UL sentBytes) {
	m_pClientEventListener->OnSent(this, sentPacket, sentBytes);
}

NS_JNET_END
