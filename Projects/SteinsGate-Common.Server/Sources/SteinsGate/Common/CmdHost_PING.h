/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 9:17:46 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Primitives/StaticString.h>
#include <SteinsGate/Common/ServerEnum.h>
#include <SteinsGate/Common/InterServerCmdDefine.h>

// ======================= CMD LIST =======================
// SCE_TimeSync				: 시간 정보 요청
// CES_TimeSyncAck			: 시간 정보 요청 응답
// ======================= CMD LIST =======================

#define CMDID_SCE_TimeSync					50100
#define CMDID_CES_TimeSyncAck				50101

#pragma pack(push, CMD_ALIGNMENT)

HOST_STATIC_CMD_BEGIN(SCE_TimeSync, CMDID_SCE_TimeSync)
JCore::DateTime PeerServerTime;
HOST_STATIC_CMD_END(SCE_TimeSync)

// ===============================================================================================================

HOST_STATIC_CMD_BEGIN(CES_TimeSyncAck, CMDID_CES_TimeSyncAck)
JCore::DateTime PeerServerTime;
JCore::DateTime MasterServerTime;
HOST_STATIC_CMD_END(CES_TimeSyncAck)

// ===============================================================================================================

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)