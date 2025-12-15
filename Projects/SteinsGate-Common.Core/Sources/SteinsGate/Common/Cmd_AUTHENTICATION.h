/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:59:29 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Primitives/StaticString.h>
#include <JNetwork/Packet/Command.h>
#include <SteinsGate/Common/Struct.h>

// ======================= CMD LIST =======================
// CAU_Login
// AUC_LoginAck
// ======================= CMD LIST =======================
// COUNT = 2

#define CMDID_CAU_Login			1000
#define CMDID_AUC_LoginAck		1001

#pragma pack(push, CMD_ALIGNMENT)

STATIC_CMD_BEGIN(CAU_Login, CMDID_CAU_Login)
SGStaticString<Const::StringLen::AccountId> id_;
SGStaticString<Const::StringLen::AccountPass> pass_;
STATIC_CMD_END


STATIC_CMD_BEGIN(AUC_LoginAck, CMDID_AUC_LoginAck)
LoginResult_t result_;
AuthenticationSerial_t serial_;
GameServerType_t lastServer_;
JCore::DateTime banBeginDate_;
JCore::DateTime banEndDate_;
STATIC_CMD_END


#pragma pack(pop)