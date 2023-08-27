/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 5:17:29 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "S_MESSAGE_COMMON.h"

#include <SteinsGate/Common/Cmd_MESSAGE.h>


void S_MESSAGE_COMMON::SEND_SC_ClientText(const char* clientTextKey) {
	auto sending = SendBegin<SC_ClientText>();
	sending.Cmd.Key.SetString(clientTextKey);
}
