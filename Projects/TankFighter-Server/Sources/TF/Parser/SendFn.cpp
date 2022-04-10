/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
#include <TF/Parser/SendFn.h>
#include <Common/Command.h>
#include <TF/Game/Player.h>

#include <TF/Database/MysqlDatabase.h>
#include <TF/Game/World.h>
#include <TF/Game/Channel.h>

using namespace JNetwork;
using namespace JCore;


#define _Database	MysqlDatabase::GetInstance()
#define _World		World::GetInstance()


void SendFn::SendUpdateFriendListAck(Player* player, int characterUID) {
	const auto pPacket = new Packet<UpdateFriendListAck>;
	UpdateFriendListAck* pUpdateFriendListAck = pPacket->Get<0>();

	std::string query(250, '\0');
	query = "select * from t_character where c_uid = ( "
						"select c_ack_character_uid from t_friendship where c_req_character_uid = ? "
						"union all "
						"select c_req_character_uid from t_friendship where c_ack_character_uid = ? "
					")";
	
	const auto spQueryResult = _Database->Query(query, characterUID, characterUID);
	pUpdateFriendListAck->Count = spQueryResult->GetResultRowCount();

	const int iCharacterUID = player->GetChannelUID();
	Channel* pChannel = _World->GetChannel(iCharacterUID);

	if (pChannel == nullptr) {
		SendFn::SendServerMessageSyn(player, "당신의 채널 정보가 이상합니다.");
		return;
	}

	for (int i = 0; i < pUpdateFriendListAck->Count; i++) {
		CharacterInfo* info = &pUpdateFriendListAck->Info[i];
		info->CharacterUID = spQueryResult->GetInt(i, 0);
		strcpy_s(info->Name, NAME_LEN, spQueryResult->GetString(i, 3).Source());
		info->Win = spQueryResult->GetInt(i, 4);
		info->Lose = spQueryResult->GetInt(i, 5);
		info->Kill = spQueryResult->GetInt(i, 6);
		info->Death = spQueryResult->GetInt(i, 7);
		info->Money = spQueryResult->GetInt(i, 8);

		int iFriendUID = info->CharacterUID;

		// 현재 접속중인지 확인하기 위햇 현재 채널에서 해당 플레이어를 찾는다.
		Player* pFriendPlayer = pChannel->FindPlayerByCharacterUID(iCharacterUID);
		info->PlayerState = pFriendPlayer ? pFriendPlayer->GetPlayerState() : PlayerState::Disconnected;
	}

	player->SendAsync(pPacket);
}

void SendFn::SendServerMessageSyn(Player* player, const String& message) {
	if (player == nullptr) {
		return;
	}

	const auto pPacket = new Packet<ServerMessageSyn>;
	ServerMessageSyn* pServerMessageSyn = pPacket->Get<0>();
	strcpy_s(pServerMessageSyn->Message, MESSAGE_LEN, message.Source());
	player->SendAsync(pPacket);
}

// 플레이어에게 방 정보 전송
void SendFn::SendRoomInfoAck(Room* room, Player* player) {
	const auto pPacket = new Packet<LoadRoomInfoAck>;
	LoadRoomInfoAck* pLoadRoomInfoAck = pPacket->Get<0>();

	if (room == nullptr) {
		pLoadRoomInfoAck->Result = false;
		strcpy_s(pLoadRoomInfoAck->Reason, REASON_LEN, u8"참가한 방 정보가 없습니다.");
	} else {
		pLoadRoomInfoAck->Result = true;
		room->LoadRoomInfo(pLoadRoomInfoAck->Info);
	}
	player->SendAsync(pPacket);
}


// LoadCharacterInfoAck 패킷 전달
// 캐릭터 정보들을 클라한테 전달함
void SendFn::SendLoadCharacterInfoAck(Player* player) {
	const auto pPacket = new Packet<LoadCharacterInfoAck>;
	LoadCharacterInfoAck* pLoadCharacterInfoAck = pPacket->Get<0>();

	const auto spQueryResult = _Database->Query("select * from t_character where c_account_uid = ? and c_channel_uid = ?", player->GetAccountUID(), player->GetChannelUID());
	const int iCharacterCount = spQueryResult->GetResultRowCount();
	pLoadCharacterInfoAck->Count = iCharacterCount;
	for (int i = 0; i < iCharacterCount; i++) {
		CharacterInfo* pCharacterInfo = &pLoadCharacterInfoAck->Info[i];

		pCharacterInfo->CharacterUID = spQueryResult->GetInt(i, 0);
		strcpy_s(pCharacterInfo->Name, NAME_LEN, spQueryResult->GetString(i, 3).Source());
		pCharacterInfo->Win = spQueryResult->GetInt(i, 4);
		pCharacterInfo->Lose = spQueryResult->GetInt(i, 5);
		pCharacterInfo->Kill = spQueryResult->GetInt(i, 6);
		pCharacterInfo->Death = spQueryResult->GetInt(i, 7);
		pCharacterInfo->Money = spQueryResult->GetInt(i, 8);
	}

	player->SendAsync(pPacket);
}



