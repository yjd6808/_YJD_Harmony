/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 6:04:09 PM
 * =====================
 *
 */

#include <jnet/Core.h>
#include <jnet/IOCPOverlapped/IOCPOverlappedSendTo.h>

#include <jc/Primitives/RefCountObjectPtr.h>

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlappedSendTo::IOCPOverlappedSendTo(Session* _pSender, IOCP* _pIocp, IPacket* _pSentPacket)
: IOCPOverlapped(_pIocp, Type::SendTo)
, sender_(_pSender)
, sentPacket_(_pSentPacket)
{
	sender_->AddPendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlappedSendTo::~IOCPOverlappedSendTo()
{
	sender_->DecreasePendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPOverlappedSendTo::Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey)
{
	(void)_pCompletionKey;

	JC_REF_COUNT_GUARD(sentPacket_, false);
	const SOCKET sentSocket = sender_->SocketHandle();
	Int32U errorCode = 0;

	if (IsFailed(sentSocket, _result, _bytesTransferred, errorCode) || _bytesTransferred == 0)
	{
		return;
	}

	sender_->Sent(sentPacket_, _bytesTransferred);
}

NS_JNET_END
