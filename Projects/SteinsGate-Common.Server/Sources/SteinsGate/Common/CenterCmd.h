/*
 * 작성자: 윤정도
 * 생성일: 3/31/2023 10:19:48 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/Command.h>
#include <JCore/Primitives/StaticString.h>
#include <SteinsGate/Common/CenterEnum.h>

#define CmdConnection_CS						100

#pragma pack(push, CMD_ALIGNMENT)

StaticCmdBegin(CmdConnection, CmdConnection_CS)
CenterClientType_t ClientType;
StaticCmdEnd(CmdConnection)


//DynamicCmdBegin(DynamicMessage, Cmd_DynamicMessage, char)
//char Msg[1];
//DynamicCmdEnd(DynamicMessage)

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)