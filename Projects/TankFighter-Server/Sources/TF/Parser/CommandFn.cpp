#define _WINSOCKAPI_

#include <TF/Parser/CommandFn.h>
#include <TF/Parser/SendFn.h>
#include <TF/Parser/QueryFn.h>

#include <Common/Command.h>
#include <TF/Game/Player.h>

#include <TF/Database/MysqlDatabase.h>
#include <TF/Game/World.h>
#include <TF/Game/Channel.h>


using namespace JNetwork;
using namespace JCore;


#define _Database	MysqlDatabase::GetInstance()
#define _World		World::GetInstance()


// LOGIN_SYN 100 커맨드
void CommandFn::CmdLoginSyn(Player* player, ICommand* cmd) {
	LoginSyn* pLoginSyn = cmd->CastCommand<LoginSyn*>();

	const auto spQuery = _Database->Query("select * from t_account where c_id = ? and c_pass = md5(?)", pLoginSyn->Id, pLoginSyn->Password);
	auto* pLoginAckPacket = new Packet<LoginAck>;
	LoginAck* pLoginAck = pLoginAckPacket->Get<0>();

	if (spQuery->GetResultRowCount()) {
		int iAccountUID = spQuery->Result().GetInt(0, 0);

		pLoginAck->UID = iAccountUID;

		if (_World->IsPlayerExist(iAccountUID)) {
			strcpy_s(pLoginAck->Reason, REASON_LEN, u8"로그인에 실패하였습니다. 이미 접속중입니다.");
			pLoginAck->Result = false;
			goto SEND;
		} 

		pLoginAck->Result = true;

		// 로그인 시간 업데이트 해줌 - 여기서 결과 볼 필요 없으므로 바로 Release
		_Database->QueryAsync("update t_account set c_last_login_time = ? where c_UID = ?", DateTime::Now(), iAccountUID)->Release();

		// 이제 해당 플레이어는 월드에 속한다.
		player->SetAccountUID(iAccountUID);
		player->UpdateLoggedInTime();
		_World->AddNewPlayer(player);
	} else {
		pLoginAck->Result = false;
		strcpy_s(pLoginAck->Reason, REASON_LEN, u8"로그인에 실패하였습니다. 일치하는 ID/PW가 없습니다.");
	}

SEND:
	player->Session()->SendAsync(pLoginAckPacket);
}


// REGISTER_SYN 102
void CommandFn::CmdRegisterSyn(Player* player, ICommand* cmd) {
	RegisterSyn* pRegisterSyn = cmd->CastCommand<RegisterSyn*>();

	const auto spIdCheckQuery = _Database->Query("select * from t_account where c_id = ?", pRegisterSyn->Id);
	auto* pRegisterAckPacket = new Packet<RegisterAck>;

	RegisterAck* pRegisterAck = pRegisterAckPacket->Get<0>();
	if (spIdCheckQuery->GetResultRowCount()) {
		pRegisterAck->Result = false;
		strcpy_s(pRegisterAck->Reason, REASON_LEN, u8"이미 존재하는 ID입니다.");
	} else {
		auto spIdCheckQuery = _Database->Query("insert into t_account (c_id, c_pass) values (?, md5(?))", pRegisterSyn->Id, pRegisterSyn->Password);

		if (spIdCheckQuery->IsSuccess()) {
			pRegisterAck->Result = true;
			strcpy_s(pRegisterAck->Reason, REASON_LEN, u8"회원가입에 성공하였습니다.");
		} else {
			pRegisterAck->Result = true;
			strcpy_s(pRegisterAck->Reason, REASON_LEN, u8"회원가입에 실패하였습니다. (insert 실패)");
		}
	} 

	player->Session()->SendAsync(pRegisterAckPacket);
}


// LOAD_CHANNEL_INFO_SYN 104
void CommandFn::CmdLoadChannelInfoSyn(Player* player, ICommand* cmd) {
	const LoadChannelInfoSyn* pRegisterSyn = cmd->CastCommand<LoadChannelInfoSyn*>();

	const Vector<Channel*> channels = _World->GetChannels();

	player->SetPlayerState(PlayerState::ChannelSelect);


	const auto pReplyPacket = new Packet<LoadChannelInfoAck>();
	LoadChannelInfoAck* pLoadChannelInfoAck = pReplyPacket->Get<0>();
	pLoadChannelInfoAck->Count = channels.Size();

	for (int i = 0; i < channels.Size(); i++) {
		strcpy_s(pLoadChannelInfoAck->Info[i].Name, NAME_LEN, channels[i]->GetChnnelName().Source());
		pLoadChannelInfoAck->Info[i].ChannelUID = channels[i]->GetChannelUID();
		pLoadChannelInfoAck->Info[i].MaxPlayerCount = channels[i]->GetMaxPlayerCount();
		pLoadChannelInfoAck->Info[i].PlayerCount = channels[i]->GetPlayerCount();
	}

	player->Session()->SendAsync(pReplyPacket);
}

