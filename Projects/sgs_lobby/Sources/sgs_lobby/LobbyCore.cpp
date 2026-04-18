/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */

#include "LobbyCore.h"

#include "sgs_lobby/UnauthenticatedSessionManager.h"

#include "R_AUTHENTICATION.h"
#include "R_LOBBY.h"
#include "sg/Cmd_LOBBY.h"
#include "sgs/CmdRelay_AUTHENTICATION.h"
#include "sgs/Net/NetGroup_InterServ.h"
#include "sgs/Net/NetGroup_Main.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeLobbyCore()
{
	g_cUnauthenticatedSessionManager; // 생성

	g_cNetGroup_InterServ.Parser().AddCommand<SAU_AuthenticationCheck>(R_AUTHENTICATION::RECV_AUS_AuthenticationCheckAck);
	g_cNetGroup_Main.Parser().AddCommand<CLO_JoinLobby>(R_LOBBY::RECV_CLO_JoinLobby);
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeLobbyCore()
{
	g_cUnauthenticatedSessionManager.Free();
}
