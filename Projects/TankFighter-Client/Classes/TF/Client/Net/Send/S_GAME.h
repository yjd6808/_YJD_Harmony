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
	static void SEND_CS_SelectChannel(int channelPrimaryKey);
	static bool SEND_CS_LoadCharacterInfo(int accountPrimaryKey, int channelPrimaryKey);
	static void SEND_CS_LeaveChannel(int channelPrimaryKey);
	static void SEND_CS_CreateCharacter(const char* nickName);
	static void SEND_CS_DeleteCharacter(const char* nickName);
	static void SEND_CS_SelectCharacter(int characterPrimaryKey);
	static bool SEND_CS_JoinLobby();
	// static void SEND_CS_CreateRoom();
	// static void SEND_CS_JoinRoom();
	// static void SEND_CS_AddFriend();
	// static void SEND_CS_AddFriendRequest();
	// static void SEND_CS_DeleteFriend();
	// static void SEND_CS_ServerMessage();
	// static void SEND_CS_LoadRoomInfo();
	// static void SEND_CS_RoomGameStart();
	// static void SEND_CS_RoomGameReady();
	// static void SEND_CS_RoomGameReadyCancel();
	// static void SEND_CS_RoomLeave();
	// static void SEND_CS_BattleFieldLoad();
	// static void SEND_CS_BattileFieldTankMove();
	// static void SEND_CS_BattileFieldTankUpdate();
	// static void SEND_CS_BattleFieldPlayWaitEnd();
	// static void SEND_CS_BattleFieldPlayingEnd();
	// static void SEND_CS_BattleFieldEndWaitEnd();
	// static void SEND_CS_BattleFieldLeave();
	// static void SEND_CS_ChatMessage();
	// static void SEND_CS_BattleFieldFire();
	// static void SEND_CS_BattleFieldDeath();
	// static void SEND_CS_BattleFieldRevival();
	// static void SEND_CS_BattleFieldStatisticsUpdate();
	// static void SEND_CS_TcpRTT();








};
