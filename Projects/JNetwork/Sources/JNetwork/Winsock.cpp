/*
 *	작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Socket.h>

#include "JCore/Primitives/StringUtil.h"

NS_JNET_BEGIN
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

	if (Detail::UseConnectEx() == false) {
		DebugAssertMsg(false, "UseConnectEx 실패");
	}

	if (Detail::UseDisconnectEx() == false) {
		DebugAssertMsg(false, "UseDisconnectEx 실패");
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

JCore::String Winsock::LastErrorMessageUTF8() {
	return ErrorMessageUTF8(::WSAGetLastError());
}

JCore::String Winsock::ErrorMessageMBCS(Int32U errorCode) {
	constexpr int BUF_SIZE = 512;

	JCore::String mbcsString{ BUF_SIZE };
	char* pSource = mbcsString.Source();
	DWORD dwLength = FormatMessageA(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		pSource, BUF_SIZE, NULL);

	mbcsString.SetLength(int(dwLength));
	return mbcsString;
}

JCore::String Winsock::ErrorMessageUTF8(Int32U errorCode) {
	// MBCS -> UTF16 -> UTF8보다는 바로 UTF16 -> UTF8로 변환이 당연히 낫겟지?
	// 근데, FormatMessage에서 곧바로 UTF8 문자열을 반환 받는 방법은 없나..

	constexpr int BUF_SIZE_UNICODCE = 512;

	wchar_t buf[BUF_SIZE_UNICODCE];

	FormatMessageW(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buf, BUF_SIZE_UNICODCE, NULL);

	
	int iRequiredLength = 0;
	// 먼저 필요한 버퍼 크기를 얻는다.
	if ((iRequiredLength = WideCharToMultiByte(CP_UTF8, 0, buf, BUF_SIZE_UNICODCE, nullptr, 0, NULL, NULL)) == 0) {
		// @에러코드 표: https://learn.microsoft.com/ko-kr/windows/win32/debug/system-error-codes--0-499-
		DebugAssertMsg(false, "%d", ::GetLastError());
		return {};
	}

	JCore::String utf8String{ iRequiredLength + 1 };
	char* pSource = utf8String.Source();

	if (WideCharToMultiByte(CP_UTF8, 0, buf, BUF_SIZE_UNICODCE, pSource, iRequiredLength, NULL, NULL) == 0) {
		DebugAssertMsg(false, "%d", ::GetLastError());
		return {};
	}

	pSource[iRequiredLength] = '\0';
	utf8String.SetLength(iRequiredLength);
	return utf8String;
}

NS_JNET_END

