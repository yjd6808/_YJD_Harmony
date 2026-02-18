/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 4:01:24 PM
 * =====================
 *
 */

#include "Core.h"
#include "S_SETUP_IS_COMMON.h"

#include <sgs/CmdHost_SETUP.h>

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
bool S_SETUP_IS_COMMON::SEND_SCE_NotifyBootState(int _serverId, ServerType_t _serverType, ServerBootState_t _state)
{
	auto sending = SendBegin<SCE_NotifyBootState>();
	sending.cmd_.State = _state;
	sending.cmd_.ServerType = _serverType;
	sending.cmd_.ServerId = _serverId;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool S_SETUP_IS_COMMON::SEND_SCE_NotifyOrderFailed(int _serverId, ServerType_t _serverType, CenterOrder_t _failedOrder, Int32U _errorCode)
{
	auto sending = SendBegin<SCE_NotifyOrderFailed>();
	sending.cmd_.ErrorCode = _errorCode;
	sending.cmd_.ServerType = _serverType;
	sending.cmd_.ServerId = _serverId;
	sending.cmd_.Order = _failedOrder;
	return true;
}
