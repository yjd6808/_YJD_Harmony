/*
 * 작성자: 윤정도
 * 생성일: 5/9/2023 2:32:19 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "S_LOBBY.h"

#include <SteinsGate/Common/Cmd_LOBBY.h>
#include <SteinsGate/Client/AuthenticationComponent.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void S_LOBBY::SEND_CLO_JoinLobby()
{
	const auto sending = SendBegin<CLO_JoinLobby>();
	const AuthenticationComponent* pAuthenticationComponent = Core::Net->GetAuthenticationComponent();
	sending.Cmd.AccountId = pAuthenticationComponent->GetAccountData().id_;
	sending.Cmd.Serial = pAuthenticationComponent->GetSerial();
}

//////////////////////////////////////////////////////////////////////////////////////////
void S_LOBBY::SEND_CLO_LoadChannelInfo(GameServerType_t _serverType)
{
	const auto sending = SendBegin<CLO_LoadChannelInfo>();
	sending.Cmd.Server = _serverType;
}
