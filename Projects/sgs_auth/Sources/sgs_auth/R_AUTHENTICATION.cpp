/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 3:32:23 PM
 * =====================
 *
 */



#include "R_AUTHENTICATION.h"

#include "sg/Cmd_AUTHENTICATION.h"
#include "sgs/CmdRelay_AUTHENTICATION.h"
#include "sgs/S_MESSAGE_COMMON.h"

#include "sgs_auth/Q_LOGIN.h"
#include "sgs_auth/S_AUTHENTICATION.h"
#include "sgs_auth/S_AUTHENTICATION_IS.h"

#include "AuthenticationManager.h"
#include "sgs/Net/NetGroup_InterServ.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_AUTHENTICATION::RECV_CAU_Login(Session* _session, ICommand* _cmd)
{
	CAU_Login* pCmd = (CAU_Login*)_cmd;

	S_AUTHENTICATION::AutoFlush _;
	S_AUTHENTICATION::SetInformation(_session, SendStrategy::SendAlloc);
	AccountData accountData;

	LoginResult_t eResult = LoginResult::LoginSuccess;

	bool bHasAccount = Q_LOGIN::SelectAccountInfo(pCmd->id_.Source, accountData);
	bool bRegistered = false;

	if (!Q_LOGIN::IsSuccess) {
		S_AUTHENTICATION::SEND_AUC_LoginAck(LoginResult::QueryFailed);
		return;
	}

	AuthenticationData* pAuthenticationData = nullptr;

	// 계정이 없는 경우, 회원가입시도
	if (!bHasAccount)  {
		bRegistered = Q_LOGIN::RegisterAccount(pCmd->id_.Source, pCmd->pass_.Source);

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
	if (accountData.pass_ != pCmd->pass_) {
		S_AUTHENTICATION::SEND_AUC_LoginAck(LoginResult::IdPasswordMismatch);
		return;
	}

	pAuthenticationData = g_cAuthMgr.Issue(accountData);
	if (pAuthenticationData == nullptr) {
		S_AUTHENTICATION::SEND_AUC_LoginAck(LoginResult::Logined);
		return;
	}

	S_AUTHENTICATION::SEND_AUC_LoginAck(eResult, accountData.lastServer_, pAuthenticationData->serial_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void R_AUTHENTICATION::RECV_SAU_AuthenticationCheck(Session* _session, ICommand* _cmd)
{
	SAU_AuthenticationCheck* pCmd = (SAU_AuthenticationCheck*)_cmd;
	ServerProcessType_t eReplyServer = ServerProcessType::None;
	AuthenticationData* pAuthenticationData = nullptr;

	if (pCmd->RequestedServer == ServerProcessType::Lobby) {
		pAuthenticationData = g_cAuthMgr.Update(pCmd->Serial, pCmd->AccountId.Source, AuthenticationState::Lobby);
		eReplyServer = ServerProcessType::Lobby;
	} else if (pCmd->RequestedServer == ServerProcessType::Game) {
		pAuthenticationData = g_cAuthMgr.Update(pCmd->Serial, pCmd->AccountId.Source, AuthenticationState::Game);
		eReplyServer = ServerProcessType::Game;
	} else {
		_LogWarn_("알 수 없는 프로세스로부터 수신");
		return;
	}

	const GameServerType_t eLastServer = GameServerType::Hilder;		// 힐더서버를 디폴트로...
	const bool bAuthenticated = pAuthenticationData != nullptr;			// 업데이트가 성공적으로 끝나서 올바른 AuthenticationData를 반환한 경우

	S_AUTHENTICATION_IS::SetInformation(g_cNetGroup_InterServ.GetCenterTcp(), SendStrategy::SendAsync, eReplyServer);
	S_AUTHENTICATION_IS::SEND_AUS_AuthenticationCheckAck(bAuthenticated, pCmd->SessionHandle, pAuthenticationData->accountData_.lastServer_);
}
