/*
 * 작성자 : 윤정도
 *
 * 1. 수신한 패킷에 대한 처리를 하도록합니다.
 * 2. 패킷 통신 코드는 모두 여기만 작성합니다.
 * 3. 단, 예외적으로 BattleFieldWorkder.cpp 에서도 패킷 송신에 대한 처리를 합니다.
 *
 */

#include <TF/PrecompiledHeader.h>
#include <TF/Parser/CommandFn.h>
#include <TF/Parser/SendFn.h>
#include <TF/Parser/QueryFn.h>

#include <JCore/Random.h>
#include <Common/Command.h>
#include <Common/GameConfiguration.h>

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
	auto* pLoginAckPacket = new StaticPacket<LoginAck>;
	LoginAck* pLoginAck = pLoginAckPacket->Get<0>();

	if (spQuery->GetResultRowCount()) {
		int iAccountUID = spQuery->GetInt(0, 0);

		pLoginAck->UID = iAccountUID;

		if (_World->IsPlayerExist(iAccountUID)) {
			strcpy_s(pLoginAck->Reason, REASON_LEN, "로그인에 실패하였습니다. 이미 접속중입니다.");
			pLoginAck->Result = false;
			goto SEND;
		} 

		pLoginAck->Result = true;

		// 로그인 시간 업데이트 해줌 - 여기서 결과 볼 필요 없으므로 바로 Release
		_Database->QueryAsync("update t_account set c_last_login_time = ? where c_uid = ?", DateTime::Now(), iAccountUID)->Release();

		// 이제 해당 플레이어는 월드에 속한다.
		player->SetAccountUID(iAccountUID);
		player->UpdateLoggedInTime();
		_World->AddNewPlayer(player);
	} else {
		pLoginAck->Result = false;
		strcpy_s(pLoginAck->Reason, REASON_LEN, "로그인에 실패하였습니다. 일치하는 ID/PW가 없습니다.");
	}

SEND:
	player->SendAsync(pLoginAckPacket);
}


// REGISTER_SYN 102
void CommandFn::CmdRegisterSyn(Player* player, ICommand* cmd) {
	RegisterSyn* pRegisterSyn = cmd->CastCommand<RegisterSyn*>();

	const auto spIdCheckQuery = _Database->Query("select * from t_account where c_id = ?", pRegisterSyn->Id);
	auto* pRegisterAckPacket = new StaticPacket<RegisterAck>;

	RegisterAck* pRegisterAck = pRegisterAckPacket->Get<0>();
	if (spIdCheckQuery->GetResultRowCount()) {
		pRegisterAck->Result = false;
		strcpy_s(pRegisterAck->Reason, REASON_LEN, "이미 존재하는 ID입니다.");
	} else {
		auto spIdCheckQuery = _Database->Query("insert into t_account (c_id, c_pass) values (?, md5(?))", pRegisterSyn->Id, pRegisterSyn->Password);

		if (spIdCheckQuery->IsSuccess()) {
			pRegisterAck->Result = true;
			strcpy_s(pRegisterAck->Reason, REASON_LEN, "회원가입에 성공하였습니다.");
		} else {
			pRegisterAck->Result = true;
			strcpy_s(pRegisterAck->Reason, REASON_LEN, "회원가입에 실패하였습니다. (insert 실패)");
		}
	} 

	player->SendAsync(pRegisterAckPacket);
}


// LOAD_CHANNEL_INFO_SYN 104
void CommandFn::CmdLoadChannelInfoSyn(Player* player, ICommand* cmd) {
	const LoadChannelInfoSyn* pRegisterSyn = cmd->CastCommand<LoadChannelInfoSyn*>();

	const Vector<Channel*> channels = _World->GetChannels();

	// 클라가 로비에서 채널선택으로 이동한 경우
	const int iChannelUID = player->GetChannelUID();
	if (iChannelUID != INVALID_UID) {
		Channel* pChannel = _World->GetChannel(iChannelUID);
		if (pChannel && pChannel->RemovePlayer(player)) {

		}
	}

	player->SetPlayerState(PlayerState::ChannelSelect);


	const auto pReplyPacket = new StaticPacket<LoadChannelInfoAck>();
	LoadChannelInfoAck* pLoadChannelInfoAck = pReplyPacket->Get<0>();
	pLoadChannelInfoAck->Count = channels.Size();

	for (int i = 0; i < channels.Size(); i++) {
		strcpy_s(pLoadChannelInfoAck->Info[i].Name, NAME_LEN, channels[i]->GetChnnelName().Source());
		pLoadChannelInfoAck->Info[i].ChannelUID = channels[i]->GetChannelUID();
		pLoadChannelInfoAck->Info[i].MaxPlayerCount = channels[i]->GetMaxPlayerCount();
		pLoadChannelInfoAck->Info[i].PlayerCount = channels[i]->GetPlayerCount();
	}

	player->SendAsync(pReplyPacket);
}

