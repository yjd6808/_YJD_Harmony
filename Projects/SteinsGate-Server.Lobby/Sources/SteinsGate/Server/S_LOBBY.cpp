/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 5:56:01 PM
 * =====================
 *
 */

#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "S_LOBBY.h"

#include <SteinsGate/Common/Cmd_LOBBY.h>

void S_LOBBY::SEND_LOC_JoinLobbyAck(bool success) {
	auto sending = SendBegin<LOC_JoinLobbyAck>();
	sending.Cmd.Success = success;
}
