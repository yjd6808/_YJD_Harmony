/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 2:33:47 PM
 * =====================
 *
 */


#include <jnet/Core.h>
#include <jnet/Winsock.h>
#include <jnet/Host/UdpClient.h>

NS_JNET_BEGIN
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UdpClient::UdpClient(
	const IOCPPtr& _pIocp,
	const MemoryPoolAbstractPtr& _pBufferAllocator,
	PacketParser* _pParser,
	int _recvBufferSize,
	int _sendBufferSize)
: Session(_pIocp, _pBufferAllocator, _pParser, _recvBufferSize, _sendBufferSize)
, pEventListener_(nullptr)
{
	UdpClient::Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
UdpClient::~UdpClient()
{
	Disconnect();
	WaitForZeroPending();

	JC_DELETE_SAFE(pEventListener_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UdpClient::Initialize()
{
	Session::Initialize();

	if (!CreateSocket(TransportProtocol::UDP, NonblokingSocket))
	{
		jc_assert_msg(false, "UDP 소켓 생성에 실패했습니다. (%u)", Winsock::LastError());
	}

	if (!ConnectIocp())
	{
		jc_assert_msg(false, "IOCP 연결 실패 (%d)", ::GetLastError());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UdpClient::FlushSendBuffer()
{
	CommandBufferPacket* pWrappedPacket = GetCommandBufferForSending();
	if (pWrappedPacket)
	{
		SendToAsync(pWrappedPacket, remoteEndPoint_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UdpClient::SetRemoteEndpoint(const IPv4EndPoint& _remoteEndPoint)
{
	remoteEndPoint_ = _remoteEndPoint;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UdpClient::Connected()
{
	// UDP는 연결이라는 개념이 존재하지 않는다. 이 함수는 아무데서도 호출하지 않음
	// 추후 ReliableUDP를 구현하게된다면 활용할 듯?
	socket_.State = Socket::eBinded;

	if (pEventListener_)
	{
		pEventListener_->OnConnected(this);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UdpClient::ConnectFailed(Int32U _errorCode)
{
	// UDP는 연결이라는 개념이 존재하지 않는다. 이 함수는 아무데서도 호출하지 않음
	// 추후 ReliableUDP를 구현하게된다면 활용할 듯?

	if (pEventListener_)
	{
		pEventListener_->OnConnectFailed(this, _errorCode);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UdpClient::Disconnected(Int32U _errorCode)
{
	// UDP는 연결이 끊긴다는 개념이 존재하지 않는다. 이 함수는 아무데서도 호출하지 않음
	// 추후 ReliableUDP를 구현하게된다면 활용할 듯?
	if (pEventListener_)
	{
		pEventListener_->OnDisconnected(this, _errorCode);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UdpClient::NotifyCommand(ICommand* _pCmd)
{
	if (pEventListener_)
	{
		pEventListener_->OnReceived(this, _pCmd);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UdpClient::NotifyPacket(RecvedCommandPacket* _pPacket)
{
	if (pEventListener_)
	{
		pEventListener_->OnReceived(this, _pPacket);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UdpClient::NotifyRaw(char* _pData, int _len)
{
	if (pEventListener_)
	{
		pEventListener_->OnReceivedRaw(this, _pData, _len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UdpClient::Sent(IPacket* _pSentPacket, Int32UL _sentBytes)
{
	if (pEventListener_)
	{
		pEventListener_->OnSent(this, _pSentPacket, _sentBytes);
	}
}

NS_JNET_END
