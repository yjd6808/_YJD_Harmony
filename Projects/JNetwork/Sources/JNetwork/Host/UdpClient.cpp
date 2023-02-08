/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 2:33:47 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Host/UdpClient.h>
#include <JNetwork/Packet/Packet.h>

#include <JNetwork/IOCPOverlapped/IOCPOverlappedSendTo.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecvFrom.h>

NS_JNET_BEGIN
USING_NS_JC;

UdpClient::UdpClient(const IOCPPtr& iocp, ClientEventListener* listener, int recvBufferSize, int sendBufferSize)
	: Session(iocp, recvBufferSize, sendBufferSize)
	, m_pClientEventListener(listener)
{
	if (!Winsock::IsInitialized()) {
		DebugAssertMsg(false, "윈속 초기화를 먼저 해주세요. Winsock::Initialize()");
	}

	if (CreateSocket(TransportProtocol::UDP) == false) {
		DebugAssertMsg(false, "UDP 소켓 생성에 실패했습니다.");
	}
}

UdpClient::~UdpClient() {}


bool UdpClient::RecvFromAsync() {
	WSABUF buf = m_pRecvBuffer->GetRemainBuffer();
	Int32UL uiReceivedBytes = 0;

	IOCPOverlappedRecvFrom* pRecvFromOverlapped = new IOCPOverlappedRecvFrom(this, m_spIocp.GetPtr());

	const int iResult = m_Socket.ReceiveFromEx(
		&buf, 
		&uiReceivedBytes, 
		pRecvFromOverlapped, 
		pRecvFromOverlapped->SenderAddrPtr(), 
		pRecvFromOverlapped->SenderAddrLenPtr());

	if (iResult == SOCKET_ERROR) {
		if (Winsock::LastError() != WSA_IO_PENDING) {
			DebugAssertMsg(false, "RecvFromAsync() 실패 (%d:%s)", Winsock::LastError(), Winsock::LastErrorMessage().Source());
			return false;
		}
	}
	return true;
}

bool UdpClient::SendToAsync(ISendPacket* packet, const IPv4EndPoint& destination) {

	packet->AddRef();
	WSABUF buf = packet->GetWSABuf();
	Int32UL uiSendBytes = 0;
	IOCPOverlapped* pOverlapped = new IOCPOverlappedSendTo(this, m_spIocp.GetPtr(), packet);

	const int iResult = m_Socket.SendToEx(&buf, &uiSendBytes, pOverlapped, destination);
	if (iResult == SOCKET_ERROR) {
		if (Winsock::LastError() != WSA_IO_PENDING) {
			DebugAssertMsg(false, "SendAsync() 실패");
			pOverlapped->Release();
			packet->Release();
			return false;
		}
	}

	return true;
}

void UdpClient::Connected() {
	m_eSessionState = eConnected;
	m_pClientEventListener->OnConnected();
}

void UdpClient::Received(Int32UL receivedBytes, IOCPOverlappedRecvFrom* recvFrom) {
	m_RemoteEndPoint = { *recvFrom->SenderAddrPtr() };
	Session::Received(receivedBytes);
}

void UdpClient::NotifyCommand(ICommand* cmd) {
	m_pClientEventListener->OnReceived(cmd);
}

void UdpClient::Sent(ISendPacket* sentPacket, Int32UL sentBytes) {
	m_pClientEventListener->OnSent(sentPacket, sentBytes);
}

NS_JNET_END
