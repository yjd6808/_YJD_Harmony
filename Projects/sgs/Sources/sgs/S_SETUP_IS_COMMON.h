/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:46:12 PM
 * =====================
 *
 */

#pragma once

#include "sgs/InterServerSendHelper.h"
#include "sgs/ServerEnum.h"
#include "jnet/Packet/SendHelper.h"


struct S_SETUP_IS_COMMON : InterServerSendHelper<S_SETUP_IS_COMMON>
{
	// HOST
	static bool SEND_SCE_ItsMe(ServerProcessType_t _clientType);
	static bool SEND_SCE_NotifyBootState(object_id _serverHandle, ServerType_t _serverType, ServerBootState_t _state);
	static bool SEND_SCE_NotifyOrderFailed(object_id _serverHandle, ServerType_t _serverType, CenterOrder_t _failedOrder, _u32 _errorCode);

	// RELAY
};
