/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/IOCP/IOCPPostOrder.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

NS_JNET_BEGIN

IOCPOverlapped::IOCPOverlapped(IOCP* iocp, Type type)
	: OVERLAPPED()
	, m_eType(type)
    , m_pIocp(iocp)
{
	m_pIocp->AddPendingCount();
}

IOCPOverlapped::~IOCPOverlapped() {
	m_pIocp->DecreasePendingCount();
}

// 무조건 제일 마지막에 호출
void IOCPOverlapped::Release() {
	delete this;
}

void IOCPOverlapped::Process(BOOL result, Int32UL numberOfBytesTransffered, IOCPPostOrder* completionKey) {}

bool IOCPOverlapped::IsFailed(SOCKET hSocket, BOOL result, Int32UL numberOfBytesTransffered) {

	if (result == FALSE) {
		// GetQueuedCompletionStatus이 실패한 경우 GetLastError()로 오류 코드를 얻을 수 있다.
		// 하지만 이 코드는 일반적인 윈도우 오류 코드이다. (윈도우 오류 코드표 : https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes--500-999-)
		// 따라서 WSAGetOverlappedResult함수로 올바른 소켓 오류 코드를 얻어야한다.
		// @참고 : TCP/IP 윈도우 소켓 프로그래밍 463p

		Int32UL dwTransfer = 0;
		Int32UL dwFlag = 0;
		WSAGetOverlappedResult(hSocket, this, &dwTransfer, FALSE, &dwFlag);
		return true;
	}

	return false;
}

NS_JNET_END