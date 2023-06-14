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
// CmdLogin
// CmdLoginAck
// ======================= CMD LIST =======================

#pragma pack(push, CMD_ALIGNMENT)

StaticCmdBegin
	( CmdLogin
	, 100
	, eClientToServer
	)
// ===========================================
SGStaticString<AccountIdLen_v> Id;
SGStaticString<AccountPassLen_v> Pass;
// ===========================================
StaticCmdEnd(CmdLogin)


StaticCmdBegin
	( CmdLoginAck
	, 101
	, eServerToClient
	)
// ===========================================
LoginResult_t Result;
// ===========================================
StaticCmdEnd(CmdLoginAck)


#pragma pack(pop)