// SELECT_CHANNEL_SYN 106
void CommandFn::CmdSelectChannelSyn(Player* player, ICommand* cmd) {
	const SelectChannelSyn* pSelectChannelSyn = cmd->CastCommand<SelectChannelSyn*>();
	const auto pReplyPacket = new StaticPacket<SelectChannelAck>();
	SelectChannelAck* pSelectChannelAck = pReplyPacket->Get<0>();

	Channel* pSelectedChannel =  _World->GetChannel(pSelectChannelSyn->ChanneldUID);

	// 플레이어 채널 UID 설정 및 채널에 추가
	if (pSelectedChannel->TryAddPlayer(player)) {
		pSelectChannelAck->Result = true;
		pSelectChannelAck->ChanneldUID = pSelectChannelSyn->ChanneldUID;
		strcpy_s(pSelectChannelAck->Reason, REASON_LEN, "채널 진입 성공");
	} else {
		pSelectChannelAck->Result = false;
		strcpy_s(pSelectChannelAck->Reason, REASON_LEN, "인원이 꽉 찼습니다.");
	}

	player->SendAsync(pReplyPacket);
}


// LOAD_CHARACTER_INFO_SYN 108
void CommandFn::CmdLoadCharacterInfoSyn(Player* player, ICommand* cmd) {
	const LoadCharacterInfoSyn* pLoadCharacterInfoSyn = cmd->CastCommand<LoadCharacterInfoSyn*>();

	const int iAccountUID = pLoadCharacterInfoSyn->AccountUID;
	const int iChannelUID = pLoadCharacterInfoSyn->ChannelUID;

	

	if (iAccountUID != player->GetAccountUID() ||
		iChannelUID != player->GetChannelUID()) {
		SafeConsole::WriteLine(ConsoleColor::LightGray, "잘못된 유저입니다.");
		player->Disconnect();
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
		SafeConsole::WriteLine(ConsoleColor::LightGray, "잘못된 유저입니다.");
		player->Disconnect();
		return;
	}

	const auto pReplyPacket = new StaticPacket<SelectCharacterAck>;
	SelectCharacterAck* pSelectCharacterAck = pReplyPacket->Get<0>();

	if (QueryFn::IsCharacterExistByIDs(iAccountUID, iChannelUID, iCharacterUID)) {
		pSelectCharacterAck->Result = true;
		pSelectCharacterAck->CharacterUID = iCharacterUID;
	} else {
		pSelectCharacterAck->Result = false;
		strcpy_s(pSelectCharacterAck->Reason, REASON_LEN, "선택하신 캐릭터의 정보가 존재하지 않습니다.");
		SendFn::SendLoadCharacterInfoAck(player);
	}

	
	player->SendAsync(pReplyPacket);
}


// CREATE_CHARACTER_SYN 110
void CommandFn::CmdCreateCharacterSyn(Player* player, ICommand* cmd) {
	const CreateCharacterSyn* pCreateCharacterSyn = cmd->CastCommand<CreateCharacterSyn*>();

	const int iAccountUID = pCreateCharacterSyn->AccountUID;
	const int iChannelUID = pCreateCharacterSyn->ChannelUID;

	if (iAccountUID != player->GetAccountUID() ||
		iChannelUID != player->GetChannelUID()) {
		SafeConsole::WriteLine(ConsoleColor::LightGray, "잘못된 유저입니다.");
		player->Disconnect();
		return;
	}

	const auto pReplyPacket = new StaticPacket<CreateCharacterAck>;
	CreateCharacterAck* pCreateCharacterAck = pReplyPacket->Get<0>();
	const String szCharacterName = pCreateCharacterSyn->CharacterName;

	pCreateCharacterAck->Result = false;
	if (QueryFn::IsCharacterExistByName(szCharacterName)) {
		strcpy_s(pCreateCharacterAck->Reason, REASON_LEN, "이미 해당 캐릭터 명이 이미 존재합니다.");
	} else {

		// 일부러 동기로 진행해줌
		if (_Database->Query("insert into t_character (c_account_uid, c_channel_uid, c_name, c_win, c_lose, c_kill, c_death, c_money) values (?, ?, ?, ?, ?, ?, ?, ?)",
			iAccountUID, iChannelUID, szCharacterName, 0, 0, 0, 0, 1000000)->IsFailed()) {
			strcpy_s(pCreateCharacterAck->Reason, REASON_LEN, "캐릭터 생성에 실패했습니다. (select failed)");
			goto SEND;
		}

		pCreateCharacterAck->Result = true;
		strcpy_s(pCreateCharacterAck->Reason, REASON_LEN, "캐릭터가 성공적으로 생성되었습니다!");
		// 캐릭터 생성되면 캐릭목록을 재전송해주자.
		SendFn::SendLoadCharacterInfoAck(player);
	}

SEND:
	player->SendAsync(pReplyPacket);
}


