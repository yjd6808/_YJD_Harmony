/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:39:05 PM
 * =====================
 *
 */

#pragma once

#include <JCore/Primitives/StaticString.h>
#include <JNetwork/Packet/Command.h>
#include <SteinsGate/Common/Struct.h>

// ======================= CMD LIST =======================
// CLO_JoinLobby
// LOC_JoinLobbyAck
// ======================= CMD LIST =======================
// COUNT = 2

#define CMDID_CLO_JoinLobby		2000
#define CMDID_LOC_JoinLobbyAck	2001

#pragma pack(push, CMD_ALIGNMENT)

STATIC_CMD_BEGIN(CLO_JoinLobby, CMDID_CLO_JoinLobby)
SGStaticString<Const::StringLen::AccountId> AccountId;
AuthenticationSerial_t Serial;
STATIC_CMD_END(CLO_JoinLobby)


STATIC_CMD_BEGIN(LOC_JoinLobbyAck, CMDID_LOC_JoinLobbyAck)
STATIC_CMD_END(LOC_JoinLobbyAck)


#pragma pack(pop)