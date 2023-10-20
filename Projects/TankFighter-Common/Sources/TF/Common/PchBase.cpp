/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 9:34:43 AM
 * =====================
 *
 */


#include "PchBase.h"
#include <TF/Common/Command.h>


void InitializeCommon() {
	Core::CommandNameMap.Add<CS_Login>();
	Core::CommandNameMap.Add<SC_Login>();
	Core::CommandNameMap.Add<CS_Register>();
	Core::CommandNameMap.Add<SC_Register>();
	Core::CommandNameMap.Add<CS_LoadChannelInfo>();
	Core::CommandNameMap.Add<SC_LoadChannelInfo>();
	Core::CommandNameMap.Add<CS_JoinChannel>();
	Core::CommandNameMap.Add<SC_JoinChannel>();
	Core::CommandNameMap.Add<CS_LeaveChannel>();
	Core::CommandNameMap.Add<SC_LeaveChannel>();
	Core::CommandNameMap.Add<CS_LoadCharacterInfo>();
	Core::CommandNameMap.Add<SC_LoadCharacterInfo>();
	Core::CommandNameMap.Add<CS_CreateCharacter>();
	Core::CommandNameMap.Add<CS_DeleteCharacter>();
	Core::CommandNameMap.Add<CS_SelectCharacterAndJoinLobby>();
	Core::CommandNameMap.Add<SC_SelectCharacterAndJoinLobby>();
	Core::CommandNameMap.Add<CS_LoadLobbyInfo>();
	Core::CommandNameMap.Add<CS_LeaveLobby>();
	Core::CommandNameMap.Add<SC_LeaveLobby>();
	Core::CommandNameMap.Add<SC_UpdateCharacterInfo>();
	Core::CommandNameMap.Add<SC_UpdateRoomList>();
	Core::CommandNameMap.Add<SC_UpdateFriendList>();
	Core::CommandNameMap.Add<SC_UpdatePlayerList>();
	Core::CommandNameMap.Add<CS_CreateRoom>();
	Core::CommandNameMap.Add<SC_CreateRoom>();
	Core::CommandNameMap.Add<CS_JoinRoom>();
	Core::CommandNameMap.Add<SC_JoinRoom>();
	Core::CommandNameMap.Add<SC_UpdateRoomMemberList>();
	Core::CommandNameMap.Add<CS_AddFriend>();
	Core::CommandNameMap.Add<CS_AddFriendRequest>();
	Core::CommandNameMap.Add<SC_AddFriendRequest>();
	Core::CommandNameMap.Add<CS_DeleteFriend>();
	Core::CommandNameMap.Add<CS_LoadRoomInfo>();
	Core::CommandNameMap.Add<SC_LoadRoomInfo>();
	Core::CommandNameMap.Add<CS_RoomGameStart>();
	Core::CommandNameMap.Add<SC_RoomGameStart>();
	Core::CommandNameMap.Add<CS_RoomGameReady>();
	Core::CommandNameMap.Add<CS_RoomLeave>();
	Core::CommandNameMap.Add<SC_RoomLeave>();
	Core::CommandNameMap.Add<CS_BattleFieldLoad>();
	Core::CommandNameMap.Add<SC_BattleFieldLoad>();
	Core::CommandNameMap.Add<CS_BattileFieldTankMove>();
	Core::CommandNameMap.Add<CS_BattileFieldTankUpdate>();
	Core::CommandNameMap.Add<CS_BattleFieldPlayWaitEnd>();
	Core::CommandNameMap.Add<CS_BattleFieldPlayingEnd>();
	Core::CommandNameMap.Add<CS_BattleFieldEndWaitEnd>();
	Core::CommandNameMap.Add<CS_BattleFieldLeave>();
	Core::CommandNameMap.Add<SC_BattleFieldLeave>();
	Core::CommandNameMap.Add<SC_ServerMessage>();
	Core::CommandNameMap.Add<CS_ChatMessage>();
	Core::CommandNameMap.Add<SC_ChatMessage>();
	Core::CommandNameMap.Add<CS_BattleFieldFire>();
	Core::CommandNameMap.Add<SC_BattleFieldFire>();
	Core::CommandNameMap.Add<CS_BattleFieldDeath>();
	Core::CommandNameMap.Add<SC_BattleFieldDeath>();
	Core::CommandNameMap.Add<CS_BattleFieldRevival>();
	Core::CommandNameMap.Add<CS_BattleFieldStatisticsUpdate>();
	Core::CommandNameMap.Add<CS_TcpRTT>();
	Core::CommandNameMap.Add<SC_TcpRTT>();
	Core::CommandNameMap.Add<SC_Disconnect>();
}

void FinalizeCommon() {
	
}

