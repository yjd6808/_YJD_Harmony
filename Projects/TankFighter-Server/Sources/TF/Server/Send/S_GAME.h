/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 11:20:08 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/SendHelper.h>

struct ChannelInfo;
struct CharacterInfo;

struct S_GAME : JNetwork::SendHelper<S_GAME>
{
	static void SEND_SC_Login(bool success, int accountPrimaryKey);
	static void SEND_SC_Register(bool success);
	static void SEND_SC_LoadChannelInfo(ChannelInfo* channelInfo, int count);
	static void SEND_SC_SelectChannel(int channelPrimaryKey);
	static void SEND_SC_LeaveChannel();
	static void SEND_SC_LoadCharacterInfo(int accountPrimaryKey, int channelPrimaryKey);
	static void SEND_SC_SelectCharacter(const CharacterInfo& info);
	//static void SEND_SC_UpdateCharacterInfo();
	//static void SEND_SC_UpdateRoomList();
	//static void SEND_SC_UpdateFriendList();
	//static void SEND_SC_CreateRoom();
	//static void SEND_SC_JoinRoom();
	//static void SEND_SC_UpdateRoomInfo();
	//static void SEND_SC_AddFriend();
	//static void SEND_SC_AddFriendRequest();
	//static void SEND_SC_DeleteFriend();
	//static void SEND_SC_BattleFieldLoad();
	//static void SEND_SC_BattleFieldLeave();
	static void SEND_SC_ServerMessage(const char* msg);
	static void SEND_SC_ServerMessage(const JCore::String& msg);
	//static void SEND_SC_ChatMessage();
	//static void SEND_SC_BattleFieldFire();
	//static void SEND_SC_BattleFieldDeath();
	//static void SEND_SC_BattleFieldRevival();
	//static void SEND_SC_BattleFieldStatisticsUpdate();
	//static void SEND_SC_TcpRTT();
};

