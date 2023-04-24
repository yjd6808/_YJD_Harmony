/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 4:01:24 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "S_CENTER_COMMON.h"

#include <SteinsGate/Common/CenterCmd.h>

USING_NS_JC;
USING_NS_JNET;

bool S_CENTER_COMMON::SendItsMe(CenterClientType_t clientType) {
	auto sending = SendBegin<CmdItsMe>();
	sending.Cmd.ClientType = clientType;
	return true;
}

bool S_CENTER_COMMON::SendCenterMessage(const String& msg) {
	auto sending = SendBegin<CmdCenterMessage>(msg.LengthWithNull());
	sending.Cmd.Msg.SetStringUnsafe(msg);
	return true;
}

bool S_CENTER_COMMON::SendNotifyBootState(ServerBootState_t state) {
	auto sending = SendBegin<CmdNotifyBootState>();
	sending.Cmd.State = state;
	return true;
}
