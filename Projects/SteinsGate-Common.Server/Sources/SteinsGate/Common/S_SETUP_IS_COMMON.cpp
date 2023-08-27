/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 4:01:24 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "S_SETUP_IS_COMMON.h"

#include <SteinsGate/Common/CmdHost_SETUP.h>

USING_NS_JC;
USING_NS_JNET;

bool S_SETUP_IS_COMMON::SEND_SCE_ItsMe(ServerProcessType_t clientType, int serverId) {
	auto sending = SendBegin<SCE_ItsMe>();
	sending.Cmd.ClientServerType = clientType;
	sending.Cmd.ServerId = serverId;
	return true;
}

bool S_SETUP_IS_COMMON::SEND_SCE_NotifyBootState(int serverId, ServerType_t serverType, ServerBootState_t state) {
	auto sending = SendBegin<SCE_NotifyBootState>();
	sending.Cmd.State = state;
	sending.Cmd.ServerType = serverType;
	sending.Cmd.ServerId = serverId;
	return true;
}

bool S_SETUP_IS_COMMON::SEND_SCE_NotifyOrderFailed(int serverId, ServerType_t serverType, CenterOrder_t failedOrder, Int32U errorCode) {
	auto sending = SendBegin<SCE_NotifyOrderFailed>();
	sending.Cmd.ErrorCode = errorCode;
	sending.Cmd.ServerType = serverType;
	sending.Cmd.ServerId = serverId;
	sending.Cmd.Order = failedOrder;
	return true;
}