// DELETE_CHARACTER_SYN 112
void CommandFn::CmdDeleteCharacterSyn(Player* player, ICommand* cmd) {
	const DeleteCharacterSyn* pDeleteCharacterSyn = cmd->CastCommand<DeleteCharacterSyn*>();

	const int iAccountUID = pDeleteCharacterSyn->AccountUID;
	const int iChannelUID = pDeleteCharacterSyn->ChannelUID;

	if (iAccountUID != player->GetAccountUID() ||
		iChannelUID != player->GetChannelUID()) {
		SafeConsole::WriteLine(ConsoleColor::LightGray, "잘못된 유저입니다.");
		player->Disconnect();
		return;
	}

	const auto pReplyPacket = new StaticPacket<DeleteCharacterAck>;
	DeleteCharacterAck* pDeleteCharacterAck = pReplyPacket->Get<0>();

	const String szCharacterName = pDeleteCharacterSyn->CharacterName;
	if (QueryFn::IsCharacterExistByName(szCharacterName)) {
		if (_Database->Query("delete from t_character where c_name = ? and c_account_uid = ? and c_channel_uid = ?",
			szCharacterName, iAccountUID, iChannelUID)->IsFailed()) {
			strcpy_s(pDeleteCharacterAck->Reason, REASON_LEN, "캐릭터 삭제에 실패하였습니다. account_uid, channel_uid mismatch");
		} else {
			strcpy_s(pDeleteCharacterAck->Reason, REASON_LEN, "캐릭터가 성공적으로 삭제되었습니다.");

			// 캐릭터가 삭제되면 캐릭목록을 재전송해주자.
			SendFn::SendLoadCharacterInfoAck(player);
		}
	} else {
		strcpy_s(pDeleteCharacterAck->Reason, REASON_LEN, "해당 닉네임의 캐릭터가 존재하지 않습니다.");
	}

	player->SendAsync(pReplyPacket);
}

// JOIN_LOBBY_SYN 116
void CommandFn::CmdJoinLobbySyn(Player* player, ICommand* cmd) {
	const JoinLobbySyn* pJoinLobbySyn = cmd->CastCommand<JoinLobbySyn*>();

	const int iAccountUID = pJoinLobbySyn->AccountUID;
	const int iChannelUID = pJoinLobbySyn->ChannelUID;
	const int iCharacterUID = pJoinLobbySyn->CharacterUID;

	// 클라에서 씬 전환 지멋대로 한 경우를 대비해서
	if (player->GetRoomUID() != INVALID_UID) {
		Room* pRoom = _World->GetRoomByPlayer(player);
		if (pRoom) {
			Channel* pChannel = pRoom->GetChannel();
			if (pRoom && pRoom->RemovePlayer(player)) {
				if (pRoom->IsEmpty()) {
					pChannel->RemoveRoom(pRoom->GetRoomUID());
				} else {
					SendFn::BroadcastUpdateRoomUserAck(pRoom, false);
				}
			}
		}
	}

	if (iAccountUID != player->GetAccountUID() ||
		iChannelUID != player->GetChannelUID()) {
		SafeConsole::WriteLine(ConsoleColor::LightGray, "잘못된 유저입니다.");
		player->Disconnect();
		return;
	}

	player->SetCharacterUID(iCharacterUID);
	player->SetPlayerState(PlayerState::Lobby);

	// player에게 자신의 캐릭터 정보 전송
	SendFn::SendUpdateCharacterInfoAck(player);

	// player에게 친구로 등록된 캐릭터 정보들 전송
	SendFn::SendUpdateFriendListAck(player, iCharacterUID);

	// player에게 방 리스트 정보 전송
	SendFn::SendUpdateRoomListAck(player, iChannelUID);
}


// CREATE_ROOM_SYN 120
void CommandFn::CmdCreateRoomSyn(Player* player, ICommand* cmd) {
	const CreateRoomSyn* pCreateRoomSyn = cmd->CastCommand<CreateRoomSyn*>();

	// 속한 채널에서 방을 생성한다.
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());
	Room* pRoom = pChannel->CreateRoom(player, pCreateRoomSyn->RoomName, ROOM_MAX_PLAYER_COUNT);


	// 해당 플레이어는 방으로 이동할 수 있도록 ACK를 전송한다.
	const auto pReplyPacket = new StaticPacket<CreateRoomAck>;
	CreateRoomAck* pCreateRoomAck = pReplyPacket->Get<0>();
	
	if (pRoom == nullptr) {
		pCreateRoomAck->Result = false;
		strcpy_s(pCreateRoomAck->Reason, REASON_LEN, "방 생성에 실패하였습니다.");
	} else {
		pCreateRoomAck->Result = true;
		pCreateRoomAck->RoomUID = pRoom->GetRoomUID();
	}

	player->SendAsync(pReplyPacket);

	// 채널의 로비에 있는 모든 유저들에게 방리스트 목록 업데이트 패킷을 전송한다.
	SendFn::BroadcastUpdateRoomListAck(pChannel);
}


