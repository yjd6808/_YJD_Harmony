/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 9:28:56 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Primitives/StaticString.h>
#include <SteinsGate/Common/Struct.h>
#include <SteinsGate/Common/InterServerCmdDefine.h>

 // ======================= CMD LIST =======================
 // SAU_AuthenticationCheck		: 로비/게임이 인증시리얼 체크를 위해 인증서버에 물어봄
 // SAU_AuthenticationCheckAck	: 인증결과 알려줌
 // ======================= CMD LIST =======================

#define CMDID_SAU_AuthenticationCheck					55000
#define CMDID_SAU_AuthenticationCheckAck				55001

#pragma pack(push, CMD_ALIGNMENT)

RELAY_STATIC_CMD_BEGIN(SAU_AuthenticationCheck, CMDID_SAU_AuthenticationCheck)
int SessionHandle;
SGStaticString<Const::StringLen::AccountId> AccountId;
ServerProcessType_t RequestedServer;	// 요청한 서버
AuthenticationSerial_t Serial;			// 체크해야하는 시리얼
RELAY_STATIC_CMD_END(SAU_AuthenticationCheck)

RELAY_STATIC_CMD_BEGIN(SAU_AuthenticationCheckAck, CMDID_SAU_AuthenticationCheckAck)
int SessionHandle;
bool Success;			// 실패시 만료 혹은 이상한유저
RELAY_STATIC_CMD_END(SAU_AuthenticationCheckAck)


#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)