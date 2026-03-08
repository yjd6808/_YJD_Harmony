/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 4:14:29 PM
 * =====================
 *
 */



#include "Core.h"
#include "R_AUTHENTICATION.h"

#include <sgs/AuthenticationComponent.h>
#include <sgs/CmdRelay_AUTHENTICATION.h>
#include <sgs/S_MESSAGE_COMMON.h>
#include <sgs/_Net/NetGroup_Main.h>

#include <sgs_lobby/UnauthenticatedSessionManager.h>
#include <sgs_lobby/S_LOBBY.h>


USING_NS_JC;
USING_NS_SG;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_AUTHENTICATION::RECV_AUS_AuthenticationCheckAck(UNUSED Session*, ICommand* _pCommand)
{
	AUS_AuthenticationCheckAck* pAuthenticationCheckAck = (AUS_AuthenticationCheckAck*)_pCommand;
	NetSession* pSession = g_cNetGroup_Main.GetSession(pAuthenticationCheckAck->SessionHandle);

	if (pSession == nullptr || !pSession->IsConnected())
	{
		return;
	}

	if (!pAuthenticationCheckAck->Success)
	{
		S_MESSAGE_COMMON::SetInformation(pSession, SendStrategy::SendAsync);
		S_MESSAGE_COMMON::SEND_SC_ClientText("AUTHENTICATION_LOBBY_FAILED");
		pSession->Disconnect();
		return;
	}

	sg::AuthenticationComponent* pAuthenticationComponent = pSession->GetComponent<sg::AuthenticationComponent>();

	if (!g_cUnauthenticatedSessionManager.Remove(pAuthenticationComponent->GetSerial()))
	{
		_LogWarn_("인증이 성공했는데, 미인증 세션 목록에 없습니다.");
		S_MESSAGE_COMMON::SetInformation(pSession, SendStrategy::SendAsync);
		S_MESSAGE_COMMON::SEND_SC_ClientText("AUTHENTICATION_LOBBY_FAILED");
		pSession->Disconnect();
		return;
	}

	pAuthenticationComponent->SetState(AuthenticationState::Lobby);
	S_LOBBY::SetInformation(pSession, SendStrategy::SendAsync);
	S_LOBBY::SEND_LOC_JoinLobbyAck(pAuthenticationCheckAck->LastServer);
}
