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
// CmdP2PRelayStaticTest				: Static 커맨드를 릴레이 P2P 송신 테스트
// CmdP2PRelayDynamicTest				: Dynamic 커맨드를 릴레이 P2P 송신 테스트
// ======================= CMD LIST =======================

#define CMDID_CmdP2PRelayStaticTest					60000
#define CMDID_CmdP2PRelayDynamicTest				60001

#pragma pack(push, CMD_ALIGNMENT)

RELAY_STATIC_CMD_BEGIN
	( CmdP2PRelayStaticTest
	, CMDID_CmdP2PRelayStaticTest
	, ePeerToPeer
	)
// ───────────────────────────────────────────
JCore::StaticString<64> Msg;
// ───────────────────────────────────────────
RELAY_STATIC_CMD_END(CmdTest)

RELAY_DYNAMIC_CMD_BEGIN
	( CmdP2PRelayDynamicTest
	, CMDID_CmdP2PRelayDynamicTest
	, ePeerToPeer
	, JCore::StaticString<1>
	)
// ───────────────────────────────────────────
JCore::StaticString<1> Msg;
// ───────────────────────────────────────────
RELAY_DYNAMIC_CMD_END(CmdTest)

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)