// SELECT_CHANNEL_SYN 106
void CommandFn::CmdSelectChannelSyn(Player* player, ICommand* cmd) {
	const SelectChannelSyn* pSelectChannelSyn = cmd->CastCommand<SelectChannelSyn*>();
	const auto pReplyPacket = new Packet<SelectChannelAck>();
	SelectChannelAck* pSelectChannelAck = pReplyPacket->Get<0>();

	Channel* pSelectedChannel =  _World->GetChannel(pSelectChannelSyn->ChanneldUID);

	// 플레이어 채널 UID 설정 및 채널에 추가
	if (pSelectedChannel->TryAddPlayer(player)) {
		pSelectChannelAck->Result = true;
		pSelectChannelAck->ChanneldUID = pSelectChannelSyn->ChanneldUID;
		strcpy_s(pSelectChannelAck->Reason, REASON_LEN, u8"채널 진입 성공");
	} else {
		pSelectChannelAck->Result = false;
		strcpy_s(pSelectChannelAck->Reason, REASON_LEN, u8"인원이 꽉 찼습니다.");
	}

	player->Session()->SendAsync(pReplyPacket);
}


// LOAD_CHARACTER_INFO_SYN 108
void CommandFn::CmdLoadCharacterInfoSyn(Player* player, ICommand* cmd) {
	const LoadCharacterInfoSyn* pLoadCharacterInfoSyn = cmd->CastCommand<LoadCharacterInfoSyn*>();

	const int iAccountUID = pLoadCharacterInfoSyn->AccountUID;
	const int iChannelUID = pLoadCharacterInfoSyn->ChannelUID;

	if (iAccountUID != player->GetAccountUID() ||
		iChannelUID != player->GetChannelUID()) {
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "잘못된 유저입니다.");
		player->Session()->Disconnect();
		return;
	}

	player->SetPlayerState(PlayerState::CharacterSelect);
	SendFn::SendLoadCharacterInfoAck(player);
}


// SELECT_CHARACTER_SYN 114
void CommandFn::CmdSelectCharacterSyn(Player* player, ICommand* cmd) {
	const SelectCharacterSyn* pLoadCharacterInfoSyn = cmd->CastCommand<SelectCharacterSyn*>();

	const int iAccountUID = pLoadCharacterInfoSyn->AccountUID;
	const int iChannelUID = pLoadCharacterInfoSyn->ChannelUID;
	const int iCharacterUID = pLoadCharacterInfoSyn->CharacterUID;

	if (iAccountUID != player->GetAccountUID() ||
		iChannelUID != player->GetChannelUID()) {
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "잘못된 유저입니다.");
		player->Session()->Disconnect();
		return;
	}

	const auto pReplyPacket = new Packet<SelectCharacterAck>;
	SelectCharacterAck* pSelectCharacterAck = pReplyPacket->Get<0>();

	if (QueryFn::IsCharacterExistByIDs(iAccountUID, iChannelUID, iCharacterUID)) {
		pSelectCharacterAck->Result = true;
		pSelectCharacterAck->CharacterUID = iCharacterUID;
	} else {
		pSelectCharacterAck->Result = false;
		strcpy_s(pSelectCharacterAck->Reason, REASON_LEN, u8"선택하신 캐릭터의 정보가 존재하지 않습니다.");
		SendFn::SendLoadCharacterInfoAck(player);
	}

	
	player->Session()->SendAsync(pReplyPacket);
}


