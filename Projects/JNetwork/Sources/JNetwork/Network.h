/*
 *	작성자 : 윤정도
 *
 *  [내가 꿈꾸던 회사의 면접을 앞둔 정도가 22/05/11 적음]
 *	이 헤더파일을 참조할 시 왠만하면 최상단에 위치하도록 할 것
 *  Windows.h가 이 파일보다 위에서 include 되면 _WINSOCKAPI_ 디파인땜에 충돌발생함 ^^
 *	==> 아니 이제 그럴 필요 없다. Windows.h헤더를 include시키는 파일들 전부 WIN32_LEAN_AND_MEAN 디파인 처리 해둠
 *
 *	테스트해보고 싶다면
 *	TankFighterMain-Client 프로젝트의 main.cpp 최상단에 Windows.h 파일을 포함하고있는 헤더파일 아무거나 include 시킨 후
 *	그 아래 이 헤더 파일 include 해본 후 빌드해보면 알 수 있당!
 */

#pragma once

#pragma comment(lib, "Mswsock.lib")	// AcceptEx 사용을 위해서 추가
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "JCore.lib")


#include <WinSock2.h>
#include <WS2tcpip.h>

#include <sstream>

#define WIN32_LEAN_AND_MEAN			// Windows.h 헤더에 Winsock.h 헤더가 포함되는 것을 막는다. 그 외에 헤더들이 많이 추가되는 것을 방지 // https://social.msdn.microsoft.com/Forums/vstudio/en-US/6177f9bc-db75-4317-9904-02ef55b527d0/linking-problem-with-winsock2?forum=vcgeneral
#include <JCore/Core.h>


#define FORMAT_STR(fmt)  JCore::StringUtil::Format(fmt)
#define MAX_MSS 1460
#define MAX_MTU 1500
#define TEST_DUMMY_PACKET_SIZE	20