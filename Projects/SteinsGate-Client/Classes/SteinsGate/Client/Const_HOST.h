/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 5:02:45 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Namespace.h>

NS_CONST_BEGIN

namespace Host
{
	// 네트그룹에 저장하는 용도, 전역변수에 저장하고 사용하므로 중요치 않음
	constexpr int AuthTcpId = 0;
	constexpr int LobbyTcpId = 1;
	constexpr int LogicTcpId = 2;
	constexpr int AreaTcpId = 3;
	constexpr int ChatTcpId = 4;

	constexpr int AuthUdpId = 10;
	constexpr int LobbyUdpId = 11;
	constexpr int LogicUdpId = 12;
	constexpr int AreaUdpId = 13;
	constexpr int ChatUdpId = 14;
}


NS_CONST_END