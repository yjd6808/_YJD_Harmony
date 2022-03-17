#include <JStudy/Study.h>
#include <JStudy/ServerBase.h>

#define WSA_LOW_VERSION 2
#define WSA_HIGH_VERSION 2


namespace JStudy {
/*
	참고 : https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsastartup

	동작과정
	1. 사용하고자하는 윈도우 소켓 라이브러리의 버전 정보를 요청한다.
	2. Winsock DLL에서 윈도우 OS(호출자)와 지원가능한 버전 정보와 비교하여 WSADATA에 사용가능한 버전 결과를 반환해준다.
	근래 유명한 대부분의 OS의 경우 모두 2.2버전(최상위)의 윈도우 소켓 버전을 지원한다.

	함수 설명 : 윈도우 소켓 라이브러리 사용을 위한 초기 작업을 진행합니다.
*/
void ServerBase::Initialize()
{
	const WORD wRequestVersion = MAKEWORD(WSA_LOW_VERSION, WSA_HIGH_VERSION);
	
	WSADATA wsaData;

	if (WSAStartup(wRequestVersion, &wsaData) != 0)
		throw std::runtime_error("윈도우 소켓 라이브러리 초기화 실패");

	const BYTE lowVersion = LOBYTE(wsaData.wVersion);
	const BYTE highVersion = HIBYTE(wsaData.wVersion);

	if (lowVersion != WSA_LOW_VERSION || highVersion != WSA_HIGH_VERSION) {
		WSACleanup();
		throw std::runtime_error("요청한 버전 정보와 일치하지 않습니다.");
	}


	std::cout << "2.2버전 윈속 라이브러리 초기화 완료" << std::endl;
}

} // namespace JStudy