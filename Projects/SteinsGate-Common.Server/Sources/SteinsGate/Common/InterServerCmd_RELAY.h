/*
 * 작성자: 윤정도
 * 생성일: 3/31/2023 10:19:48 PM
 * =====================
 * 개별 서버간 통신 커맨드
 *
 * TCP는 중앙서버를 경유해서 통신
 * UDP는 직접적으로 보낸다.
 *
 */


#pragma once

#include <JCore/Primitives/StaticString.h>
#include <SteinsGate/Common/ServerEnum.h>
#include <SteinsGate/Common/InterServerCmdDefine.h>


// ======================= CMD LIST =======================
// SS_P2PRelayStaticTest				: Static 커맨드를 릴레이 P2P 송신 테스트
// SS_P2PRelayDynamicTest				: Dynamic 커맨드를 릴레이 P2P 송신 테스트
// ======================= CMD LIST =======================

#define CMDID_SS_P2PRelayStaticTest					60000
#define CMDID_SS_P2PRelayDynamicTest				60001

#pragma pack(push, CMD_ALIGNMENT)

RELAY_STATIC_CMD_BEGIN(SS_P2PRelayStaticTest, CMDID_SS_P2PRelayStaticTest)
JCore::StaticString<64> Msg;
RELAY_STATIC_CMD_END(SS_P2PRelayStaticTest)

RELAY_DYNAMIC_CMD_BEGIN(SS_P2PRelayDynamicTest, CMDID_SS_P2PRelayDynamicTest, JCore::StaticString<1>)
JCore::StaticString<1> Msg;
RELAY_DYNAMIC_CMD_END(SS_P2PRelayDynamicTest)

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)