/*
 *	작성자 : 윤정도
 */


#include "jnet/Winsock.h"
#include "jnet/Socket.h"

#include "jc/Primitives/StringUtil.h"

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
bool Winsock::Initialize(_u8 _highVersion, _u8 _lowVersion)
{
	const WORD requestVersion = MAKEWORD(_lowVersion, _highVersion);

	WSADATA wsaData;
	const _u32l ret = WSAStartup(requestVersion, &wsaData);
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
		jc_assert_msg(false, _T("UseConnectEx 실패"));
	}

	if (detail::UseDisconnectEx() == false)
	{
		jc_assert_msg(false, _T("UseDisconnectEx 실패"));
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

_u32 Winsock::LastError()
{
	return WSAGetLastError();
}

jc::String Winsock::LastErrorMessage()
{
	return ErrorMessage(::WSAGetLastError());
}

jc::String Winsock::ErrorMessage(_u32 _errorCode)
{
	constexpr int BUF_SIZE = 512;

	jc::String msg{BUF_SIZE};
	// FormatMessage는 UNICODE 기준 매크로(W/A 자동 선택). 버퍼 _char*와 타입이 일치한다.
	DWORD messageLength = FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
	                                    nullptr, _errorCode,
	                                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	                                    msg.Source(), BUF_SIZE, nullptr);

	msg.SetLength(int(messageLength));
	return msg;
}

NS_END
