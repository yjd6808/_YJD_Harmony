#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <MSWSock.h>


namespace JNetwork {

	JCore::CriticalSectionMutex Winsock::s_consoleLock;

	bool Winsock::ms_bFinalized = false;
	bool Winsock::ms_bInitailized = false;

/*
	���� : https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsastartup

	���۰���
	1. ����ϰ����ϴ� ������ ���� ���̺귯���� ���� ������ ��û�Ѵ�.
	2. Winsock DLL���� ������ OS(ȣ����)�� ���������� ���� ������ ���Ͽ� WSADATA�� ��밡���� ���� ����� ��ȯ���ش�.
	�ٷ� ������ ��κ��� OS�� ��� ��� 2.2����(�ֻ���)�� ������ ���� ������ �����Ѵ�.

	�Լ� ���� : ������ ���� ���̺귯�� ����� ���� �ʱ� �۾��� �����մϴ�.
*/
bool Winsock::Initialize(Byte highVersion, Byte lowVersion) {
	const WORD wRequestVersion = MAKEWORD(lowVersion, highVersion);

	WSADATA wsaData;
	const Int32UL ret = WSAStartup(wRequestVersion, &wsaData);
	WSASetLastError(ret);

	const BYTE retLowVersion = LOBYTE(wsaData.wVersion);
	const BYTE retHighVersion = HIBYTE(wsaData.wVersion);

	if (retLowVersion != lowVersion || retHighVersion != highVersion) {
		WSACleanup();
		return false;
	}

	return ms_bInitailized = true;
}

// ���� ����� �����ϸ鼭 �Ҵ�� ���ҽ� ������ �����Ѵ�.
bool Winsock::Finalize() {
	if (WSACleanup() != 0) {
		return false;
	}

	return ms_bFinalized = true;
}

Int32U Winsock::LastError() {
	return WSAGetLastError();
}

JCore::String Winsock::LastErrorMessage() {
	char buf[1024];

	const Int32U errorCode = WSAGetLastError();

	FormatMessageA(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buf, 1024, NULL);
	
	JCore::String ret = buf;
	return ret + "(" + errorCode + ")";
}

}

