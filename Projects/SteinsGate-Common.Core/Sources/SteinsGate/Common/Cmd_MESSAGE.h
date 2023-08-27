/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 5:07:00 PM
 * =====================
 *
 */


#pragma once


#include <JCore/Primitives/StaticString.h>
#include <JNetwork/Packet/Command.h>
#include <SteinsGate/Common/Struct.h>

// ======================= CMD LIST =======================
// SC_ClientText	: 클라이언트 텍스트 키값을 서버측에서 전달
// ======================= CMD LIST =======================
// COUNT = 1

#define CMDID_SC_ClientText			3000

#pragma pack(push, CMD_ALIGNMENT)

STATIC_CMD_BEGIN(SC_ClientText, CMDID_SC_ClientText)
SGStaticString<Const::StringLen::ClientTextKey> Key;
STATIC_CMD_END(SC_ClientText)

#pragma pack(pop)