// CREATE_CHARACTER_SYN 110
void CommandFn::CmdCreateCharacterSyn(Player* player, ICommand* cmd) {
	const CreateCharacterSyn* pCreateCharacterSyn = cmd->CastCommand<CreateCharacterSyn*>();

	const int iAccountUID = pCreateCharacterSyn->AccountUID;
	const int iChannelUID = pCreateCharacterSyn->ChannelUID;

	if (iAccountUID != player->GetAccountUID() ||
		iChannelUID != player->GetChannelUID()) {
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "잘못된 유저입니다.");
		player->Session()->Disconnect();
		return;
	}

	const auto pReplyPacket = new Packet<CreateCharacterAck>;
	CreateCharacterAck* pCreateCharacterAck = pReplyPacket->Get<0>();
	const String szCharacterName = pCreateCharacterSyn->CharacterName;

	pCreateCharacterAck->Result = false;
	if (QueryFn::IsCharacterExistByName(szCharacterName)) {
		strcpy_s(pCreateCharacterAck->Reason, REASON_LEN, u8"이미 해당 캐릭터 명이 이미 존재합니다.");
	} else {

		// 일부러 동기로 진행해줌
		if (_Database->Query("insert into t_character (c_account_uid, c_channel_uid, c_name, c_win, c_lose, c_kill, c_death, c_money) values (?, ?, ?, ?, ?, ?, ?, ?)",
			iAccountUID, iChannelUID, szCharacterName, 0, 0, 0, 0, 1000000)->IsFailed()) {
			strcpy_s(pCreateCharacterAck->Reason, REASON_LEN, u8"캐릭터 생성에 실패했습니다. (select failed)");
			goto SEND;
		}

		pCreateCharacterAck->Result = true;
		strcpy_s(pCreateCharacterAck->Reason, REASON_LEN, u8"캐릭터가 성공적으로 생성되었습니다!");
		// 캐릭터 생성되면 캐릭목록을 재전송해주자.
		SendFn::SendLoadCharacterInfoAck(player);
	}

SEND:
	player->Session()->SendAsync(pReplyPacket);
}


// DELETE_CHARACTER_SYN 112
void CommandFn::CmdDeleteCharacterSyn(Player* player, ICommand* cmd) {
	const DeleteCharacterSyn* pDeleteCharacterSyn = cmd->CastCommand<DeleteCharacterSyn*>();

	const int iAccountUID = pDeleteCharacterSyn->AccountUID;
	const int iChannelUID = pDeleteCharacterSyn->ChannelUID;

	if (iAccountUID != player->GetAccountUID() ||
		iChannelUID != player->GetChannelUID()) {
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "잘못된 유저입니다.");
		player->Session()->Disconnect();
		return;
	}

	const auto pReplyPacket = new Packet<DeleteCharacterAck>;
	DeleteCharacterAck* pDeleteCharacterAck = pReplyPacket->Get<0>();

	pDeleteCharacterAck->Result = false;

	const String szCharacterName = pDeleteCharacterSyn->CharacterName;
	if (QueryFn::IsCharacterExistByName(szCharacterName)) {
		if (_Database->Query("delete from t_character where c_name = ? and c_account_uid = ? and c_channel_uid = ?",
			szCharacterName, iAccountUID, iChannelUID)->IsFailed()) {
			strcpy_s(pDeleteCharacterAck->Reason, REASON_LEN, u8"캐릭터 삭제에 실패하였습니다. account_uid, channel_uid mismatch");
			goto SEND;
		}

		pDeleteCharacterAck->Result = true;
	} else {
		strcpy_s(pDeleteCharacterAck->Reason, REASON_LEN, u8"해당 닉네임의 캐릭터가 존재하지 않습니다.");
	}

	
SEND:
	player->Session()->SendAsync(pReplyPacket);
}

// JOIN_LOBBY_SYN 116
void CommandFn::CmdJoinLobbySyn(Player* player, ICommand* cmd) {
	const JoinLobbySyn* pJoinLobbySyn = cmd->CastCommand<JoinLobbySyn*>();

	const int iAccountUID = pJoinLobbySyn->AccountUID;
	const int iChannelUID = pJoinLobbySyn->ChannelUID;
	const int iCharacterUID = pJoinLobbySyn->CharacterUID;

	if (iAccountUID != player->GetAccountUID() ||
		iChannelUID != player->GetChannelUID()) {
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "잘못된 유저입니다.");
		player->Session()->Disconnect();
		return;
	}

	player->SetCharacterUID(iCharacterUID);
	player->SetPlayerState(PlayerState::Lobby);

	// player에게 자신의 캐릭터 정보 전송
	SendFn::SendUpdateCharacterInfoAck(player);

	// player에게 방 리스트 정보 전송
	SendFn::SendUpdateFriendListAck(player, iCharacterUID);

	// player에게 친구로 등록된 캐릭터 정보들 전송
	SendFn::SendUpdateRoomListAck(player, iChannelUID);
}


