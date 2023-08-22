/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 4:01:24 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "S_INTERSERVER_COMMON.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>
#include <SteinsGate/Common/InterServerCmd_RELAY.h>


USING_NS_JC;
USING_NS_JNET;

bool S_INTERSERVER_COMMON::SEND_SCE_ItsMe(InterServerClientType_t clientType, int serverId) {
	auto sending = SendBegin<SCE_ItsMe>();
	sending.Cmd.ClientType = clientType;
	sending.Cmd.ServerId = serverId;
	return true;
}

bool S_INTERSERVER_COMMON::SEND_SS_HostMessage(const String& msg) {
	auto sending = SendBegin<SS_HostMessage>(msg.LengthWithNull());
	sending.Cmd.Msg.SetStringUnsafe(msg);
	return true;
}

bool S_INTERSERVER_COMMON::SEND_SCE_NotifyBootState(int serverId, ServerType_t serverType, ServerBootState_t state) {
	auto sending = SendBegin<SCE_NotifyBootState>();
	sending.Cmd.State = state;
	sending.Cmd.ServerType = serverType;
	sending.Cmd.ServerId = serverId;
	return true;
}

bool S_INTERSERVER_COMMON::SEND_SCE_NotifyOrderFailed(int serverId, ServerType_t serverType, CenterOrder_t failedOrder, Int32U errorCode) {
	auto sending = SendBegin<SCE_NotifyOrderFailed>();
	sending.Cmd.ErrorCode = errorCode;
	sending.Cmd.ServerType = serverType;
	sending.Cmd.ServerId = serverId;
	sending.Cmd.Order = failedOrder;
	return true;
}

bool S_INTERSERVER_COMMON::SEND_SCE_TimeSync() {
	auto sending = SendBegin<SCE_TimeSync>();
	return true;
}


// ===============================================================================
// 
// ===============================================================================
bool S_INTERSERVER_COMMON::SEND_SS_P2PRelayStaticTest(const char* msg) {
	auto sending = SendBegin<SS_P2PRelayStaticTest>();
	sending.Cmd.Msg = msg;
	return true;
}

bool S_INTERSERVER_COMMON::SEND_SS_P2PRelayDynamicTest(const char* msg) {
	auto sending = SendBegin<SS_P2PRelayDynamicTest>(64);
	sending.Cmd.Msg.SetStringUnsafe(msg);
	return true;
}
