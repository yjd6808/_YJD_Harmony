/*
 *	작성자 : 윤정도
 * ================
 *
 *	이 헤더파일을 참조할 시 왠만하면 최상단에 위치하도록 할 것
 *  Windows.h가 이 파일보다 위에서 include 되면 _WINSOCKAPI_ 디파인땜에 충돌발생함 ^^
 *	==> 아니 이제 그럴 필요 없다. Windows.h헤더를 include시키는 파일들 전부 WIN32_LEAN_AND_MEAN 디파인 처리 해둠
 *
 *	테스트해보고 싶다면 TankFighterMain-Client 프로젝트의 main.cpp 최상단에 Windows.h 파일을 포함하고있는 헤더파일 아무거나 include 시킨 후
 *	그 아래 이 헤더 파일 include 해본 후 빌드해보면 알 수 있당!
 */

#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <sstream>

#include <JCore/Core.h>

#define MAX_MSS 1460
#define MAX_MTU 1500
#define TEST_DUMMY_PACKET_SIZE	20