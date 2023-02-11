/*
 * 작성자 : 윤정도
 * 테스트로 쓸 커맨드 목록
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