// JOIN_ROOM_SYN 121
void CommandFn::CmdJoinRoomSyn(Player* player, ICommand* cmd) {
	const JoinRoomSyn* pCreateRoomSyn = cmd->CastCommand<JoinRoomSyn*>();

	// 속한 채널에서 방을 생성한다.
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());
	const auto pReplyPacket = new StaticPacket<JoinRoomAck>;
	JoinRoomAck* pJoinRoomAck = pReplyPacket->Get<0>();
	Room* pRoom = pChannel->GetRoomByRoomUID(pCreateRoomSyn->RoomUID);

	pRoom = pChannel->JoinRoom(pCreateRoomSyn->RoomUID, player);

	if (pRoom == nullptr) {
		pJoinRoomAck->Result = false;
		strcpy_s(pJoinRoomAck->Reason, REASON_LEN, "해당 방이 존재하지 않거나 꽉 찼습니다.");
	} else {
		pJoinRoomAck->Result = true;
		pJoinRoomAck->RoomUID = pCreateRoomSyn->RoomUID;


		// 채널의 로비에 있는 모든 유저들에게 방리스트 목록 업데이트 패킷을 전송한다.
		SendFn::BroadcastUpdateRoomListAck(pChannel);
	}
	player->SendAsync(pReplyPacket);
}

// ADD_FRIEND_SYN 122
void CommandFn::CmdAddFriendSyn(Player* player, ICommand* cmd) {
	const AddFriendSyn* pAddFriendSyn = cmd->CastCommand<AddFriendSyn*>();
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	// 플레이어가 속한 채널의 유저들을 확인하면서 친구 요청 패킷을 보낸다.
	Player* pTargetPlayer =pChannel->PlayerFindIf([pAddFriendSyn](Player* playerInChannel)->bool {
		return playerInChannel->CheckNameEqual(pAddFriendSyn->FriendName);
	});



	const auto pReplyPacket = new StaticPacket<AddFriendAck>;
	AddFriendAck* pAddFriendAck = pReplyPacket->Get<0>();
	if (pTargetPlayer == nullptr) {
		pAddFriendAck->Result = false;
		strcpy_s(pAddFriendAck->Reason, REASON_LEN, "해당 플레이어가 현재 채널에 접속중이지 않습니다.");
	} else {
		// 자기 자신인 경우
		if (pTargetPlayer == player) {
			pAddFriendAck->Result = false;
			strcpy_s(pAddFriendAck->Reason, REASON_LEN, "허허.. 자기자신은 불가능하다네");
		} else {
			int iRequestCharacterUID = player->GetCharacterUID();			// 요청자
			int iTargetCharacterUID = pTargetPlayer->GetCharacterUID();		// 수락/거부 예정자

			// 373번줄에서 성공해서 플레이저 정보를 가져왔는데
			// 여기까지 오기전에 플레이어가 나가버리면 INVALID_UID로 초기화 되버리는데.. 예외처리를 너무 빡시게 해야하는데 그러면
			// 이걸 어떻게하면 해결해야 좋을지 모르겠다.
			// 이전에 스키드러쉬 서버 분석했을때 채널 서버, 로비 서버 이렇게 전부 나눠놨었던데 관리가 어려워서 그랬던건가

			if (QueryFn::IsCharacterFriend(iTargetCharacterUID, iRequestCharacterUID)) {
				pAddFriendAck->Result = false;
				strcpy_s(pAddFriendAck->Reason, REASON_LEN, "이미 친구입니다.");
			} else {
				// 친구 요청 대상에게 누군가 친구 요청했음을 알려준다.
				const auto pRequestPacket = new StaticPacket<AddFriendRequestSyn>;
				AddFriendRequestSyn* pAddFriendRequestSyn = pRequestPacket->Get<0>();
				pAddFriendRequestSyn->RequestCharacterUID = iRequestCharacterUID;
				player->LoadCharacterInfo(pAddFriendRequestSyn->Info);
				pTargetPlayer->SendAsync(pRequestPacket);
			}
		}
	}

	// 플레이어에게 요청 보낸 결과를 송신한다.
	player->SendAsync(pReplyPacket);
}

