#include <JStudy/Study.h>
#include <JStudy/ServerBase.h>

#define WSA_LOW_VERSION 2
#define WSA_HIGH_VERSION 2


namespace JStudy {
/*
	���� : https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsastartup

	���۰���
	1. ����ϰ����ϴ� ������ ���� ���̺귯���� ���� ������ ��û�Ѵ�.
	2. Winsock DLL���� ������ OS(ȣ����)�� ���������� ���� ������ ���Ͽ� WSADATA�� ��밡���� ���� ����� ��ȯ���ش�.
	�ٷ� ������ ��κ��� OS�� ��� ��� 2.2����(�ֻ���)�� ������ ���� ������ �����Ѵ�.

	�Լ� ���� : ������ ���� ���̺귯�� ����� ���� �ʱ� �۾��� �����մϴ�.
*/
void ServerBase::Initialize()
{
	const WORD wRequestVersion = MAKEWORD(WSA_LOW_VERSION, WSA_HIGH_VERSION);
	
	WSADATA wsaData;

	if (WSAStartup(wRequestVersion, &wsaData) != 0)
		throw std::runtime_error("������ ���� ���̺귯�� �ʱ�ȭ ����");

	const BYTE lowVersion = LOBYTE(wsaData.wVersion);
	const BYTE highVersion = HIBYTE(wsaData.wVersion);

	if (lowVersion != WSA_LOW_VERSION || highVersion != WSA_HIGH_VERSION) {
		WSACleanup();
		throw std::runtime_error("��û�� ���� ������ ��ġ���� �ʽ��ϴ�.");
	}


	std::cout << "2.2���� ���� ���̺귯�� �ʱ�ȭ �Ϸ�" << std::endl;
}

} // namespace JStudy