/*
 * 작성자 : 윤정도
 * 테스트로 쓸 커맨드 목록
 */

#pragma once

#include <JNetwork/Packet/Command.h>
#include <JCore/Primitives/StaticString.h>

#define Cmd_SaticMessage						100
#define Cmd_DynamicMessage						101

#pragma pack(push, CMD_ALIGNMENT)

StaticCmdBegin(StaticMessage, Cmd_SaticMessage)
JCore::StaticString<512> Msg;
StaticCmdEnd(StaticMessage)


struct Dummy
{
	short a = 1;
	int b = 2;
	short c = 3;
};

DynamicCmdBegin(DynamicMessage, Cmd_DynamicMessage, char)
Dummy dmg;
long long d = 4;
char Msg[1];
DynamicCmdEnd(DynamicMessage)

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)