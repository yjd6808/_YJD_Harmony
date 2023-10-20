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
struct RoomInfo;

class Room;
class Channel;
class ChannelLobby;

struct S_GAME : JNetwork::SendHelper<S_GAME>
{
	static void SEND_SC_Login(bool success, int accountPrimaryKey);
	static void SEND_SC_Register(bool success);
	static void SEND_SC_LoadChannelInfo(const JCore::Vector<ChannelInfo>& channelInfoList);
	static void SEND_SC_SelectChannel(int channelPrimaryKey);
	static void SEND_SC_LeaveChannel();
	static void SEND_SC_LoadCharacterInfo(int accountPrimaryKey, int channelPrimaryKey);
	static void SEND_SC_SelectCharacterAndJoinLobby(const CharacterInfo& info);
	static void SEND_SC_UpdateCharacterInfo(Character* character);
	static void SEND_SC_UpdateRoomList(ChannelLobby* lobby);
	static void SEND_SC_UpdateFriendList(Character* character);
	static void SEND_SC_UpdatePlayerListBroadcastInLobby(ChannelLobby* pLobby);
	static void SEND_SC_UpdatePlayerList(ChannelLobby* pLobby);
	static void SEND_SC_LeaveLobby();
	static void SEND_SC_CreateRoom(int roomAccessId);
	static void SEND_SC_JoinRoom(int roomAccessId);
	static void SEND_SC_LoadRoomInfo(Room* room);
	static void SEND_SC_AddFriendRequest(Character* character);
	static void SEND_SC_RoomGameReadyBroadcast(Room* room, int characterPrimaryKey, bool ready);
	static void SEND_SC_RoomLeave();
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
	static bool SEND_SC_Disconnect(JNetwork::Session* session);
	static void SEND_SC_ChatMessageBroadcastLobby(IBroadcastable* broadcastable, const char* msg);
	static void SEND_SC_ChatMessageBroadcastBattleField(IBroadcastable* broadcastable, const char* msg);
	

};