// CREATE_ROOM_SYN 120
void CommandFn::CmdCreateRoomSyn(Player* player, ICommand* cmd) {
	const CreateRoomSyn* pCreateRoomSyn = cmd->CastCommand<CreateRoomSyn*>();

	// 속한 채널에서 방을 생성한다.
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());
	Room* pRoom = pChannel->CreateRoom(player, pCreateRoomSyn->RoomName, ROOM_MAX_PLAYER_COUNT);


	// 해당 플레이어는 방으로 이동할 수 있도록 ACK를 전송한다.
	const auto pReplyPacket = new Packet<CreateRoomAck>;
	CreateRoomAck* pCreateRoomAck = pReplyPacket->Get<0>();
	
	if (pRoom == nullptr) {
		pCreateRoomAck->Result = false;
		strcpy_s(pCreateRoomAck->Reason, REASON_LEN, u8"방 생성에 실패하였습니다.");
	} else {
		pCreateRoomAck->Result = true;
		pCreateRoomAck->RoomUID = pRoom->GetRoomUID();
	}

	player->Session()->SendAsync(pReplyPacket);

	// 채널의 로비에 있는 모든 유저들에게 방리스트 목록 업데이트 패킷을 전송한다.
	SendFn::BroadcastUpdateRoomListAck(pChannel);
}


// JOIN_ROOM_SYN 121
void CommandFn::CmdJoinRoomSyn(Player* player, ICommand* cmd) {
	const JoinRoomSyn* pCreateRoomSyn = cmd->CastCommand<JoinRoomSyn*>();

	// 속한 채널에서 방을 생성한다.
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());
	const auto pReplyPacket = new Packet<JoinRoomAck>;
	JoinRoomAck* pJoinRoomAck = pReplyPacket->Get<0>();
	Room* pRoom = pChannel->JoinRoom(pCreateRoomSyn->RoomUID, player);

	if (pRoom == nullptr) {
		pJoinRoomAck->Result = false;
		strcpy_s(pJoinRoomAck->Reason, REASON_LEN, u8"해당 방이 존재하지 않거나 꽉 찼습니다.");
	} else {
		pJoinRoomAck->Result = true;
		pJoinRoomAck->RoomUID = pCreateRoomSyn->RoomUID;
	}
	player->Session()->SendAsync(pReplyPacket);

	// 채널의 로비에 있는 모든 유저들에게 방리스트 목록 업데이트 패킷을 전송한다.
	SendFn::BroadcastUpdateRoomListAck(pChannel);

	// 방에 있는 모든 유저들에게 해당 방에 있는 플레이어 정보들을 전달한다.
	SendFn::BroadcastUpdateRoomUserAck(pRoom);
}

// ADD_FRIEND_SYN 122
void CommandFn::CmdAddFriendSyn(Player* player, ICommand* cmd) {
	const AddFriendSyn* pAddFriendSyn = cmd->CastCommand<AddFriendSyn*>();
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	// 플레이어가 속한 채널의 유저들을 확인하면서 친구 요청 패킷을 보낸다.
	Player* pFind =pChannel->PlayerFindIf([pAddFriendSyn](Player* playerInChannel)->bool {
		return playerInChannel->CheckNameEqual(pAddFriendSyn->FriendName);
	});

	const auto pReplyPacket = new Packet<AddFriendAck>;
	AddFriendAck* pAddFriendAck = pReplyPacket->Get<0>();
	if (pFind == nullptr) {
		pAddFriendAck->Result = false;
		strcpy_s(pAddFriendAck->Reason, REASON_LEN, u8"해당 플레이어가 현재 채널에 접속중이지 않습니다.");
	} else {
		pAddFriendAck->Result = true;
		strcpy_s(pAddFriendAck->Reason, REASON_LEN, u8"요청을 성공적으로 보냈습니다.");

		// 친구 요청 대상에게 누군가 친구 요청했음을 알려준다.
		const auto pRequestPacket = new Packet<AddFriendRequestSyn>;
		AddFriendRequestSyn* pAddFriendRequestSyn = pRequestPacket->Get<0>();
		pAddFriendRequestSyn->RequestCharacterUID = player->GetCharacterUID();
		pFind->Session()->SendAsync(pRequestPacket);
	}

	// 플레이어에게 요청 보낸 결과를 송신한다.
	player->Session()->SendAsync(pReplyPacket);
}

