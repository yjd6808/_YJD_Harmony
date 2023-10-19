/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 11:19:52 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>

class R_GAME
{
public:
	static void RECV_CS_Login(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_Register(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_LoadChannelInfo(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_SelectChannel(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_LeaveChannel(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_LoadCharacterInfo(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_CreateCharacter(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_DeleteCharacter(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_SelectCharacter(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_JoinLobby(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_LeaveLobby(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_CreateRoom(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_JoinRoom(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_AddFriend(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_AddFriendRequest(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_DeleteFriend(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_LoadRoomInfo(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_RoomGameStart(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_RoomGameReady(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_RoomGameReadyCancel(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_RoomLeave(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_BattleFieldLoad(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_BattileFieldTankMove(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_BattileFieldTankUpdate(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_BattleFieldPlayWaitEnd(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_BattleFieldPlayingEnd(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_BattleFieldEndWaitEnd(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_BattleFieldLeave(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_ChatMessage(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_BattleFieldFire(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_BattleFieldDeath(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_BattleFieldRevival(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_BattleFieldStatisticsUpdate(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_CS_TcpRTT(JNetwork::Session* session, JNetwork::ICommand* cmd);

};



