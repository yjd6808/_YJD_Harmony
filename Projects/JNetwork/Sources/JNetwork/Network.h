/*
 *	작성자 : 윤정도
 */

#pragma once

#pragma comment(lib, "Mswsock.lib")	// AcceptEx 사용을 위해서 추가
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "JCore.lib")

#define _WINSOCKAPI_
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <sstream>

#define WIN32_LEAN_AND_MEAN			// Windows.h 헤더에 Winsock.h 헤더가 포함되는 것을 막는다. 그 외에 헤더들이 많이 추가되는 것을 방지 // https://social.msdn.microsoft.com/Forums/vstudio/en-US/6177f9bc-db75-4317-9904-02ef55b527d0/linking-problem-with-winsock2?forum=vcgeneral
#include <JCore/Core.h>


#define FORMAT_STR(fmt)  JCore::StringUtil::Format(fmt)
#define MAX_MSS 1460
#define MAX_MTU 1500
#define TEST_DUMMY_PACKET_SIZE	20