/*
	�ۼ��� : ������
	
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
		// �������� �����߰ų� 0����Ʈ�� ������ ��� ���з� ����

		if (result == FALSE) {
			// GetQueuedCompletionStatus�� ������ ��� GetLastError()�� ���� �ڵ带 ���� �� �ִ�.
			// ������ �� �ڵ�� �Ϲ����� ������ ���� �ڵ��̴�. (������ ���� �ڵ� ǥ : https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes--500-999-)
			// ���� WSAGetOverlappedResult�Լ��� �ùٸ� ���� ���� �ڵ带 �����Ѵ�.
			// @���� : TCP/IP ������ ���� ���α׷��� 463p

			DWORD dwTransfer = 0;
			DWORD dwFlag = 0;
			WSAGetOverlappedResult(hSocket, this, &dwTransfer, FALSE, &dwFlag);
			Winsock::WinsockMessage("�������� �����Ͽ����ϴ�.");
			return true;
		}

		return false;
	}

} // namespace JNetwork