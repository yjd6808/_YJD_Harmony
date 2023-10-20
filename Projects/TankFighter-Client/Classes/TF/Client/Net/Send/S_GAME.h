/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 5:08:11 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/SendHelper.h>

struct S_GAME : JNetwork::SendHelper<S_GAME>
{
	static void SEND_CS_Login(const JCore::String& id, const JCore::String& pass);
	static void SEND_CS_Register(const JCore::String& id, const JCore::String& pass);
	static bool SEND_CS_LoadChannelInfo();
	static void SEND_CS_JoinChannel(int channelPrimaryKey);
	static bool SEND_CS_LoadCharacterInfo(int accountPrimaryKey, int channelPrimaryKey);
	static void SEND_CS_LeaveChannel(int channelPrimaryKey);
	static void SEND_CS_CreateCharacter(const char* nickName);
	static void SEND_CS_DeleteCharacter(const char* nickName);
	static void SEND_CS_SelectCharacterAndJoinLobby(int characterPrimaryKey);
	static bool SEND_CS_LoadLobbyInfo();
	static void SEND_CS_CreateRoom(const JCore::String& roomName);
	static void SEND_CS_JoinRoom(int roomAccessId);
	static void SEND_CS_AddFriend(const char* nickName);
	static void SEND_CS_AddFriendRequest(bool accecpt, int requestCharacterPrimaryKey);
	static void SEND_CS_DeleteFriend(int characterPrimaryKey);
	static void SEND_CS_LeaveLobby();
	static bool SEND_CS_LoadRoomInfo(int roomAccessId);
	static void SEND_CS_RoomGameStart();
	static void SEND_CS_RoomGameIntrude();
	static void SEND_CS_RoomGameReady(bool ready);
	static void SEND_CS_RoomLeave();
	// static void SEND_CS_BattleFieldLoad();
	// static void SEND_CS_BattileFieldTankMove();
	// static void SEND_CS_BattileFieldTankUpdate();
	// static void SEND_CS_BattleFieldPlayWaitEnd();
	// static void SEND_CS_BattleFieldPlayingEnd();
	// static void SEND_CS_BattleFieldEndWaitEnd();
	// static void SEND_CS_BattleFieldLeave();
	static void SEND_CS_ChatMessage(const char* msg);
	



	// static void SEND_CS_BattleFieldFire();
	// static void SEND_CS_BattleFieldDeath();
	// static void SEND_CS_BattleFieldRevival();
	// static void SEND_CS_BattleFieldStatisticsUpdate();
	// static void SEND_CS_TcpRTT();








};
