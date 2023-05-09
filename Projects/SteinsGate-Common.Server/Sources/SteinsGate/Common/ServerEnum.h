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

SEnumBegin(CenterClientType)
None,
Auth,
Begin = Auth,
Lobby,
Game,
End = Game,
Max
SEnumMiddle(CenterClientType)

static constexpr const char* Name[Max]{
	"",
	"인증",
	"로비",
	"게임",
};

SEnumMiddleEnd(CenterClientType)

SEnumBegin(CommonInputEvent)
NoEvent,
TerminateProgram,
Help,
CommonMax
SEnumMiddle(CommonInputEvent)
static const char* CommonName[CommonMax];
SEnumMiddleEnd(CommonInputEvent)

// =================================================

SEnumInteritedBegin(AuthInputEvent, CommonInputEvent)
AuthMax = 2
SEnumMiddle(AuthInputEvent)
static const char* AuthName[AuthMax];
static const char* NameOf(int inputEvent);
static JCore::Pair<JCore::String, int> PairOf(int inputEvent);
SEnumMiddleEnd(AuthInputEvent)


SEnumInteritedBegin(CenterInputEvent, CommonInputEvent)
ShutdownAllServer,
ShutdownAuthServer,
ShutdownLobbyServer,
ShutdownGameServer,
ServerState,
CenterMax
SEnumMiddle(CenterInputEvent)
static const char* CenterName[CenterMax];
static const char* NameOf(int inputEvent);
static JCore::Pair<JCore::String, int> PairOf(int inputEvent);
SEnumMiddleEnd(CenterInputEvent)


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

