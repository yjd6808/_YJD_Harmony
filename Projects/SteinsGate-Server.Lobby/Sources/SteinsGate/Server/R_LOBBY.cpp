/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */



#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "R_LOBBY.h"

#include <SteinsGate/Common/Cmd_LOBBY.h>

#include <SteinsGate/Server/Q_LOBBY.h>
#include <SteinsGate/Server/S_AUTHENTICATION_IS.h>
#include <SteinsGate/Server/LobbySession.h>

#include <SteinsGate/Common/UnauthenticatedSessionManager.h>
#include <SteinsGate/Common/AuthenticationComponent.h>

USING_NS_JC;
USING_NS_JNET;

void R_LOBBY::RECV_CLO_JoinLobby(Session* session, ICommand* cmd) {
	CLO_JoinLobby* pCmd = (CLO_JoinLobby*)cmd;
	LobbySession* pSession = (LobbySession*)session;

	if (!Const::Authentication::SerialRange.Contain(pCmd->Serial)) {
		_LogWarn_("세션의 시리얼 정보가 올바르지 않습니다. (시리얼: %d)", pCmd->Serial);
		session->Disconnect();
		return;
	}

	if (!Core::Contents.UnauthenticatedSessionManager->Add(pCmd->Serial, pSession)) {
		_LogWarn_("세션이 이미 포함되어있습니다.");
		session->Disconnect();
		return;
	}

	_LogInfo_("%s 로비 접속 시도중", pCmd->AccountId.Source);
	AuthenticationComponent* pAuthenticationComponent = pSession->GetAuthenticationComponent();
	pAuthenticationComponent->SetSerial(pCmd->Serial);
	pAuthenticationComponent->SetState(AuthenticationState::LobbyWait);
	pAuthenticationComponent->SetAccountId(pCmd->AccountId.Source);

	S_AUTHENTICATION_IS::SetInformation(Core::InterServerClientTcp, SendStrategy::SendAsync, SingleServerType::Auth);
	S_AUTHENTICATION_IS::SEND_SAU_AuthenticationCheck(pSession->GetHandle(), pCmd->AccountId.Source, pCmd->Serial);

}