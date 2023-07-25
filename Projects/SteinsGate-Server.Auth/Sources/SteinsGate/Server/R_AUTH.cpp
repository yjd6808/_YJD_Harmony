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
#include <SteinsGate/Server/Q_LOGIN.h>
#include <SteinsGate/Server/S_AUTH.h>

USING_NS_JC;
USING_NS_JNET;

void R_AUTH::RecvLogin(Session* session, ICommand* cmd) {
	CmdLogin* pCmd = (CmdLogin*)cmd;
	S_AUTH::AutoFlush _;
	S_AUTH::SetInformation(session, eSendAlloc);
	AccountData accountData;

	if (!Q_LOGIN::SelectAccountInfo(pCmd->Id.Source, pCmd->Pass.Source, accountData))  {
		const bool bRegistered = Q_LOGIN::RegisterAccount(pCmd->Id.Source, pCmd->Pass.Source);
		if (bRegistered) { S_AUTH::SendLoginAck(LoginResult::RegisterSuccess); }
		else { S_AUTH::SendLoginAck(LoginResult::IdAlreadyExist); }
		return;
	}

	if (!CoreTokenManager_v->Issue(accountData.DBTableId, accountData.Id.Source)) {
		S_AUTH::SendLoginAck(LoginResult::Logined);
		return;
	}

	S_AUTH::SendLoginAck(LoginResult::LoginSuccess);
}