void SendFn::SendUpdateCharacterInfoAck(Player* player) {
	const auto pPacket = new Packet<UpdateCharacterInfoAck>;
	UpdateCharacterInfoAck* pUpdateCharacterInfoAck = pPacket->Get<0>();

	const auto spQueryResult = _Database->Query("select * from t_character where c_account_uid = ? and c_channel_uid = ? and c_uid = ?", 
		player->GetAccountUID(), player->GetChannelUID(), player->GetCharacterUID());

	if(spQueryResult->GetResultRowCount()) {
		pUpdateCharacterInfoAck->Result = true;
		CharacterInfo* pMyInfo = &pUpdateCharacterInfoAck->Info;
		pMyInfo->CharacterUID = spQueryResult->GetInt(0, 0);
		strcpy_s(pMyInfo->Name, NAME_LEN, spQueryResult->GetString(0, 3).Source());
		pMyInfo->Win = spQueryResult->GetInt(0, 4);
		pMyInfo->PlayerState = player->GetPlayerState();
		pMyInfo->Lose = spQueryResult->GetInt(0, 5);
		pMyInfo->Kill = spQueryResult->GetInt(0, 6);
		pMyInfo->Death = spQueryResult->GetInt(0, 7);
		pMyInfo->Money = spQueryResult->GetInt(0, 8);
		player->UpdateCharacterInfo(*pMyInfo);
	} else {
		pUpdateCharacterInfoAck->Result = false;
		strcpy_s(pUpdateCharacterInfoAck->Reason, REASON_LEN, u8"캐릭터 정보가 없습니다. account_uid, channel_uid, character_uid mismatch");
	}

	player->SendAsync(pPacket);
}

void SendFn::SendUpdateRoomListAck(Player* player, int channelUID) {
	const auto pPacket = new Packet<UpdateRoomListAck>;
	UpdateRoomListAck* pUpdateRoomListAck = pPacket->Get<0>();
	Channel* pChannel = _World->GetChannel(channelUID);
	int iIndexer = 0;

	if (pChannel == nullptr) {
		pUpdateRoomListAck->Result = false;
		strcpy_s(pUpdateRoomListAck->Reason, REASON_LEN, u8"채널 정보가 이상합니다.");
	} else {
		pUpdateRoomListAck->Result = true;
		pChannel->RoomForEach([&iIndexer, &pUpdateRoomListAck](Room* room) {
			RoomInfo* pRoomInfo = &pUpdateRoomListAck->Info[iIndexer];
			pRoomInfo->PlayerCount = room->GetPlayerCount();
			pRoomInfo->MaxPlayerCount = room->GetMaxPlayerCount();
			pRoomInfo->RoomUID = room->GetRoomUID();
			strcpy_s(pRoomInfo->Name, NAME_LEN, room->GetRoomName().Source());
			iIndexer++;
		});
		pUpdateRoomListAck->Count = iIndexer;
	}

	player->SendAsync(pPacket);
}



void SendFn::BroadcastUpdateRoomListAck(Channel* channel) {
	const auto pPacket = new Packet<UpdateRoomListAck>;
	UpdateRoomListAck* pUpdateRoomListAck = pPacket->Get<0>();
	int iIndexer = 0;
	pUpdateRoomListAck->Result = true;

	channel->RoomForEach([&iIndexer, &pUpdateRoomListAck](Room* room) {
		RoomInfo* pRoomInfo = &pUpdateRoomListAck->Info[iIndexer];
		pRoomInfo->PlayerCount = room->GetPlayerCount();
		pRoomInfo->MaxPlayerCount = room->GetMaxPlayerCount();
		pRoomInfo->RoomUID = room->GetRoomUID();
		strcpy_s(pRoomInfo->Name, NAME_LEN, room->GetRoomName().Source());
		iIndexer++;
	});
	pUpdateRoomListAck->Count = iIndexer;

	channel->BroadcastLobbyPacket(pPacket);
}




// 방에 있는 모든 유저들에게 해당 방에 있는 플레이어 정보들을 전달한다.
void SendFn::BroadcastUpdateRoomUserAck(Room* room, bool unsafe) {
	if (room == nullptr) {
		return;
	}

	const auto pPacket = new Packet<UpdateRoomInfoAck>;
	UpdateRoomInfoAck* pUpdateRoomInfoAck = pPacket->Get<0>();
	int iIndexer = 0;

	pUpdateRoomInfoAck->HostCharacterUID = room->GetHost()->GetCharacterUID();

	if (unsafe) {
		room->UnsafeForEach([&iIndexer, pUpdateRoomInfoAck](Player* player) {
			player->LoadRoomCharacterInfo(pUpdateRoomInfoAck->Info[iIndexer]);
			iIndexer++;
		});
		
	} else {
		room->ForEach([&iIndexer, pUpdateRoomInfoAck](Player* player) {
			player->LoadRoomCharacterInfo(pUpdateRoomInfoAck->Info[iIndexer]);
			iIndexer++;
		});
	}
	pUpdateRoomInfoAck->Count = iIndexer;

	if (unsafe) {
		room->UnsafeBroadcast(pPacket);
	} else {
		room->Broadcast(pPacket);
	}
}


void SendFn::SendRTTAck(Player* player, Int64U tick) {
	const auto pPacket = new Packet<TcpRTTAck>;
	TcpRTTAck* pTcpRTTAck = pPacket->Get<0>();
	pTcpRTTAck->Tick = tick;
	player->SendAsync(pPacket);
}