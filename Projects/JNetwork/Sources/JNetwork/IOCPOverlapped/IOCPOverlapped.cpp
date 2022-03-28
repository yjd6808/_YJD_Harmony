/*
	�ۼ��� : ������
	
*/

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/IOCP/IOCPPostOrder.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>


namespace JNetwork {

	void IOCPOverlapped::Process(BOOL result, DWORD numberOfBytesTransffered, IOCPPostOrder* completionKey) {

	}

	bool IOCPOverlapped::IsFailed(SOCKET hSocket, BOOL result, DWORD numberOfBytesTransffered) {
		// �������� �����߰ų� 0����Ʈ�� ������ ��� ���з� ����

		if (result == FALSE && numberOfBytesTransffered == 0) {

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