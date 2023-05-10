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

#define CmdLogin_CS						100
#define CmdLoginAck_SC					101

#pragma pack(push, CMD_ALIGNMENT)

StaticCmdBegin(CmdLogin, CmdLogin_CS)
SGStaticString<AccountIdLen_v> Id;
SGStaticString<AccountPassLen_v> Pw;
StaticCmdEnd(CmdLogin)

StaticCmdBegin(CmdLoginAck, CmdLoginAck_SC)
bool Success;
int Result;
StaticCmdEnd(CmdLoginAck)


#pragma pack(pop)