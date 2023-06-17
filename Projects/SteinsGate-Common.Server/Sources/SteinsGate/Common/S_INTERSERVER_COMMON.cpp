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

USING_NS_JC;
USING_NS_JNET;

bool S_INTERSERVER_COMMON::SendItsMe(InterServerClientType_t clientType, int serverId) {
	auto sending = SendBegin<CmdItsMe>();
	sending.Cmd.ClientType = clientType;
	sending.Cmd.ServerId = serverId;
	return true;
}

bool S_INTERSERVER_COMMON::SendCenterMessage(const String& msg) {
	auto sending = SendBegin<CmdInterServerMessage>(msg.LengthWithNull());
	sending.Cmd.Msg.SetStringUnsafe(msg);
	return true;
}

bool S_INTERSERVER_COMMON::SendNotifyBootState(ServerBootState_t state) {
	auto sending = SendBegin<CmdNotifyBootState>();
	sending.Cmd.State = state;
	return true;
}
