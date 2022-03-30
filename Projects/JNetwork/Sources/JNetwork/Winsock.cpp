#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <MSWSock.h>


namespace JNetwork {

	JCore::CriticalSectionMutex Winsock::s_consoleLock;

	bool Winsock::ms_bFinalized = false;
	bool Winsock::ms_bInitailized = false;

/*
	참고 : https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsastartup

	동작과정
	1. 사용하고자하는 윈도우 소켓 라이브러리의 버전 정보를 요청한다.
	2. Winsock DLL에서 윈도우 OS(호출자)와 지원가능한 버전 정보와 비교하여 WSADATA에 사용가능한 버전 결과를 반환해준다.
	근래 유명한 대부분의 OS의 경우 모두 2.2버전(최상위)의 윈도우 소켓 버전을 지원한다.

	함수 설명 : 윈도우 소켓 라이브러리 사용을 위한 초기 작업을 진행합니다.
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

// 윈속 사용을 종료하면서 할당된 리소스 정보를 해제한다.
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

