/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:32:23 PM
 * =====================
 *
 */



#include "Auth.h"
#include "AuthCoreHeader.h"
#include "R_AUTHENTICATION.h"

#include <SteinsGate/Common/Cmd_AUTHENTICATION.h>
#include <SteinsGate/Server/Q_LOGIN.h>
#include <SteinsGate/Server/S_AUTHENTICATION.h>

#include <SteinsGate/Common/CmdRelay_AUTHENTICATION.h>

USING_NS_JC;
USING_NS_JNET;

void R_AUTHENTICATION::RECV_CAU_Login(Session* session, ICommand* cmd) {
	CAU_Login* pCmd = (CAU_Login*)cmd;
	S_AUTHENTICATION::AutoFlush _;
	S_AUTHENTICATION::SetInformation(session, SendStrategy::SendAlloc);
	AccountData accountData;

	LoginResult_t eResult = LoginResult::LoginSuccess;

	bool bHasAccount = Q_LOGIN::SelectAccountInfo(pCmd->Id.Source, accountData);
	bool bRegistered = false;

	if (!Q_LOGIN::IsSuccess) {
		S_AUTHENTICATION::SEND_AUC_LoginAck(LoginResult::QueryFailed);
		return;
	}

	AuthenticationData* pAuthenticationData = nullptr;

	// 계정이 없는 경우, 회원가입시도
	if (!bHasAccount)  {
		bRegistered = Q_LOGIN::RegisterAccount(pCmd->Id.Source, pCmd->Pass.Source);

		if (!Q_LOGIN::IsSuccess) {
			S_AUTHENTICATION::SEND_AUC_LoginAck(LoginResult::QueryFailed);
			return;
		}

		if (bRegistered) 
			eResult = LoginResult::RegisterSuccess;
		else 
			eResult = LoginResult::IdAlreadyExist;
		S_AUTHENTICATION::SEND_AUC_LoginAck(eResult);
		return;
	}

	// 계정이 있는 경우
	if (accountData.Pass != pCmd->Pass) {
		S_AUTHENTICATION::SEND_AUC_LoginAck(LoginResult::IdPasswordMismatch);
		return;
	}

	pAuthenticationData = Core::Contents.AuthenticationManager->Issue(accountData.Id.Source);
	if (pAuthenticationData == nullptr) {
		S_AUTHENTICATION::SEND_AUC_LoginAck(LoginResult::Logined);
		return;
	}

	S_AUTHENTICATION::SEND_AUC_LoginAck(eResult, accountData.LastServer, pAuthenticationData->Serial);
}


void R_AUTHENTICATION::RECV_SAU_AuthenticationCheck(Session* session, ICommand* cmd) {
	SAU_AuthenticationCheck* pCmd = (SAU_AuthenticationCheck*)cmd;

	if (pCmd->RequestedServer == ServerProcessType::Lobby) {
		Core::Contents.AuthenticationManager->Update(pCmd->Serial, pCmd->AccountId.Source, AuthenticationState::Lobby);
	}
}
