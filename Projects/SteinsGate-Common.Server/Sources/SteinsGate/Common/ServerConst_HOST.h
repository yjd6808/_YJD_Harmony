/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 2:01:14 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Primitives/Range.h>
#include <SteinsGate/Common/ServerNamespace.h>

NS_CONST_BEGIN


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

	// 세션핸들
	constexpr int SeqUnit = 100000;

	constexpr JCore::Range AuthHandleRange = { SeqUnit * 1, SeqUnit * 2 };		// 100000 ~ 200000
	constexpr JCore::Range LobbyHandleRange = { SeqUnit * 2, SeqUnit * 3 };		// 200000 ~ 300000
	constexpr JCore::Range LogicHandleRange = { SeqUnit * 3, SeqUnit * 4 };		// 300000 ~ 400000
	constexpr JCore::Range AreaHandleRange = { SeqUnit * 4, SeqUnit * 5 };		// 400000 ~ 500000
	constexpr JCore::Range ChatHandleRange = { SeqUnit * 5, SeqUnit * 6 };		// 500000 ~ 600000
	constexpr JCore::Range CenterHandleRange = { SeqUnit * 6, SeqUnit * 7 };	// 600000 ~ 700000
}


NS_CONST_END
