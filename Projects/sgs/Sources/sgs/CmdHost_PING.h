/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 9:17:46 PM
 * =====================
 *
 */


#pragma once

#include "jc/Primitives/StaticString.h"
#include "sgs/ServerEnum.h"
#include "sgs/InterServerCmdDefine.h"

// ======================= CMD LIST =======================
// SCE_TimeSync				: 시간 정보 요청
// CES_TimeSyncAck			: 시간 정보 요청 응답
// ======================= CMD LIST =======================

#define CMDID_SCE_TimeSync					50100
#define CMDID_CES_TimeSyncAck				50101

#pragma pack(push, CMD_ALIGNMENT)

HOST_STATIC_CMD_BEGIN(SCE_TimeSync, CMDID_SCE_TimeSync)
	jc::DateTime PeerServerTime;
HOST_STATIC_CMD_END

// ===============================================================================================================

HOST_STATIC_CMD_BEGIN(CES_TimeSyncAck, CMDID_CES_TimeSyncAck)
	jc::DateTime PeerServerTime;
	jc::DateTime MasterServerTime;
HOST_STATIC_CMD_END

// ===============================================================================================================

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)
