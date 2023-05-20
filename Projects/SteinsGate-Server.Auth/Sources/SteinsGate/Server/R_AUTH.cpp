/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:32:23 PM
 * =====================
 *
 */



#include "Auth.h"
#include "AuthCoreHeader.h"
#include "R_AUTH.h"

#include <SteinsGate/Common/AuthCmd.h>
#include <SteinsGate/Server/S_AUTH.h>

USING_NS_JC;
USING_NS_JNET;

void R_AUTH::RecvLogin(Session* session, ICommand* cmd) {
	CmdLogin* pCmd = (CmdLogin*)cmd;

	AccountData accountData;

	S_AUTH::AuthFlush _;
	S_AUTH::SetInformation(session, eSendAlloc);
	S_AUTH::SendLoginAck(true, 100);
}
