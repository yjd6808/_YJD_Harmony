/*
 * 작성자: 윤정도
 * 생성일: 5/13/2023 3:12:18 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Namepsace.h>

NS_CONST_BEGIN

namespace Process
{
	constexpr int LoopPerSecondAuth = 10;
	constexpr int LoopPerSecondLobby = 5;
	constexpr int LoopPerSecondGame = 4;
	constexpr int LoopPerSecondCenter = 10;
}

namespace NetGroup
{
	// NetMaster에 추가할때 지정할 ID, 크게 신경안써도됨
	constexpr int MainId = 0;
	constexpr int InterServerId = 1;
}

namespace Host
{
	// NetGroup에 추가할때 지정할 ID, 크게 신경안써도됨
	constexpr int AuthTcpId = 0;
	constexpr int AuthUdpId = 1;
	constexpr int AuthInterServerTcpId = 2;
	constexpr int AuthInterServerUdpId = 3;

	constexpr int LobbyTcpId = 10;
	constexpr int LobbyUdpId = 11;
	constexpr int LobbyInterServerTcpId = 12;
	constexpr int LobbyInterServerUdpId = 13;

	constexpr int CenterTcpId = 20;
	constexpr int CenterUdpId = 21;
	constexpr int CenterInterServerTcpId = 22;
	constexpr int CenterInterServerUdpId = 23;

	constexpr int LogicTcpId = 30;
	constexpr int LogicUdpId = 31;
	constexpr int AreaTcpId = 32;
	constexpr int AreaUdpId = 33;
	constexpr int ChatTcpId = 34;
	constexpr int ChatUdpId = 35;
	constexpr int GameInterServerTcpId = 36;
	constexpr int GameInterServerUdpId = 37;
}

NS_CONST_END
