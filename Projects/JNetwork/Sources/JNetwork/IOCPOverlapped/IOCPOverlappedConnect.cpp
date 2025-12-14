/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedConnect.h>
#include <JNetwork/Host/TcpServer.h>

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlappedConnect::IOCPOverlappedConnect(TcpClient* _pClient, IOCP* _pIocp, IPacket* _pSentPacket)
: IOCPOverlapped(_pIocp, Type::Connect)
, client_(_pClient)
, sentPacket_(_pSentPacket)
{
	client_->AddPendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlappedConnect::~IOCPOverlappedConnect()
{
	client_->DecreasePendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPOverlappedConnect::Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey)
{
	(void)_pCompletionKey;

	const SOCKET connectedSocket = client_->SocketHandle();
	Int32U errorCode = 0;
	if (IsFailed(connectedSocket, _result, _bytesTransferred, errorCode))
	{
		client_->Disconnect();
		client_->ConnectFailed(errorCode);
#if TEST_DUMMY_PACKET_TRANSFER
		sentPacket_->Release();
#endif
		return;
	}

	client_->Connected();

	if (sentPacket_)
	{
		client_->Sent(sentPacket_, _bytesTransferred);
		sentPacket_->Release();
	}

	if (!client_->RecvAsync())
	{
		client_->Disconnect();
	}
}

NS_JNET_END
