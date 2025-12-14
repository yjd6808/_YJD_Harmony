/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSend.h>
#include <JNetwork/Host/TcpServer.h>

#include <JCore/Primitives/RefCountObjectPtr.h>

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlappedSend::IOCPOverlappedSend(Session* _pSession, IOCP* _pIocp, IPacket* _pSentPacket)
: IOCPOverlapped(_pIocp, Type::Send)
, sender_(_pSession)
, sentPacket_(_pSentPacket)
{
	sender_->AddPendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlappedSend::~IOCPOverlappedSend()
{
	sender_->DecreasePendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPOverlappedSend::Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey)
{
	(void)_pCompletionKey;

	JCORE_REF_COUNT_GUARD(sentPacket_, false);
	const SOCKET sentSocket = sender_->SocketHandle();
	Int32U errorCode = 0;

	if (IsFailed(sentSocket, _result, _bytesTransferred, errorCode) || _bytesTransferred == 0)
	{
		return;
	}

	sender_->Sent(sentPacket_, _bytesTransferred);
}

NS_JNET_END
