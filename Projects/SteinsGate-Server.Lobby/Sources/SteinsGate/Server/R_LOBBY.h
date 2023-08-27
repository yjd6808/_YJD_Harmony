/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>

struct R_LOBBY
{
public:
	static void RECV_CLO_JoinLobby(JNetwork::Session* session, JNetwork::ICommand* cmd);
};