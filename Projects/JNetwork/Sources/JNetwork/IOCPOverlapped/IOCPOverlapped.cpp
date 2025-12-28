/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/IOCP/IOCPPostOrder.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlapped::IOCPOverlapped(IOCP* _pIocp, Type _type)
: OVERLAPPED()
, type_(_type)
, pIocp_(_pIocp)
{
	pIocp_->AddPendingCount();
	DebugAssertMsg(_pIocp->GetState() == IOCP::State::Running, "IOCP가 Running 상태가 아닙니다.");
}

//////////////////////////////////////////////////////////////////////////////////////////
IOCPOverlapped::~IOCPOverlapped()
{
	pIocp_->DecreasePendingCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPOverlapped::Process(BOOL _result, Int32UL _bytesTransferred, IOCPPostOrder* _pCompletionKey)
{
	(void)_result;
	(void)_bytesTransferred;
	(void)_pCompletionKey;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IOCPOverlapped::IsFailed(SOCKET _socket, BOOL _result, Int32UL _bytesTransferred, OUT Int32U& _errorCode)
{
	(void)_bytesTransferred;

	if (_result == FALSE)
	{
		// GetQueuedCompletionStatus이 실패한 경우 GetLastError()로 오류 코드를 얻을 수 있다.
		// 하지만 이 코드는 일반적인 윈도우 오류 코드이다. (윈도우 오류 코드표 : https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes--500-999-)
		// 따라서 WSAGetOverlappedResult함수로 올바른 소켓 오류 코드를 얻어야한다.
		// @참고 : TCP/IP 윈도우 소켓 프로그래밍 463p

		Int32UL transferred = 0;
		Int32UL flag = 0;
		WSAGetOverlappedResult(_socket, this, &transferred, FALSE, &flag);
		_errorCode = Winsock::LastError();
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IOCPOverlapped::IsFailed(BOOL _result, Int32U& _errorCode)
{
	if (_result == FALSE)
	{
		_errorCode = ::GetLastError();
		return true;
	}

	return false;
}

NS_JNET_END
