/*
 * 작성자: 윤정도
 * 생성일: 3/31/2023 10:19:48 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/Command.h>
#include <JCore/Primitives/StaticString.h>

#define Cmd_SaticMessage						100
#define Cmd_DynamicMessage						101

#pragma pack(push, 1)

StaticCmdBegin(StaticMessage, Cmd_SaticMessage)
JCore::StaticString<512> Msg;
StaticCmdEnd(StaticMessage)


DynamicCmdBegin(DynamicMessage, Cmd_DynamicMessage, char, Msg)
DynamicCmdEnd(DynamicMessage)

#pragma pack(pop)