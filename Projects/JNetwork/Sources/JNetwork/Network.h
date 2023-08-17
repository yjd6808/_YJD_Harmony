/*
 *	작성자 : 윤정도
 * ================
 *
 *	이 헤더파일을 참조할 시 왠만하면 최상단에 위치하도록 할 것
 *  Windows.h가 이 파일보다 위에서 include 되면 _WINSOCKAPI_ 디파인땜에 충돌발생함 ^^
 *	==> 아니 이제 그럴 필요 없다. Windows.h헤더를 include시키는 파일들 전부 WIN32_LEAN_AND_MEAN 디파인 처리 해둠
 */

#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <sstream>

#include <JCore/Core.h>
#include <JNetwork/NetLoggerDefine.h>
#include <JNetwork/Namespace.h>

#define MAX_MSS 1460
#define MAX_MTU 1500

#define TEST_DUMMY_PACKET_TRANSFER 0
#define TEST_DUMMY_PACKET_SIZE	20

NS_JNET_BEGIN
inline int DbgFlag_v = 0;	// 외부 프로젝트에서 JNetwork 프로젝트에 브레이크 포인트 걸기위해 추가함.
NS_JNET_END