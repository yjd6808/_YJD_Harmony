/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 12:35:13 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>

struct R_GAME
{
	static void RECV_SC_Login(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_Disconnect(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_Register(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_LoadChannelInfo(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_SelectChannel(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_LeaveChannel(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_LoadCharacterInfo(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_LeaveLobby(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_SelectCharacter(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_UpdateCharacterInfo(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_UpdateRoomList(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_UpdatePlayerList(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_UpdateFriendList(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_CreateRoom(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_JoinRoom(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_RoomGameStart(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_RoomGameReady(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_RoomGameIntrude(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_RoomLeave(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_LoadRoomInfo(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_UpdateRoomMemberList(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_AddFriendRequest(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_BattleFieldLoad(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_BattleFieldLeave(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_ServerMessage(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_ChatMessage(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_BattleFieldFire(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_BattleFieldDeath(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_BattleFieldRevival(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_BattleFieldStatisticsUpdate(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RECV_SC_TcpRTT(JNetwork::Session* session, JNetwork::ICommand* cmd);
};