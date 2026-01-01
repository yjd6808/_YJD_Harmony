/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 6:04:33 PM
 * =====================
 *
 */

#include <jnet/Core.h>
#include <jnet/IOCPOverlapped/IOCPOverlappedRecvFrom.h>

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlappedRecvFrom::IOCPOverlappedRecvFrom(Session* _pSession, IOCP* _pIocp)
: IOCPOverlapped(_pIocp, Type::ReceiveFrom)
, receiver_(_pSession)
{
	receiver_->AddPendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlappedRecvFrom::~IOCPOverlappedRecvFrom()
{
	receiver_->DecreasePendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPOverlappedRecvFrom::Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey)
{
	(void)_pCompletionKey;

	const SOCKET receiveSocket = receiver_->SocketHandle();
	Int32U errorCode = 0;
	if (IsFailed(receiveSocket, _result, _bytesTransferred, errorCode) || _bytesTransferred == 0)
	{
		return;
	}

	receiver_->Received(_bytesTransferred);

	// TODO: 리시브 오버랩 재사용 기능 구현
	if (!receiver_->RecvFromAsync())
	{
	}
}

NS_JNET_END
