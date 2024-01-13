/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 4:01:24 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "S_MESSAGE_IS_COMMON.h"

#include <SteinsGate/Common/CmdHost_MESSAGE.h>

USING_NS_JC;
USING_NS_JNET;

bool S_MESSAGE_IS_COMMON::SEND_SS_HostMessage(const String& msg) {
	auto sending = SendBegin<SS_HostMessage>(msg.LengthWithNull());
	sending.Cmd.Msg()->SetStringUnsafe(msg);
	return true;
}
