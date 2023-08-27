/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 9:19:33 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Primitives/StaticString.h>
#include <SteinsGate/Common/ServerEnum.h>
#include <SteinsGate/Common/InterServerCmdDefine.h>

// ======================= CMD LIST =======================
// CES_WhoAreYou			: 서버가 접속한 클라이언트가 누군지 물어봄
// SCE_ItsMe				: 클라가 누구인지 서버에게 알려줌
// CES_AlreadyConnected		: 서버가 접속한 클라이언트에게 넌 이미 접속중이라고 알려줌
// CES_YouNeedToDoThis		: 서버가 접속한 클라이언트에게 명령을 내림
// SCE_NotifyBootState		: 클라가 현재 자신의 부팅상태를 서버에게 알려줌
// SCE_NotifyOrderFailed	: 지시한 명령수행 실패시 서버에게 알려줌
// ======================= CMD LIST =======================

#define CMDID_CES_WhoAreYou					50000
#define CMDID_SCE_ItsMe						50001
#define CMDID_CES_AlreadyConnected			50002
#define CMDID_CES_YouNeedToDoThis			50003
#define CMDID_SCE_NotifyBootState			50004
#define CMDID_SCE_NotifyOrderFailed			50005

#pragma pack(push, CMD_ALIGNMENT)

HOST_STATIC_CMD_BEGIN(CES_WhoAreYou, CMDID_CES_WhoAreYou)
HOST_STATIC_CMD_END(CES_WhoAreYou)

// ===============================================================================================================

HOST_STATIC_CMD_BEGIN(SCE_ItsMe, CMDID_SCE_ItsMe)
ServerProcessType_t ClientServerType;
int ServerId;
HOST_STATIC_CMD_END(SCE_ItsMe)

// ===============================================================================================================

HOST_STATIC_CMD_BEGIN(CES_AlreadyConnected, CMDID_CES_AlreadyConnected)
HOST_STATIC_CMD_END(CES_AlreadyConnected)

// ===============================================================================================================

HOST_STATIC_CMD_BEGIN(CES_YouNeedToDoThis, CMDID_CES_YouNeedToDoThis)
CenterOrder_t Order;
HOST_STATIC_CMD_END(CES_YouNeedToDoThis)

// ===============================================================================================================

HOST_STATIC_CMD_BEGIN(SCE_NotifyBootState, CMDID_SCE_NotifyBootState)
int ServerId;
ServerType_t ServerType;
ServerBootState_t State;
HOST_STATIC_CMD_END(SCE_NotifyBootState)

// ===============================================================================================================

HOST_STATIC_CMD_BEGIN(SCE_NotifyOrderFailed, CMDID_SCE_NotifyOrderFailed)
int ServerId;
ServerType_t ServerType;
CenterOrder_t Order;
Int32U ErrorCode;
HOST_STATIC_CMD_END(SCE_NotifyOrderFailed)

// ===============================================================================================================

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)