// DELETE_FRIEND_SYN 123
void CommandFn::CmdDeleteFriendSyn(Player* player, ICommand* cmd) {
	const DeleteFriendSyn* pDeleteFriendSyn = cmd->CastCommand<DeleteFriendSyn*>();

	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	Player* pRequest = player;	// 친구 삭제를 요청한 사람
	Player* pDeleted = pChannel->FindPlayerByCharacterUID(pDeleteFriendSyn->DeleteCharacterUID);

	int iRequesterCharacterUID = pRequest->GetCharacterUID();
	int iDeletedCharacterUID = pDeleted->GetCharacterUID();

	const auto pReplyPacket = new StaticPacket<DeleteFriendAck>;
	DeleteFriendAck* pDeleteFriendAck = pReplyPacket->Get<0>();

	if (_Database->Query("delete from t_friendship where (c_req_character_uid = ? and c_ack_character_uid = ?) or (c_ack_character_uid = ? and c_req_character_uid = ?)",
		iRequesterCharacterUID, iDeletedCharacterUID, iRequesterCharacterUID, iDeletedCharacterUID)->IsSuccess()) {
		pDeleteFriendAck->Result = true;
		strcpy_s(pDeleteFriendAck->Reason, REASON_LEN, "성공적으로 친구가 삭제되었습니다.");


		// 삭제 대상이 접속중인 경우 알려준다.
		if (pDeleted) {
			// 누가 널 친구 삭제했다고
			SendFn::SendServerMessageSyn(pDeleted, "누군가 당신을 친구삭제 하였습니다.");

			// 친구 목록도 갱신해준다.
			SendFn::SendUpdateFriendListAck(pDeleted, iDeletedCharacterUID );
		}

		// 삭제를 요청한 사람도 갱신해줌
		SendFn::SendUpdateFriendListAck(pRequest, iRequesterCharacterUID);
	} else {
		pDeleteFriendAck->Result = false;
		strcpy_s(pDeleteFriendAck->Reason, REASON_LEN, "요청을 성공적으로 보냈습니다.");
	}

	player->SendAsync(pReplyPacket);
}

// ADD_FRIEND_REQUEST_ACK 128
void CommandFn::CmdAddFriendRequestAck(Player* player, ICommand* cmd) {
	const AddFriendRequestAck* pAddFriendRequestAck = cmd->CastCommand<AddFriendRequestAck*>();

	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	Player* pAccepter = player;	// 친구 요청을 수락 또는 거부한사람
	Player* pRequester = pChannel->FindPlayerByCharacterUID(pAddFriendRequestAck->RequestCharacterUID);

	// 둘다 접속중이어야 보내야함
	if (pRequester == nullptr || pAccepter == nullptr) {
		// 내부에서 null 체크함

		SendFn::SendServerMessageSyn(pAccepter, "상대방이 접속중이지 않습니다.");
		SendFn::SendServerMessageSyn(pRequester, "상대방이 접속중이지 않습니다.");
	}

	// 수락한 경우
	if (pAddFriendRequestAck->Accept) 
		SendFn::SendServerMessageSyn(pRequester, "상대방이 친구 요청을 수락하였습니다.");
	else
		SendFn::SendServerMessageSyn(pRequester, "상대방이 친구 요청을 거절하였습니다.");

	// 1. 수락한 경우 친구 요청/수락자 모두 친구 목록을 갱신해준다.
	// 2. 데이버베이스에 친구 관계를 추가해준다.
	if (pAddFriendRequestAck->Accept) {
		if (_Database->Query("insert into t_friendship (c_req_character_uid, c_ack_character_uid) values (?, ?)",
			pRequester->GetCharacterUID(), pAccepter->GetCharacterUID())->IsSuccess()) {

			// 한번에 보내는게 효율적이긴 한데 코드 다시 짜야해서 일단.. 이렇게
			SendFn::SendUpdateFriendListAck(pAccepter, pAccepter->GetCharacterUID());
			SendFn::SendUpdateFriendListAck(pRequester, pRequester->GetCharacterUID());
		}
	}
}


// LOAD_ROOM_INFO_SYN 131
void CommandFn::CmdLoadRoomInfoSyn(Player* player, ICommand* cmd) {
	const LoadRoomInfoSyn* pLoadRoomInfoSyn = cmd->CastCommand<LoadRoomInfoSyn*>();

	const int iAccountUID = pLoadRoomInfoSyn->AccountUID;
	const int iChannelUID = pLoadRoomInfoSyn->ChannelUID;
	const int iCharacterUID = pLoadRoomInfoSyn->CharacterUID;
	const int RoomUID = pLoadRoomInfoSyn->RoomUID;

	if (iAccountUID != player->GetAccountUID() ||
		iChannelUID != player->GetChannelUID() ||
		iCharacterUID != player->GetCharacterUID() ||
		RoomUID != player->GetRoomUID()) {
		SafeConsole::WriteLine(ConsoleColor::LightGray, "잘못된 유저입니다.");
		player->Disconnect();
		return;
	}

	Room* pRoom = _World->GetRoomByPlayer(player);

	// 방 정보를 전달
	SendFn::SendRoomInfoAck(pRoom, player);

	// 방에 있는 모든 유저들에게 해당 방에 있는 플레이어 정보들을 전달한다.
	SendFn::BroadcastUpdateRoomUserAck(pRoom, false);

	// player에게 자신의 캐릭터 정보 전송
	SendFn::SendUpdateCharacterInfoAck(player);
}

