/*
 * 작성자 : 윤정도
 */

#include "jnet/IOCPOverlapped/IOCPOverlappedSend.h"
#include "jnet/Host/TcpServer.h"

#include "jc/Primitives/RefCountObjectPtr.h"

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
void IOCPOverlappedSend::Process(BOOL _result, _u32l _bytesTransferred, NULLABLE IOCPPostOrder* _pCompletionKey, IOCPWorker* _pWorker)
{
	(void)_pCompletionKey;

	JC_REF_COUNT_GUARD(sentPacket_, false);
	const SOCKET sentSocket = sender_->SocketHandle();
	_u32 errorCode = 0;

	if (IsFailed(sentSocket, _result, _bytesTransferred, errorCode) || _bytesTransferred == 0)
	{
		return;
	}

	sender_->Sent(sentPacket_, _bytesTransferred);
}

NS_END
