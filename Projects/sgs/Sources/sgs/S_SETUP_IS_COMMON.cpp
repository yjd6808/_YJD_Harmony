/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 4:01:24 PM
 * =====================
 *
 */

#include "S_SETUP_IS_COMMON.h"

#include "sgs/CmdHost_SETUP.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
bool S_SETUP_IS_COMMON::SEND_SCE_ItsMe(ServerProcessType_t _clientType)
{
	auto sending = SendBegin<SCE_ItsMe>();
	sending.cmd_.ProcessType = _clientType;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool S_SETUP_IS_COMMON::SEND_SCE_NotifyBootState(object_id _serverHandle, ServerType_t _serverType, ServerBootState_t _state)
{
	auto sending = SendBegin<SCE_NotifyBootState>();
	sending.cmd_.State = _state;
	sending.cmd_.ServerType = _serverType;
	sending.cmd_.ServerHandle = _serverHandle;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool S_SETUP_IS_COMMON::SEND_SCE_NotifyOrderFailed(object_id _serverHandle, ServerType_t _serverType, CenterOrder_t _failedOrder, _u32 _errorCode)
{
	auto sending = SendBegin<SCE_NotifyOrderFailed>();
	sending.cmd_.ErrorCode = _errorCode;
	sending.cmd_.ServerType = _serverType;
	sending.cmd_.ServerHandle = _serverHandle;
	sending.cmd_.Order = _failedOrder;
	return true;
}
