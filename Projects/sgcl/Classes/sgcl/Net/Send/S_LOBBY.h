/*
 * 작성자: 윤정도
 * 생성일: 5/9/2023 2:32:08 PM
 * =====================
 *
 */


#pragma once

#include "jnet/Packet/SendHelper.h"

struct S_LOBBY : jnet::SendHelper<S_LOBBY>
{
	static void SEND_CLO_JoinLobby();
	static void SEND_CLO_LoadChannelInfo(GameServerType_t _serverType);
};
