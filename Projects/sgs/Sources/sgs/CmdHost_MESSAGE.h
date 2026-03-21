/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 9:20:05 PM
 * =====================
 *
 */


#pragma once

#include "jc/Primitives/StaticString.h"
#include "sgs/ServerEnum.h"
#include "sgs/InterServerCmdDefine.h"

// ======================= CMD LIST =======================
// SS_HostMessage			: 서버/클라간 주고받는 문자열 메시지
// ======================= CMD LIST =======================

#define CMDID_SS_HostMessage				50050

#pragma pack(push, CMD_ALIGNMENT)

HOST_DYNAMIC_CMD_BEGIN(SS_HostMessage, CMDID_SS_HostMessage, jc::StaticString<1>)
	HOST_DYNAMIC_CMD_ADD_ARRAY_FIELD(Msg)
HOST_DYNAMIC_CMD_END

// ===============================================================================================================

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)
