/*
 *	작성자 : 윤정도
 */


#include <jnet/Core.h>
#include <jnet/Winsock.h>
#include <jnet/Socket.h>

#include <jc/Primitives/StringUtil.h>

NS_JNET_BEGIN
bool Winsock::Finalized = false;
bool Winsock::Initialized = false;

/*
	참고 : https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsastartup

	동작과정
	1. 사용하고자하는 윈도우 소켓 라이브러리의 버전 정보를 요청한다.
	2. Winsock DLL에서 윈도우 OS(호출자)와 지원가능한 버전 정보와 비교하여 WSADATA에 사용가능한 버전 결과를 반환해준다.
	근래 유명한 대부분의 OS의 경우 모두 2.2버전(최상위)의 윈도우 소켓 버전을 지원한다.

	함수 설명 : 윈도우 소켓 라이브러리 사용을 위한 초기 작업을 진행합니다.
*/
bool Winsock::Initialize(Byte _highVersion, Byte _lowVersion)
{
	const WORD requestVersion = MAKEWORD(_lowVersion, _highVersion);

	WSADATA wsaData;
	const Int32UL ret = WSAStartup(requestVersion, &wsaData);
	WSASetLastError(ret);

	const BYTE retLowVersion = LOBYTE(wsaData.wVersion);
	const BYTE retHighVersion = HIBYTE(wsaData.wVersion);

	if (retLowVersion != _lowVersion || retHighVersion != _highVersion)
	{
		WSACleanup();
		return false;
	}

	if (detail::UseConnectEx() == false)
	{
		jc_assert_msg(false, "UseConnectEx 실패");
	}

	if (detail::UseDisconnectEx() == false)
	{
		jc_assert_msg(false, "UseDisconnectEx 실패");
	}

	return Initialized = true;
}

// 윈속 사용을 종료하면서 할당된 리소스 정보를 해제한다.
bool Winsock::Finalize()
{
	if (WSACleanup() != 0)
	{
		return false;
	}

	return Finalized = true;
}

Int32U Winsock::LastError()
{
	return WSAGetLastError();
}

jc::String Winsock::LastErrorMessageUTF8()
{
	return ErrorMessageUTF8(::WSAGetLastError());
}

jc::String Winsock::ErrorMessageMBCS(Int32U _errorCode)
{
	constexpr int BUF_SIZE = 512;

	jc::String mbcsString{BUF_SIZE};
	char* pSource = mbcsString.Source();
	DWORD messageLength = FormatMessageA(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
	                                     nullptr, _errorCode,
	                                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	                                     pSource, BUF_SIZE, nullptr);

	mbcsString.SetLength(int(messageLength));
	return mbcsString;
}

jc::String Winsock::ErrorMessageUTF8(Int32U _errorCode)
{
	// MBCS -> UTF16 -> UTF8보다는 바로 UTF16 -> UTF8로 변환이 당연히 낫겟지?
	// 근데, FormatMessage에서 곧바로 UTF8 문자열을 반환 받는 방법은 없나..

	constexpr int BUF_SIZE_UNICODCE = 512;

	wchar_t buf[BUF_SIZE_UNICODCE];

	FormatMessageW(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
	               nullptr, _errorCode,
	               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	               buf, BUF_SIZE_UNICODCE, nullptr);

	int requiredLength = 0;
	// 먼저 필요한 버퍼 크기를 얻는다.
	if ((requiredLength = WideCharToMultiByte(CP_UTF8, 0, buf, BUF_SIZE_UNICODCE, nullptr, 0, nullptr, nullptr)) == 0)
	{
		// @에러코드 표: https://learn.microsoft.com/ko-kr/windows/win32/debug/system-error-codes--0-499-
		jc_assert_msg(false, "%d", ::GetLastError());
		return {};
	}

	jc::String utf8String{requiredLength + 1};
	char* pSource = utf8String.Source();

	if (WideCharToMultiByte(CP_UTF8, 0, buf, BUF_SIZE_UNICODCE, pSource, requiredLength, nullptr, nullptr) == 0)
	{
		jc_assert_msg(false, "%d", ::GetLastError());
		return {};
	}

	pSource[requiredLength] = '\0';
	utf8String.SetLength(requiredLength);
	return utf8String;
}

NS_JNET_END
