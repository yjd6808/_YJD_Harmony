/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:23:52 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "S_AUTH.h"

#include <SteinsGate/Common/AuthCmd.h>

USING_NS_JC;
USING_NS_JNET;

void S_AUTH::SendLoginAck(LoginResult_t result) {
	const auto sending = SendBegin<CmdLoginAck>();
	sending.Cmd.Result = result;
}