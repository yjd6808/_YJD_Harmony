/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 5:55:50 PM
 * =====================
 *
 */

#pragma once

#include <jnet/Packet/SendHelper.h>

struct S_LOBBY : jnet::SendHelper<S_LOBBY>
{
	static void SEND_LOC_JoinLobbyAck(GameServerType_t _serverType);
};
