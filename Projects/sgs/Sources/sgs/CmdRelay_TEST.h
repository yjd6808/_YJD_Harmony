/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 11:33:01 PM
 * =====================
 *
 */


#pragma once

#include "jc/Primitives/StaticString.h"
#include "sgs/InterServerCmdDefine.h"

// ======================= CMD LIST =======================
// SS_P2PRelayStaticTest				: Static 커맨드를 릴레이 P2P 송신 테스트
// SS_P2PRelayDynamicTest				: Dynamic 커맨드를 릴레이 P2P 송신 테스트
// ======================= CMD LIST =======================

#define CMDID_SS_P2PRelayStaticTest						65000
#define CMDID_SS_P2PRelayDynamicTest					65001

#pragma pack(push, CMD_ALIGNMENT)

RELAY_STATIC_CMD_BEGIN(SS_P2PRelayStaticTest, CMDID_SS_P2PRelayStaticTest)
	jc::StaticString<64> Msg;
RELAY_STATIC_CMD_END

RELAY_DYNAMIC_CMD_BEGIN(SS_P2PRelayDynamicTest, CMDID_SS_P2PRelayDynamicTest, jc::StaticString<1>)
	RELAY_DYNAMIC_CMD_ADD_ARRAY_FIELD(Msg)
RELAY_DYNAMIC_CMD_END

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)
