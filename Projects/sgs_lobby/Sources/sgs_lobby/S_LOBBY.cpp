/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 5:56:01 PM
 * =====================
 *
 */

#include "Core.h"
#include "LobbyCoreHeader.h"
#include "S_LOBBY.h"

#include <sg/Cmd_LOBBY.h>

//////////////////////////////////////////////////////////////////////////////////////////
void S_LOBBY::SEND_LOC_JoinLobbyAck(GameServerType_t _serverType)
{
	auto sending = SendBegin<LOC_JoinLobbyAck>();
	sending.Cmd.DefaultSelectedServer = _serverType;
}
