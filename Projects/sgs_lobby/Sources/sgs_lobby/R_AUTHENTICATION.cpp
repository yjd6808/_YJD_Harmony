/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 4:14:29 PM
 * =====================
 *
 */



#include "Core.h"
#include "LobbyCoreHeader.h"
#include "R_AUTHENTICATION.h"

#include <sgs/AuthenticationComponent.h>
#include <sgs/CmdRelay_AUTHENTICATION.h>
#include <sgs/S_MESSAGE_COMMON.h>
#include <sgs/UnauthenticatedSessionManager.h>

#include <sgs_lobby/LobbySession.h>
#include <sgs_lobby/S_LOBBY.h>




USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_AUTHENTICATION::RECV_AUS_AuthenticationCheckAck(UNUSED Session*, ICommand* _pCommand)
{
	AUS_AuthenticationCheckAck* pAuthenticationCheckAck = (AUS_AuthenticationCheckAck*)_pCommand;
	LobbySession* pLobbySession = (LobbySession*)sg::NetGroup->GetSessionFromContainer(pAuthenticationCheckAck->SessionHandle);

	if (pLobbySession == nullptr || !pLobbySession->IsConnected())
	{
		return;
	}

	if (!pAuthenticationCheckAck->Success)
	{
		S_MESSAGE_COMMON::SetInformation(pLobbySession, SendStrategy::SendAsync);
		S_MESSAGE_COMMON::SEND_SC_ClientText("AUTHENTICATION_LOBBY_FAILED");
		pLobbySession->Disconnect();
		return;
	}

	AuthenticationComponent* pAuthenticationComponent = pLobbySession->GetAuthenticationComponent(true);
	
	if (!sg::Contents.UnauthenticatedSessionManager->Remove(pAuthenticationComponent->GetSerial()))
	{
		_LogWarn_("인증이 성공했는데, 미인증 세션 목록에 없습니다.");
		S_MESSAGE_COMMON::SetInformation(pLobbySession, SendStrategy::SendAsync);
		S_MESSAGE_COMMON::SEND_SC_ClientText("AUTHENTICATION_LOBBY_FAILED");
		pLobbySession->Disconnect();
		return;
	}

	pAuthenticationComponent->SetState(AuthenticationState::Lobby);
	S_LOBBY::SetInformation(pLobbySession, SendStrategy::SendAsync);
	S_LOBBY::SEND_LOC_JoinLobbyAck(pAuthenticationCheckAck->LastServer);
}
