/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:59:29 PM
 * =====================
 *
 */


#pragma once

#include "jc/Primitives/StaticString.h"
#include "jnet/Packet/Command.h"

// ======================= CMD LIST =======================
// CAU_Login
// AUC_LoginAck
// ======================= CMD LIST =======================
// COUNT = 2

#define CMDID_CAU_Login			1000
#define CMDID_AUC_LoginAck		1001

#pragma pack(push, CMD_ALIGNMENT)

STATIC_CMD_BEGIN(CAU_Login, CMDID_CAU_Login)
jc::StaticString<Const::StringLen::AccountId> id_;
jc::StaticString<Const::StringLen::AccountPass> pass_;
STATIC_CMD_END


STATIC_CMD_BEGIN(AUC_LoginAck, CMDID_AUC_LoginAck)
LoginResult_t result_;
AuthenticationSerial_t serial_;
GameServerType_t lastServer_;
jc::DateTime banBeginDate_;
jc::DateTime banEndDate_;
STATIC_CMD_END


#pragma pack(pop)