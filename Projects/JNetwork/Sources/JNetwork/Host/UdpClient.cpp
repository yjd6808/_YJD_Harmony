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

#include <JNetwork/IOCPOverlapped/IOCPOverlappedSendTo.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecvFrom.h>

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

bool UdpClient::RecvFromAsync() {

	if (!m_Socket.IsBinded()) {
		_NetLogError_("소켓이 바인딩된 상태여야 수신이 가능합니다. 상대방에게 먼저 송신하여 오토 바인딩해주거나 수동 바인딩을 해주세요.");
		return false;
	}

	WSABUF buf = m_spRecvBuffer->GetRemainBuffer();
	Int32UL uiReceivedBytes = 0;
	IOCPOverlappedRecvFrom* pRecvFromOverlapped = dbg_new IOCPOverlappedRecvFrom(this, m_spIocp.GetPtr());

	const int iResult = m_Socket.ReceiveFromEx(
		&buf, 
		&uiReceivedBytes, 
		pRecvFromOverlapped, 
		&m_RemoteEndPoint.InternetAddr
	);

	if (iResult == SOCKET_ERROR) {
		Int32U uiErrorCode = Winsock::LastError();
		if (uiErrorCode != WSA_IO_PENDING) {
			DebugAssertMsg(false, "RecvFromAsync 실패 (%d)", uiErrorCode);
			return false;
		}
	}

	

	return true;
}

bool UdpClient::SendToAsync(ISendPacket* packet, const IPv4EndPoint& destination) {

	if (!destination.IsValidRemoteEndPoint()) {
		_NetLogError_("유효한 목적지 주소가 아닙니다.");
		return false;
	}

	packet->AddRef();
	WSABUF buf = packet->GetWSABuf();
	Int32UL uiSendBytes = 0;
	IOCPOverlapped* pOverlapped = dbg_new IOCPOverlappedSendTo(this, m_spIocp.GetPtr(), packet);

	const int iResult = m_Socket.SendToEx(&buf, &uiSendBytes, pOverlapped, destination);
	if (iResult == SOCKET_ERROR) {
		Int32U uiErrorCode = Winsock::LastError();
		if (uiErrorCode != WSA_IO_PENDING) {
			DebugAssertMsg(false, "SendToAsync 실패 (%d)", uiErrorCode);
			pOverlapped->Release();
			packet->Release();
			return false;
		}
	}

	return true;
}

bool UdpClient::SendToAsync(const CommandBufferPtr& buffer, const IPv4EndPoint& destination) {
#ifdef DebugMode
	DebugAssertMsg(buffer->IsValid(), "보내고자하는 커맨드 버퍼 데이터가 이상합니다.");
#endif
	return SendToAsync(new CommandBufferPacket(buffer), destination);
}

bool UdpClient::SendToAsyncEcho(ISendPacket* packet) {
	return SendToAsync(packet, m_RemoteEndPoint);
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
	m_pClientEventListener->OnConnected();
}

void UdpClient::Disconnected() {
	// UDP는 연결이 끊긴다는 개념이 존재하지 않는다. 이 함수는 아무데서도 호출하지 않음
	// 추후 ReliableUDP를 구현하게된다면 활용할 듯?
	m_pClientEventListener->OnDisconnected();
}

void UdpClient::NotifyCommand(ICommand* cmd) {
	m_pClientEventListener->OnReceived(cmd);
}

void UdpClient::Sent(ISendPacket* sentPacket, Int32UL sentBytes) {
	m_pClientEventListener->OnSent(sentPacket, sentBytes);
}

NS_JNET_END
