/*
 * 작성자: 윤정도
 * 생성일: 5/9/2023 2:32:19 PM
 * =====================
 *
 */


#include "Core.h"
#include "GameCoreHeader.h"
#include "S_LOBBY.h"

#include <sg/Cmd_LOBBY.h>
#include <sgcl/AuthenticationComponent.h>
#include <sgcl/NetCore.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void S_LOBBY::SEND_CLO_JoinLobby()
{
	const auto sending = SendBegin<CLO_JoinLobby>();
	sending.Cmd.AccountId = g_cNet.authentication_.GetAccountData().id_;
	sending.Cmd.Serial = g_cNet.authentication_.GetSerial();
}

//////////////////////////////////////////////////////////////////////////////////////////
void S_LOBBY::SEND_CLO_LoadChannelInfo(GameServerType_t _serverType)
{
	const auto sending = SendBegin<CLO_LoadChannelInfo>();
	sending.Cmd.Server = _serverType;
}
