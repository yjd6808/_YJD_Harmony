/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:39:05 PM
 * =====================
 *
 */

#pragma once

#include <jc/Primitives/StaticString.h>
#include <jnet/Packet/Command.h>
#include <sg/Struct.h>

// ======================= CMD LIST =======================
// CLO_JoinLobby
// LOC_JoinLobbyAck
// ======================= CMD LIST =======================
// COUNT = 2

#define CMDID_CLO_JoinLobby				2000
#define CMDID_LOC_JoinLobbyAck			2001
#define CMDID_CLO_LoadChannelInfo		2002

#pragma pack(push, CMD_ALIGNMENT)

STATIC_CMD_BEGIN(CLO_JoinLobby, CMDID_CLO_JoinLobby)
jc::StaticString<Const::StringLen::AccountId> AccountId;
AuthenticationSerial_t Serial;
STATIC_CMD_END


STATIC_CMD_BEGIN(LOC_JoinLobbyAck, CMDID_LOC_JoinLobbyAck)
GameServerType_t DefaultSelectedServer;
STATIC_CMD_END

STATIC_CMD_BEGIN(CLO_LoadChannelInfo, CMDID_CLO_LoadChannelInfo)
GameServerType_t Server;
STATIC_CMD_END

#pragma pack(pop)