// ROOM_GAME_START_SYN 133
void CommandFn::CmdRoomGameStartSyn(Player* player, ICommand* cmd) {
	const RoomGameStartSyn* pRoomGameStartSyn = cmd->CastCommand<RoomGameStartSyn*>();

	const int iAccountUID = pRoomGameStartSyn->AccountUID;
	const int iChannelUID = pRoomGameStartSyn->ChannelUID;
	const int iCharacterUID = pRoomGameStartSyn->CharacterUID;
	const int RoomUID = pRoomGameStartSyn->RoomUID;

	if (iAccountUID != player->GetAccountUID() ||
		iChannelUID != player->GetChannelUID() ||
		iCharacterUID != player->GetCharacterUID() ||
		RoomUID != player->GetRoomUID()) {
		SafeConsole::WriteLine(ConsoleColor::LightGray, "잘못된 유저입니다.");
		player->Disconnect();
		return;
	}

	Room* pRoom = _World->GetRoomByPlayer(player);

	const auto pReplyPacket = new StaticPacket<RoomGameStartAck>;
	RoomGameStartAck* pRoomGameStartAck = pReplyPacket->Get<0>();

	if (pRoomGameStartSyn->Intrude) {
		// 난입하여 들어오는 경우
		// 사실 이미 방안에 있고 방 상태만 배틀필드 상태인 것이기땜에 플레이어 상태만 바까주면 됨 ^_^
		if (player->IsBattleState()) {
			pRoomGameStartAck->Result = false;
			strcpy_s(pRoomGameStartAck->Reason, REASON_LEN, "난입에 실패하였습니다.\n이미 당신이 배틀중입니다.(멍미)");
			player->SendAsync(pReplyPacket);
		} else {
			
			pRoomGameStartAck->Result = true;
			player->InitializeRoomBattleIntruderState();
			player->LoadRoomCharacterInfo(pRoomGameStartAck->IntruderInfo);
			pRoom->Broadcast(pReplyPacket);
		}
	} else {
		if (pRoom == nullptr) {
			// 플레이어가 속한 방이 존재하지 않을 경우 
			pRoomGameStartAck->Result = false;
			strcpy_s(pRoomGameStartAck->Reason, REASON_LEN, "당신이 속한 방 정보가 없습니다.");
			player->SendAsync(pReplyPacket);
		}
		else {
			// 방에 있는 모든 유저들에게 게임 시작 패킷을 전송한다.
			if (pRoom->GetChannel()->StartBattle(pRoom)) {
				pRoomGameStartAck->Result = true;
			}
			else {
				pRoomGameStartAck->Result = false;
				strcpy_s(pRoomGameStartAck->Reason, REASON_LEN, "이미 게임이 진행중입니다. 문제가 있네요.");
			}
			pRoom->Broadcast(pReplyPacket);
		}
	}
}

// ROOM_GAME_READY_SYN 134
void CommandFn::CmdRoomGameReadySyn(Player* player, ICommand* cmd) {
	const RoomGameReadySyn* pRoomGameReadySyn = cmd->CastCommand<RoomGameReadySyn*>();

	Room* pRoom = _World->GetRoomByPlayer(player);

	if (pRoom) {
		player->SetReady(true);
		SendFn::BroadcastUpdateRoomUserAck(pRoom, false);
	} else {
		SendFn::SendServerMessageSyn(player, "당신이 속한 방 정보가 없습니다.");
	}
}

// ROOM_GAME_READY_CANCEL_SYN 135
void CommandFn::CmdRoomGameReadyCancelSyn(Player* player, ICommand* cmd) {
	const RoomGameReadyCancelSyn* pRoomGameReadyCancelSyn = cmd->CastCommand<RoomGameReadyCancelSyn*>();

	Room* pRoom = _World->GetRoomByPlayer(player);

	if (pRoom) {
		player->SetReady(false);
		SendFn::BroadcastUpdateRoomUserAck(pRoom, false);
	} else {
		SendFn::SendServerMessageSyn(player, "당신이 속한 방 정보가 없습니다.");
	}
}


