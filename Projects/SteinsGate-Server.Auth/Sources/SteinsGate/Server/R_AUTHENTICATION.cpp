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
#include <SteinsGate/Common/CmdRelay_AUTHENTICATION.h>
#include <SteinsGate/Common/S_MESSAGE_COMMON.h>

#include <SteinsGate/Server/Q_LOGIN.h>
#include <SteinsGate/Server/S_AUTHENTICATION.h>
#include <SteinsGate/Server/S_AUTHENTICATION_IS.h>

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
	ServerProcessType_t eReplyServer = ServerProcessType::None;
	bool bAuthenticated = false;

	if (pCmd->RequestedServer == ServerProcessType::Lobby) {
		bAuthenticated = Core::Contents.AuthenticationManager->Update(pCmd->Serial, pCmd->AccountId.Source, AuthenticationState::Lobby);
		eReplyServer = ServerProcessType::Lobby;
	} else if (pCmd->RequestedServer == ServerProcessType::Game) {
		bAuthenticated = Core::Contents.AuthenticationManager->Update(pCmd->Serial, pCmd->AccountId.Source, AuthenticationState::Game);
		eReplyServer = ServerProcessType::Game;
	} else {
		_LogWarn_("알 수 없는 프로세스로부터 수신");
		return;
	}

	S_AUTHENTICATION_IS::SetInformation(Core::InterServerClientTcp, SendStrategy::SendAsync, eReplyServer);
	S_AUTHENTICATION_IS::SEND_AUS_AuthenticationCheckAck(pCmd->SessionHandle, bAuthenticated);
}
