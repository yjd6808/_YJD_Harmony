/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:51:48 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>



 // 전체 서버 종류
JCORE_SENUM_BEGIN(ServerType)
Auth,
Begin = Auth,
Lobby,
Logic,
Chat,
Area,
Center,
End = Center,
Max
JCORE_SENUM_MIDDLE(ServerType)

static constexpr const char* Name[Max]{
	"인증",
	"로비",
	"로직",
	"채팅",
	"지역",
	"중앙"
};

JCORE_SENUM_MIDDLE_END(ServerType)

// ==========================================================================================


JCORE_SENUM_BEGIN(SingleServerType)
Center,
Begin = Center,
Auth,
Lobby,
End = Lobby,
Max
JCORE_SENUM_END(SingleServerType)

// ==========================================================================================

JCORE_SENUM_BEGIN(GameServerType)
Begin,
Luke = Begin,
Hilder,
Siroco,
Prey,
Kasias,		// 대충 발음나는데..
Diregie,
Cain,
Seria,
End = Seria,
Max
JCORE_SENUM_MIDDLE(GameServerType)
static constexpr const char* Name[Max]{
	"루크",
	"힐더",
	"시로코",
	"프레이",
	"카시아스",
	"디레지에",
	"카인",
	"세리아"
};
JCORE_SENUM_MIDDLE_END(GameServerType)

// ==========================================================================================

JCORE_SENUM_BEGIN(ServerProcessType)
None,
Auth,
Begin = Auth,
Center,
Lobby,
Game,
End = Game
JCORE_SENUM_MIDDLE(ServerProcessType)
static constexpr int Count = End;
static constexpr const char* Name[Count + 1]{
	"없음",
	"인증",
	"중앙",
	"로비",
	"게임",
};

JCORE_SENUM_MIDDLE_END(ServerProcessType)

// ==========================================================================================

// 클라이언트가 게임을 플레이하는 동안 한번이라도 접속하게되는 서버타입
JCORE_SENUM_BEGIN(ClientConnectServerType)
Auth,
Begin = Auth,
Lobby,
Logic,
Chat,
Area,
End = Area,
Max
JCORE_SENUM_END(ClientConnectServerType)


// ==========================================================================================
