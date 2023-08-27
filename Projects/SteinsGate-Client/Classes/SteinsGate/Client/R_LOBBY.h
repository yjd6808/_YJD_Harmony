/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 6:17:58 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>

struct R_LOBBY
{
public:
	static void RECV_LOC_JoinLobbyAck(JCORE_UNUSED JNetwork::Session* session, JNetwork::ICommand* cmd);
};


