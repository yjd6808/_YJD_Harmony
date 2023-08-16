/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 9:54:20 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Pair.h>
#include <JCore/Primitives/String.h>

#include <SteinsGate/Common/Enum.h>

// 메인 서버가 하나만 존재하는 서버 타입 (게임서버는 여러개 존재할 수 있으므로)
SEnumBegin(SingleServerType)
Center,
Begin = Center,
Auth,
Lobby,
End = Lobby,
Max
SEnumEnd(SingleServerType)

// Center 서버를 경유해 릴레이통신을 수행하는 서버들
SEnumBegin(InterServerClientType)
None,
Auth,
Begin = Auth,
Lobby,
Game,
End = Game,
Max
SEnumMiddle(InterServerClientType)
static constexpr const char* Name[Max]{
	"",
	"인증",
	"로비",
	"게임",
};

SEnumMiddleEnd(InterServerClientType)

SEnumBegin(CenterOrder)
RebootServer,
LaunchServer,
StopServer
SEnumEnd(CenterOrder)

SEnumBegin(ServerBootState)
Initialized,
Stopping,
Stopped,
Launching,
Launched
SEnumEnd(ServerBootState)