// ROOM_LEAVE_SYN 136
void CommandFn::CmdRoomLeaveSyn(Player* player, ICommand* cmd) {
	const RoomLeaveSyn* pRoomLeaveSyn = cmd->CastCommand<RoomLeaveSyn*>();

	Room* pRoom = _World->GetRoomByPlayer(player);
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	const auto pPacket = new StaticPacket<RoomLeaveAck>;
	RoomLeaveAck* pRoomLeaveAck = pPacket->Get<0>();

	if (pRoom && pRoom->RemovePlayer(player)) {
		pRoomLeaveAck->Result = true;

		if (pRoom->IsEmpty()) {
			// 빈 방이 된 경우 없애주자.
			pChannel->RemoveRoom(pRoom->GetRoomUID());
		} else {
			// 기존에 방에 있던 사람에게 나갔다고 알려주자.
			SendFn::BroadcastUpdateRoomUserAck(pRoom, false);
		}

		// 로비에 있는 플레이어들에게 방 정보를 전달해주자.
		SendFn::BroadcastUpdateRoomListAck(pChannel);
	} else {
		pRoomLeaveAck->Result = false;
		strcpy_s(pRoomLeaveAck->Reason, REASON_LEN, "당신이 속한 방 정보가 없습니다.");
	}

	player->SendAsync(pPacket);
}

// BATTLE_FIELD_LOAD_SYN 150
void CommandFn::CmdBattleFieldLoadSyn(Player* player, ICommand* cmd) {
	const BattleFieldLoadSyn* pBattleFieldLoadSyn = cmd->CastCommand<BattleFieldLoadSyn*>();

	// 플레이어가 배틀 필드에 진입하면 레디 상태로 바꾸고 맵에서 랜덤한 위치를 뽑아서 준다.
	Room* pRoom = _World->GetRoomByPlayer(player);
	const auto pReplyPacket = new StaticPacket<BattleFieldLoadAck>;
	BattleFieldLoadAck* pBattleFieldLoadAck = pReplyPacket->Get<0>();

	Random rand;
	TankMove initialMove{};
	pBattleFieldLoadAck->InitialMove.CharacterUID = player->GetCharacterUID();
	pBattleFieldLoadAck->InitialMove.X = (float)rand.GenerateInt(0 + 50, MAP_WIDTH - 50);
	pBattleFieldLoadAck->InitialMove.Y = (float)rand.GenerateInt(0 + 50, MAP_HEIGHT - 50);
	pBattleFieldLoadAck->InitialMove.MoveDir = MoveDirection::None;
	pBattleFieldLoadAck->InitialMove.RotationDir = RotateDirection::None;
	pBattleFieldLoadAck->InitialMove.MoveSpeed = TANK_MOVE_SPEED;
	pBattleFieldLoadAck->InitialMove.Rotation = 0.0f;
	pBattleFieldLoadAck->InitialMove.RotationSpeed = TANK_ROTATION_SPEED;
	pBattleFieldLoadAck->LeftTime = pRoom->GetTimerTime() / 1000.0f;
	pBattleFieldLoadAck->RoomState = pRoom->GetRoomState();

	player->SetReady(true);
	player->UpdateTankMove(pBattleFieldLoadAck->InitialMove);
	player->SendAsync(pReplyPacket);

	// 배틀필드 씬에 유저 정보를 알려준다.
	SendFn::BroadcastUpdateRoomUserAck(pRoom, false);

	// player에게 자신의 캐릭터 정보 전송
	SendFn::SendUpdateCharacterInfoAck(player);
}

void CommandFn::CmdBattileFieldTankMoveSyn(Player* player, ICommand* cmd) {
	BattileFieldTankMoveSyn* pBattileFieldTankMoveSyn = cmd->CastCommand<BattileFieldTankMoveSyn*>();
	player->UpdateTankMove(pBattileFieldTankMoveSyn->Move);
}

void CommandFn::CmdBattleFieldLeaveSyn(Player* player, ICommand* cmd) {
	BattleFieldLeaveSyn* pBattleFieldLeaveSyn = cmd->CastCommand<BattleFieldLeaveSyn*>();

	Room* pRoom = _World->GetRoomByPlayer(player);
	Channel* pChannel = pRoom->GetChannel();

	if (pRoom == nullptr || pChannel == nullptr) {
		DebugAssertMsg(false, "플레이 중인 채널 또는 방이 없습니다.");
		return;
	}

	if (!pChannel->LeaveRoom(player)) {
		DebugAssertMsg(false, "플레이어가 방을 떠나는데 실패했습니다. 방에 플레이어가 없습니다.");
	}

	if (!pRoom->IsEmpty()) {
		const auto pRoomBroadcastPacket = new StaticPacket<BattleFieldLeaveAck>;
		BattleFieldLeaveAck* pBattleFieldLeaveAck = pRoomBroadcastPacket->Get<0>();
		pBattleFieldLeaveAck->CharacterUID = player->GetCharacterUID();
		pRoom->Broadcast(pRoomBroadcastPacket);
	}

	// 배틀필드 방을 나간거니 로비 유저들에게 변경사항을 알려준다.
	SendFn::BroadcastUpdateRoomListAck(pChannel);
}

