/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */



#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "R_AUTHENTICATION.h"

#include <SteinsGate/Common/Cmd_LOBBY.h>

#include <SteinsGate/Server/Q_LOBBY.h>
#include <SteinsGate/Server/S_AUTHENTICATION_IS.h>
#include <SteinsGate/Server/LobbySession.h>

#include <SteinsGate/Common/UnauthenticatedSessionManager.h>
#include <SteinsGate/Common/AuthenticationComponent.h>

USING_NS_JC;
USING_NS_JNET;

void R_AUTHENTICATION::RECV_CLO_JoinLobby(Session* session, ICommand* cmd) {
	CLO_JoinLobby* pCmd = (CLO_JoinLobby*)cmd;
	LobbySession* pSession = (LobbySession*)session;

	if (Core::Contents.UnauthenticatedSessionManager->Add(pCmd->Serial, pSession)) {
		_LogNormal_("세션이 이미 포함되어있습니다.");
		session->Disconnect();
		return;
	}

	AuthenticationComponent* pAuthenticationComponent = pSession->GetAuthenticationComponent();
	pAuthenticationComponent->SetSerial(pCmd->Serial);
	pAuthenticationComponent->SetState(AuthenticationState::LobbyWait);

	S_AUTHENTICATION_IS::SetInformation(Core::InterServerClientTcp, SendStrategy::SendAsync, SingleServerType::Auth);
	S_AUTHENTICATION_IS::SEND_SAU_AuthenticationCheck(pSession->GetHandle(), pCmd->Serial);

}