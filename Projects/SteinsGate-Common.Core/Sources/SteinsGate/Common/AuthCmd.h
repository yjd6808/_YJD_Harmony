/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:59:29 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/Packet/Command.h>

#include <JCore/Primitives/StaticString.h>

#include <SteinsGate/Common/Struct.h>
#include <SteinsGate/Common/AuthEnum.h>

// ======================= CMD LIST =======================
// CAU_Login
// AUC_LoginAck
// ======================= CMD LIST =======================

inline constexpr Int16U TotalAuthCmdCount_v = 2;

#pragma pack(push, CMD_ALIGNMENT)

STATIC_CMD_BEGIN
	( CAU_Login
	, 100
	, eClientToServer
	)
// ===========================================
SGStaticString<AccountIdLen_v> Id;
SGStaticString<AccountPassLen_v> Pass;
// ===========================================
STATIC_CMD_END(CAU_Login)


STATIC_CMD_BEGIN
	( AUC_LoginAck
	, 101
	, eServerToClient
	)
// ===========================================
LoginResult_t Result;
JCore::DateTime BanBeginDate;
JCore::DateTime BanEndDate;
// ===========================================
STATIC_CMD_END(AUC_LoginAck)


#pragma pack(pop)