void CommandFn::CmdChatMessageSyn(Player* player, ICommand* cmd) {
	ChatMessageSyn* pChatMessageSyn = cmd->CastCommand<ChatMessageSyn*>();

	if (player->GetPlayerState() != pChatMessageSyn->PlayerState) {
		SendFn::SendServerMessageSyn(player, "채팅이 가능한 위치가 아닙니다.");
		return;
	}

	const auto pBroadcastPacket = new StaticPacket<ChatMessageAck>;
	ChatMessageAck* pChatMessageAck = pBroadcastPacket->Get<0>();
	strcpy_s(pChatMessageAck->Message, MESSAGE_LEN, pChatMessageSyn->Message);

	// 채팅 가능한 위치 추가될때마다 여기 추가해줄 것
	switch (player->GetPlayerState()) {
	case PlayerState::Lobby: {
		Channel* pChannel = _World->GetChannel(player->GetChannelUID());
		if (pChannel) {
			pChannel->BroadcastLobbyPacket(pBroadcastPacket);
		}
		break;
	}
	case PlayerState::RoomBattle: {
		Room* pRoom = _World->GetRoomByPlayer(player);
		if (pRoom && pRoom->IsBattleFieldState()) {
			pRoom->Broadcast(pBroadcastPacket);
		}
		break;
	}
	default:
		SendFn::SendServerMessageSyn(player, "채팅 전송에 실패했습니다.");
		pBroadcastPacket->Release();
	}
}


void CommandFn::CmdBattleFieldFireSyn(Player* player, ICommand* cmd) {
	BattleFieldFireSyn* pBattleFieldFireSyn = cmd->CastCommand<BattleFieldFireSyn*>();
	player->AddFireCount(1);

	// 총알을 쏘면 해당 방의 유저들에게 브로드캐스팅 해준다.
	Room* pBattleFieldRoom = _World->GetRoomByPlayer(player);
	if (pBattleFieldRoom && pBattleFieldRoom->IsBattleFieldState()) {
		const auto pBroadcastPacket = new StaticPacket<BattleFieldFireAck>;
		BattleFieldFireAck* pBattleFieldFireAck = pBroadcastPacket->Get<0>();
		Memory::CopyUnsafe(&pBattleFieldFireAck->BulletInfo, &pBattleFieldFireSyn->BulletInfo, sizeof(BulletInfo));
		pBattleFieldRoom->Broadcast(pBroadcastPacket);
	}
}

void CommandFn::CmdBattleFieldDeathSyn(Player* player, ICommand* cmd) {
	BattleFieldDeathSyn* pBattleFieldDeathSyn = cmd->CastCommand<BattleFieldDeathSyn*>();

	Channel* pChannel = _World->GetChannel(player->GetChannelUID());
	Room* pRoom = pChannel->GetRoomByPlayer(player);
	// 플레잉 스테이트에만 킬/데스 기록 - EndWait에는 반영안하도록 한다.
	if (pRoom && pRoom->IsBattleFieldState()) {
		Player* pKiller = pChannel->FindPlayerByCharacterUID(pBattleFieldDeathSyn->CharacterUID);
		if (pKiller) {
			pKiller->AddBattleKillCount(1);
			player->AddBattleDeathCount(1);
			player->SetRevivalLeftTime(BATTLE_REVIVAL_TIME);
			player->SetDeath(true);

			// 쿼리 반영은 비동기로 진행
			QueryFn::AddKillCountAsync(pKiller->GetCharacterUID(), 1);
			QueryFn::AddDeathCountAsync(player->GetCharacterUID(), 1);

			const auto pBroadcastPacket = new StaticPacket<BattleFieldDeathAck>;
			BattleFieldDeathAck* pBattleFieldDeathAck = pBroadcastPacket->Get<0>();
			pBattleFieldDeathAck->CharacterUID = player->GetCharacterUID();
			pBattleFieldDeathAck->RevivalLeftTime = BATTLE_REVIVAL_TIME;
			pRoom->Broadcast(pBroadcastPacket);

			SendFn::BroadcastUpdateRoomUserAck(pRoom);
		}
	}
	
}


// TCP_RTT_SYN 200
void CommandFn::CmdTcpRTTSyn(Player* player, JNetwork::ICommand* cmd) {
	const TcpRTTSyn* pTcpRTTSyn = cmd->CastCommand<TcpRTTSyn*>();

	const DateTime clientSentTime(pTcpRTTSyn->Tick);
	const DateTime serverSentTime = DateTime::Now();

	player->SetLatency(serverSentTime.Diff(clientSentTime));
	SendFn::SendRTTAck(player, serverSentTime.GetTick());
}


/*
void CommandFn::CmdTankMoveSyn(Player* session, ICommand* cmd) {
	TcpTankMoveSyn* pTankMoveSyn =  cmd->CastCommand<TcpTankMoveSyn*>();
}

void CommandFn::CmdTankMoveAck(Player* session, ICommand* cmd) {
	TcpTankMoveSyn* pTankMoveSyn = cmd->CastCommand<TcpTankMoveSyn*>();
}





*/
