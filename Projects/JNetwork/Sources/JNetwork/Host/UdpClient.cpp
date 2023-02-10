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

	if (CreateSocket(TransportProtocol::UDP) == false) {
		DebugAssertMsg(false, "UDP 소켓 생성에 실패했습니다.");
	}

	if (ConnectIocp() == false) {
		DebugAssertMsg(false, "IOCP 연결 실패 (%d)", ::GetLastError());
	}

}

bool UdpClient::RecvFromAsync() {

	DebugAssertMsg(m_Socket.IsBinded(), "소켓이 바인딩된 상태여야 수신이 가능합니다. 상대방에게 먼저 송신하여 오토 바인딩해주거나 수동 바인딩을 해주세요.");

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
			DebugAssertMsg(false, "RecvFromAsync() 실패 (%d)", uiErrorCode);
			return false;
		}
	}
	return true;
}

bool UdpClient::SendToAsync(ISendPacket* packet, const IPv4EndPoint& destination) {
	DebugAssertMsg(destination.IsValidRemoteEndPoint(), "유효한 목적지 주소가 아닙니다.");

	packet->AddRef();
	WSABUF buf = packet->GetWSABuf();
	Int32UL uiSendBytes = 0;
	IOCPOverlapped* pOverlapped = dbg_new IOCPOverlappedSendTo(this, m_spIocp.GetPtr(), packet);

	const int iResult = m_Socket.SendToEx(&buf, &uiSendBytes, pOverlapped, destination);
	if (iResult == SOCKET_ERROR) {
		Int32U uiErrorCode = Winsock::LastError();
		if (uiErrorCode != WSA_IO_PENDING) {
			DebugAssertMsg(false, "SendToAsync() 실패 (%d)", uiErrorCode);
			pOverlapped->Release();
			packet->Release();
			return false;
		}
	}

	return true;
}

bool UdpClient::SendToAsyncEcho(ISendPacket* packet) {
	return SendToAsync(packet, m_RemoteEndPoint);
}

void UdpClient::FlushSendBuffer() {
	DebugAssertMsg(m_RemoteEndPoint.IsValidRemoteEndPoint(), "유효한 목적지 주소가 아닙니다.");

	CommandBufferPacket* pWrappedPacket = GetCommandBufferForSending();
	if (pWrappedPacket) {
		SendToAsync(pWrappedPacket, m_RemoteEndPoint);
	}
}

void UdpClient::Connected() {
	m_eState = eConnected;
	m_Socket.State = Socket::eBinded;
	m_pClientEventListener->OnConnected();
}

void UdpClient::Disconnected() {
	m_pClientEventListener->OnDisconnected();
}

void UdpClient::NotifyCommand(ICommand* cmd) {
	m_pClientEventListener->OnReceived(cmd);
}

void UdpClient::Sent(ISendPacket* sentPacket, Int32UL sentBytes) {
	m_pClientEventListener->OnSent(sentPacket, sentBytes);
}

NS_JNET_END
