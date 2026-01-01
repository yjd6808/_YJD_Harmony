/*
 * 작성자 : 윤정도
 * 테스트로 쓸 커맨드 목록
 */

#pragma once

#include <jnet/Packet/Command.h>
#include <jc/Primitives/StaticString.h>

#define CMD_SATIC_MESSAGE						100
#define CMD_DYNAMIC_MESSAGE						101

#pragma pack(push, CMD_ALIGNMENT)

STATIC_CMD_BEGIN(StaticMessage, CMD_SATIC_MESSAGE)
jc::StaticString<512> msg_;
STATIC_CMD_END

struct Dummy
{
	short a_ = 1;
	int b_ = 2;
	short c_ = 3;
};

DYNAMIC_CMD_BEGIN(DynamicMessage, CMD_DYNAMIC_MESSAGE, char)
Dummy dmg_;
long long d_ = 4;
DYNAMIC_CMD_ADD_ZERO_SIZE_ARRAY_FIELD(Msg)
DYNAMIC_CMD_END

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)