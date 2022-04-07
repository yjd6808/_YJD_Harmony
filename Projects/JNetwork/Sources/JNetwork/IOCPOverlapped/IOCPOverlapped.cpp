/*
	작성자 : 윤정도
	
*/

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/IOCP/IOCPPostOrder.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>


namespace JNetwork {
	IOCPOverlapped::IOCPOverlapped(IOCP* iocp, Type type)
		: m_eType(type)
	    , m_pIocp(iocp)
	{
		m_pIocp->AddPendingCount();
	}

	void IOCPOverlapped::Release() {
		m_pIocp->DecreasePendingCount();
		delete this;
	}

	void IOCPOverlapped::Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey) {}

	bool IOCPOverlapped::IsFailed(SOCKET hSocket, BOOL result, DWORD numberOfBytesTransffered) {
		// 오버랩이 실패했거나 0바이트를 수신한 경우 실패로 간주

		if (result == FALSE) {
			// GetQueuedCompletionStatus이 실패한 경우 GetLastError()로 오류 코드를 얻을 수 있다.
			// 하지만 이 코드는 일반적인 윈도우 오류 코드이다. (윈도우 오류 코드 표 : https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes--500-999-)
			// 따라서 WSAGetOverlappedResult함수로 올바른 소켓 오류 코드를 얻어야한다.
			// @참고 : TCP/IP 윈도우 소켓 프로그래밍 463p

			DWORD dwTransfer = 0;
			DWORD dwFlag = 0;
			WSAGetOverlappedResult(hSocket, this, &dwTransfer, FALSE, &dwFlag);
			Winsock::WinsockMessage("오버랩이 실패하였습니다.");
			return true;
		}

		return false;
	}

} // namespace JNetwork