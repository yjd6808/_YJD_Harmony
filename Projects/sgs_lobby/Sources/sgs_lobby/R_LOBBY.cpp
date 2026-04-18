/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */



#include "R_LOBBY.h"

#include "sg/Cmd_LOBBY.h"

#include "sgs/AuthenticationComponent.h"
#include "sgs/Net/NetGroup_InterServ.h"

#include "sgs_lobby/Q_LOBBY.h"
#include "sgs_lobby/S_AUTHENTICATION_IS.h"
#include "sgs_lobby/UnauthenticatedSessionManager.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_LOBBY::RECV_CLO_JoinLobby(Session* _pSession, ICommand* _pCmd)
{
	CLO_JoinLobby* pCmd = (CLO_JoinLobby*)_pCmd;
	sg::NetSession* pSession = (sg::NetSession*)_pSession;

	if (!Const::Authentication::SerialRange.Contain(pCmd->Serial)) 
	{
		_LogWarn_("세션의 시리얼 정보가 올바르지 않습니다. (시리얼: %d)", pCmd->Serial);
		_pSession->Disconnect();
		return;
	}

	if (!g_cUnauthenticatedSessionManager.Add(pCmd->Serial, pSession))
	{
		_LogWarn_("세션이 이미 포함되어있습니다.");
		_pSession->Disconnect();
		return;
	}

	_LogInfo_("%s 로비 접속 시도중", pCmd->AccountId.Source);
	sg::AuthenticationComponent* pAuthenticationComponent = pSession->GetComponent<sg::AuthenticationComponent>();
	pAuthenticationComponent->SetSerial(pCmd->Serial);
	pAuthenticationComponent->SetState(AuthenticationState::LobbyWait);
	pAuthenticationComponent->SetAccountId(pCmd->AccountId.Source);

	S_AUTHENTICATION_IS::SetInformation(g_cNetGroup_InterServ.GetCenterTcp(), SendStrategy::SendAsync, SingleServerType::Auth);
	S_AUTHENTICATION_IS::SEND_SAU_AuthenticationCheck(pSession->GetHandle(), pCmd->AccountId.Source, pCmd->Serial);

}
