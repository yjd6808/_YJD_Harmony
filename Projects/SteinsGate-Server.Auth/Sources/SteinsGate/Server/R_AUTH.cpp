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
	CAU_Login* pCmd = (CAU_Login*)cmd;
	S_AUTH::AutoFlush _;
	S_AUTH::SetInformation(session, eSendAlloc);
	AccountData accountData;

	LoginResult_t eResult = LoginResult::LoginSuccess;

	bool bHasAccount = Q_LOGIN::SelectAccountInfo(pCmd->Id.Source, pCmd->Pass.Source, accountData);
	bool bRegistered = false;

	if (!Q_LOGIN::IsSuccess) {
		S_AUTH::SendLoginAck(LoginResult::QueryFailed);
		return;
	}

	// 계정이 없는 경우, 회원가입시도
	if (!bHasAccount)  {
		bRegistered = Q_LOGIN::RegisterAccount(pCmd->Id.Source, pCmd->Pass.Source);

		if (!Q_LOGIN::IsSuccess) {
			S_AUTH::SendLoginAck(LoginResult::QueryFailed);
			return;
		}

		if (bRegistered) 
			eResult = LoginResult::RegisterSuccess;
		else 
			eResult = LoginResult::IdAlreadyExist;
	} else {
		if (!CoreTokenManager_v->Issue(accountData.Id.Source)) {
			eResult = LoginResult::Logined;
		}
	}

	S_AUTH::SendLoginAck(eResult);
}
