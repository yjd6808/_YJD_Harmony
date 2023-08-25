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
SGStaticString<Const::StringLen::AccountId> Id;
SGStaticString<Const::StringLen::AccountPass> Pass;
STATIC_CMD_END(CAU_Login)


STATIC_CMD_BEGIN(AUC_LoginAck, CMDID_AUC_LoginAck)
LoginResult_t Result;
AuthenticationSerial_t Serial;
GameServerType_t LastServer;
JCore::DateTime BanBeginDate;
JCore::DateTime BanEndDate;
STATIC_CMD_END(AUC_LoginAck)


#pragma pack(pop)