// DELETE_FRIEND_SYN 123
void CommandFn::CmdDeleteFriendSyn(Player* player, ICommand* cmd) {
	const DeleteFriendSyn* pDeleteFriendSyn = cmd->CastCommand<DeleteFriendSyn*>();

	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	Player* pRequest = player;	// 친구 삭제를 요청한 사람
	Player* pDeleted = pChannel->FindPlayerByCharacterUID(pDeleteFriendSyn->DeleteCharacterUID);

	int iRequesterCharacterUID = pRequest->GetCharacterUID();
	int iDeletedCharacterUID = pDeleted->GetCharacterUID();

	const auto pReplyPacket = new Packet<DeleteFriendAck>;
	DeleteFriendAck* pDeleteFriendAck = pReplyPacket->Get<0>();

	if (_Database->Query("delete from t_friendship where (c_req_character_uid = ? and c_ack_character_uid = ?) or (c_ack_character_uid = ? and c_req_character_uid = ?)",
		iRequesterCharacterUID, iDeletedCharacterUID, iDeletedCharacterUID, iRequesterCharacterUID)->IsSuccess()) {
		pDeleteFriendAck->Result = true;
		strcpy_s(pDeleteFriendAck->Reason, REASON_LEN, u8"성공적으로 친구가 삭제되었습니다.");


		// 삭제 대상이 접속중인 경우 알려준다.
		if (pDeleted) {
			// 누가 널 친구 삭제했다고
			SendFn::SendServerMessageSyn(pDeleted, u8"누군가 당신을 친구삭제 하였습니다.");

			// 친구 목록도 갱신해준다.
			SendFn::SendUpdateFriendListAck(pDeleted, iDeletedCharacterUID );
		}

		// 삭제를 요청한 사람도 갱신해줌
		SendFn::SendUpdateFriendListAck(pRequest, iRequesterCharacterUID);
	} else {
		pDeleteFriendAck->Result = false;
		strcpy_s(pDeleteFriendAck->Reason, REASON_LEN, u8"요청을 성공적으로 보냈습니다.");
	}

	player->Session()->SendAsync(pReplyPacket);
}

// ADD_FRIEND_REQUEST_ACK 128
void CommandFn::CmdFriendRequestAck(Player* player, ICommand* cmd) {
	const AddFriendRequestAck* pAddFriendRequestAck = cmd->CastCommand<AddFriendRequestAck*>();

	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	Player* pAccepter = player;	// 친구 요청을 수락 또는 거부한사람
	Player* pRequester = pChannel->FindPlayerByCharacterUID(pAddFriendRequestAck->RequestCharacterUID);
	 
	// 요청 보낸 사람이 현재 접속중이고
	if (pRequester) {
		const auto pRequestPacket = new Packet<AddFriendRequestResultSyn>;
		AddFriendRequestResultSyn* pAddFriendRequestResultSyn = pRequestPacket->Get<0>();

		// 수락한 경우
		if (pAddFriendRequestAck->Accept) {
			pAddFriendRequestResultSyn->Result = true;
			strcpy_s(pAddFriendRequestResultSyn->Reason, REASON_LEN, u8"상대방이 친구 요청을 수락하였습니다.");
		} else { // 거절한 경우
			pAddFriendRequestResultSyn->Result = false;
			strcpy_s(pAddFriendRequestResultSyn->Reason, REASON_LEN, u8"상대방이 친구 요청을 거절하였습니다.");
		}

		pRequester->Session()->SendAsync(pRequestPacket);
	}

	// 1. 수락한 경우 친구 요청/수락자 모두 친구 목록을 갱신해준다.
	// 2. 데이버베이스에 친구 관계를 추가해준다.
	if (pAddFriendRequestAck->Accept) {
		_Database->QueryAsync("insert into t_friendship (c_req_character_uid, c_ack_character_uid) values (?, ?)", 
			pRequester->GetCharacterUID(), pAccepter->GetCharacterUID())->Release();

		// 한번에 보내는게 효율적이긴 한데 코드 다시 짜야해서 일단.. 이렇게
		SendFn::SendUpdateFriendListAck(pAccepter, pAccepter->GetCharacterUID());
		SendFn::SendUpdateFriendListAck(pRequester, pRequester->GetCharacterUID());
	}
}


void CommandFn::CmdTankMoveSyn(Player* session, ICommand* cmd) {
	TcpTankMoveSyn* pTankMoveSyn =  cmd->CastCommand<TcpTankMoveSyn*>();
}

void CommandFn::CmdTankMoveAck(Player* session, ICommand* cmd) {
	TcpTankMoveSyn* pTankMoveSyn = cmd->CastCommand<TcpTankMoveSyn*>();
}




