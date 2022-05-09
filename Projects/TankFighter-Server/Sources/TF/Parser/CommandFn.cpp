/*
 * �ۼ��� : ������
 *
 * 1. ������ ��Ŷ�� ���� ó���� �ϵ����մϴ�.
 * 2. ��Ŷ ��� �ڵ�� ��� ���⸸ �ۼ��մϴ�.
 * 3. ��, ���������� BattleFieldWorkder.cpp ������ ��Ŷ �۽ſ� ���� ó���� �մϴ�.
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


// LOGIN_SYN 100 Ŀ�ǵ�
void CommandFn::CmdLoginSyn(Player* player, ICommand* cmd) {
	LoginSyn* pLoginSyn = cmd->CastCommand<LoginSyn*>();

	const auto spQuery = _Database->Query("select * from t_account where c_id = ? and c_pass = md5(?)", pLoginSyn->Id, pLoginSyn->Password);
	auto* pLoginAckPacket = new StaticPacket<LoginAck>;
	LoginAck* pLoginAck = pLoginAckPacket->Get<0>();

	if (spQuery->GetResultRowCount()) {
		int iAccountUID = spQuery->GetInt(0, 0);

		pLoginAck->UID = iAccountUID;

		if (_World->IsPlayerExist(iAccountUID)) {
			strcpy_s(pLoginAck->Reason, REASON_LEN, u8"�α��ο� �����Ͽ����ϴ�. �̹� �������Դϴ�.");
			pLoginAck->Result = false;
			goto SEND;
		} 

		pLoginAck->Result = true;

		// �α��� �ð� ������Ʈ ���� - ���⼭ ��� �� �ʿ� �����Ƿ� �ٷ� Release
		_Database->QueryAsync("update t_account set c_last_login_time = ? where c_uid = ?", DateTime::Now(), iAccountUID)->Release();

		// ���� �ش� �÷��̾�� ���忡 ���Ѵ�.
		player->SetAccountUID(iAccountUID);
		player->UpdateLoggedInTime();
		_World->AddNewPlayer(player);
	} else {
		pLoginAck->Result = false;
		strcpy_s(pLoginAck->Reason, REASON_LEN, u8"�α��ο� �����Ͽ����ϴ�. ��ġ�ϴ� ID/PW�� �����ϴ�.");
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
		strcpy_s(pRegisterAck->Reason, REASON_LEN, u8"�̹� �����ϴ� ID�Դϴ�.");
	} else {
		auto spIdCheckQuery = _Database->Query("insert into t_account (c_id, c_pass) values (?, md5(?))", pRegisterSyn->Id, pRegisterSyn->Password);

		if (spIdCheckQuery->IsSuccess()) {
			pRegisterAck->Result = true;
			strcpy_s(pRegisterAck->Reason, REASON_LEN, u8"ȸ�����Կ� �����Ͽ����ϴ�.");
		} else {
			pRegisterAck->Result = true;
			strcpy_s(pRegisterAck->Reason, REASON_LEN, u8"ȸ�����Կ� �����Ͽ����ϴ�. (insert ����)");
		}
	} 

	player->SendAsync(pRegisterAckPacket);
}


// LOAD_CHANNEL_INFO_SYN 104
void CommandFn::CmdLoadChannelInfoSyn(Player* player, ICommand* cmd) {
	const LoadChannelInfoSyn* pRegisterSyn = cmd->CastCommand<LoadChannelInfoSyn*>();

	const Vector<Channel*> channels = _World->GetChannels();

	// Ŭ�� �κ񿡼� ä�μ������� �̵��� ���
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

	// �÷��̾� ä�� UID ���� �� ä�ο� �߰�
	if (pSelectedChannel->TryAddPlayer(player)) {
		pSelectChannelAck->Result = true;
		pSelectChannelAck->ChanneldUID = pSelectChannelSyn->ChanneldUID;
		strcpy_s(pSelectChannelAck->Reason, REASON_LEN, u8"ä�� ���� ����");
	} else {
		pSelectChannelAck->Result = false;
		strcpy_s(pSelectChannelAck->Reason, REASON_LEN, u8"�ο��� �� á���ϴ�.");
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
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "�߸��� �����Դϴ�.");
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
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "�߸��� �����Դϴ�.");
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
		strcpy_s(pSelectCharacterAck->Reason, REASON_LEN, u8"�����Ͻ� ĳ������ ������ �������� �ʽ��ϴ�.");
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
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "�߸��� �����Դϴ�.");
		player->Disconnect();
		return;
	}

	const auto pReplyPacket = new StaticPacket<CreateCharacterAck>;
	CreateCharacterAck* pCreateCharacterAck = pReplyPacket->Get<0>();
	const String szCharacterName = pCreateCharacterSyn->CharacterName;

	pCreateCharacterAck->Result = false;
	if (QueryFn::IsCharacterExistByName(szCharacterName)) {
		strcpy_s(pCreateCharacterAck->Reason, REASON_LEN, u8"�̹� �ش� ĳ���� ���� �̹� �����մϴ�.");
	} else {

		// �Ϻη� ����� ��������
		if (_Database->Query("insert into t_character (c_account_uid, c_channel_uid, c_name, c_win, c_lose, c_kill, c_death, c_money) values (?, ?, ?, ?, ?, ?, ?, ?)",
			iAccountUID, iChannelUID, szCharacterName, 0, 0, 0, 0, 1000000)->IsFailed()) {
			strcpy_s(pCreateCharacterAck->Reason, REASON_LEN, u8"ĳ���� ������ �����߽��ϴ�. (select failed)");
			goto SEND;
		}

		pCreateCharacterAck->Result = true;
		strcpy_s(pCreateCharacterAck->Reason, REASON_LEN, u8"ĳ���Ͱ� ���������� �����Ǿ����ϴ�!");
		// ĳ���� �����Ǹ� ĳ������� ������������.
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
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "�߸��� �����Դϴ�.");
		player->Disconnect();
		return;
	}

	const auto pReplyPacket = new StaticPacket<DeleteCharacterAck>;
	DeleteCharacterAck* pDeleteCharacterAck = pReplyPacket->Get<0>();

	const String szCharacterName = pDeleteCharacterSyn->CharacterName;
	if (QueryFn::IsCharacterExistByName(szCharacterName)) {
		if (_Database->Query("delete from t_character where c_name = ? and c_account_uid = ? and c_channel_uid = ?",
			szCharacterName, iAccountUID, iChannelUID)->IsFailed()) {
			strcpy_s(pDeleteCharacterAck->Reason, REASON_LEN, u8"ĳ���� ������ �����Ͽ����ϴ�. account_uid, channel_uid mismatch");
		} else {
			strcpy_s(pDeleteCharacterAck->Reason, REASON_LEN, u8"ĳ���Ͱ� ���������� �����Ǿ����ϴ�.");

			// ĳ���Ͱ� �����Ǹ� ĳ������� ������������.
			SendFn::SendLoadCharacterInfoAck(player);
		}
	} else {
		strcpy_s(pDeleteCharacterAck->Reason, REASON_LEN, u8"�ش� �г����� ĳ���Ͱ� �������� �ʽ��ϴ�.");
	}

	player->SendAsync(pReplyPacket);
}

// JOIN_LOBBY_SYN 116
void CommandFn::CmdJoinLobbySyn(Player* player, ICommand* cmd) {
	const JoinLobbySyn* pJoinLobbySyn = cmd->CastCommand<JoinLobbySyn*>();

	const int iAccountUID = pJoinLobbySyn->AccountUID;
	const int iChannelUID = pJoinLobbySyn->ChannelUID;
	const int iCharacterUID = pJoinLobbySyn->CharacterUID;

	// Ŭ�󿡼� �� ��ȯ ���ڴ�� �� ��츦 ����ؼ�
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
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "�߸��� �����Դϴ�.");
		player->Disconnect();
		return;
	}

	player->SetCharacterUID(iCharacterUID);
	player->SetPlayerState(PlayerState::Lobby);

	// player���� �ڽ��� ĳ���� ���� ����
	SendFn::SendUpdateCharacterInfoAck(player);

	// player���� ģ���� ��ϵ� ĳ���� ������ ����
	SendFn::SendUpdateFriendListAck(player, iCharacterUID);

	// player���� �� ����Ʈ ���� ����
	SendFn::SendUpdateRoomListAck(player, iChannelUID);
}


// CREATE_ROOM_SYN 120
void CommandFn::CmdCreateRoomSyn(Player* player, ICommand* cmd) {
	const CreateRoomSyn* pCreateRoomSyn = cmd->CastCommand<CreateRoomSyn*>();

	// ���� ä�ο��� ���� �����Ѵ�.
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());
	Room* pRoom = pChannel->CreateRoom(player, pCreateRoomSyn->RoomName, ROOM_MAX_PLAYER_COUNT);


	// �ش� �÷��̾�� ������ �̵��� �� �ֵ��� ACK�� �����Ѵ�.
	const auto pReplyPacket = new StaticPacket<CreateRoomAck>;
	CreateRoomAck* pCreateRoomAck = pReplyPacket->Get<0>();
	
	if (pRoom == nullptr) {
		pCreateRoomAck->Result = false;
		strcpy_s(pCreateRoomAck->Reason, REASON_LEN, u8"�� ������ �����Ͽ����ϴ�.");
	} else {
		pCreateRoomAck->Result = true;
		pCreateRoomAck->RoomUID = pRoom->GetRoomUID();
	}

	player->SendAsync(pReplyPacket);

	// ä���� �κ� �ִ� ��� �����鿡�� �渮��Ʈ ��� ������Ʈ ��Ŷ�� �����Ѵ�.
	SendFn::BroadcastUpdateRoomListAck(pChannel);
}


// JOIN_ROOM_SYN 121
void CommandFn::CmdJoinRoomSyn(Player* player, ICommand* cmd) {
	const JoinRoomSyn* pCreateRoomSyn = cmd->CastCommand<JoinRoomSyn*>();

	// ���� ä�ο��� ���� �����Ѵ�.
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());
	const auto pReplyPacket = new StaticPacket<JoinRoomAck>;
	JoinRoomAck* pJoinRoomAck = pReplyPacket->Get<0>();
	Room* pRoom = pChannel->GetRoomByRoomUID(pCreateRoomSyn->RoomUID);

	pRoom = pChannel->JoinRoom(pCreateRoomSyn->RoomUID, player);

	if (pRoom == nullptr) {
		pJoinRoomAck->Result = false;
		strcpy_s(pJoinRoomAck->Reason, REASON_LEN, u8"�ش� ���� �������� �ʰų� �� á���ϴ�.");
	} else {
		pJoinRoomAck->Result = true;
		pJoinRoomAck->RoomUID = pCreateRoomSyn->RoomUID;


		// ä���� �κ� �ִ� ��� �����鿡�� �渮��Ʈ ��� ������Ʈ ��Ŷ�� �����Ѵ�.
		SendFn::BroadcastUpdateRoomListAck(pChannel);
	}
	player->SendAsync(pReplyPacket);
}

// ADD_FRIEND_SYN 122
void CommandFn::CmdAddFriendSyn(Player* player, ICommand* cmd) {
	const AddFriendSyn* pAddFriendSyn = cmd->CastCommand<AddFriendSyn*>();
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	// �÷��̾ ���� ä���� �������� Ȯ���ϸ鼭 ģ�� ��û ��Ŷ�� ������.
	Player* pTargetPlayer =pChannel->PlayerFindIf([pAddFriendSyn](Player* playerInChannel)->bool {
		return playerInChannel->CheckNameEqual(pAddFriendSyn->FriendName);
	});



	const auto pReplyPacket = new StaticPacket<AddFriendAck>;
	AddFriendAck* pAddFriendAck = pReplyPacket->Get<0>();
	if (pTargetPlayer == nullptr) {
		pAddFriendAck->Result = false;
		strcpy_s(pAddFriendAck->Reason, REASON_LEN, u8"�ش� �÷��̾ ���� ä�ο� ���������� �ʽ��ϴ�.");
	} else {
		// �ڱ� �ڽ��� ���
		if (pTargetPlayer == player) {
			pAddFriendAck->Result = false;
			strcpy_s(pAddFriendAck->Reason, REASON_LEN, u8"����.. �ڱ��ڽ��� �Ұ����ϴٳ�");
		} else {
			int iRequestCharacterUID = player->GetCharacterUID();			// ��û��
			int iTargetCharacterUID = pTargetPlayer->GetCharacterUID();		// ����/�ź� ������

			// 373���ٿ��� �����ؼ� �÷����� ������ �����Դµ�
			// ������� �������� �÷��̾ ���������� INVALID_UID�� �ʱ�ȭ �ǹ����µ�.. ����ó���� �ʹ� ���ð� �ؾ��ϴµ� �׷���
			// �̰� ����ϸ� �ذ��ؾ� ������ �𸣰ڴ�.
			// ������ ��Ű�巯�� ���� �м������� ä�� ����, �κ� ���� �̷��� ���� ������������ ������ ������� �׷����ǰ�

			if (QueryFn::IsCharacterFriend(iTargetCharacterUID, iRequestCharacterUID)) {
				pAddFriendAck->Result = false;
				strcpy_s(pAddFriendAck->Reason, REASON_LEN, u8"�̹� ģ���Դϴ�.");
			} else {
				// ģ�� ��û ��󿡰� ������ ģ�� ��û������ �˷��ش�.
				const auto pRequestPacket = new StaticPacket<AddFriendRequestSyn>;
				AddFriendRequestSyn* pAddFriendRequestSyn = pRequestPacket->Get<0>();
				pAddFriendRequestSyn->RequestCharacterUID = iRequestCharacterUID;
				player->LoadCharacterInfo(pAddFriendRequestSyn->Info);
				pTargetPlayer->SendAsync(pRequestPacket);
			}
		}
	}

	// �÷��̾�� ��û ���� ����� �۽��Ѵ�.
	player->SendAsync(pReplyPacket);
}

// DELETE_FRIEND_SYN 123
void CommandFn::CmdDeleteFriendSyn(Player* player, ICommand* cmd) {
	const DeleteFriendSyn* pDeleteFriendSyn = cmd->CastCommand<DeleteFriendSyn*>();

	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	Player* pRequest = player;	// ģ�� ������ ��û�� ���
	Player* pDeleted = pChannel->FindPlayerByCharacterUID(pDeleteFriendSyn->DeleteCharacterUID);

	int iRequesterCharacterUID = pRequest->GetCharacterUID();
	int iDeletedCharacterUID = pDeleted->GetCharacterUID();

	const auto pReplyPacket = new StaticPacket<DeleteFriendAck>;
	DeleteFriendAck* pDeleteFriendAck = pReplyPacket->Get<0>();

	if (_Database->Query("delete from t_friendship where (c_req_character_uid = ? and c_ack_character_uid = ?) or (c_ack_character_uid = ? and c_req_character_uid = ?)",
		iRequesterCharacterUID, iDeletedCharacterUID, iRequesterCharacterUID, iDeletedCharacterUID)->IsSuccess()) {
		pDeleteFriendAck->Result = true;
		strcpy_s(pDeleteFriendAck->Reason, REASON_LEN, u8"���������� ģ���� �����Ǿ����ϴ�.");


		// ���� ����� �������� ��� �˷��ش�.
		if (pDeleted) {
			// ���� �� ģ�� �����ߴٰ�
			SendFn::SendServerMessageSyn(pDeleted, u8"������ ����� ģ������ �Ͽ����ϴ�.");

			// ģ�� ��ϵ� �������ش�.
			SendFn::SendUpdateFriendListAck(pDeleted, iDeletedCharacterUID );
		}

		// ������ ��û�� ����� ��������
		SendFn::SendUpdateFriendListAck(pRequest, iRequesterCharacterUID);
	} else {
		pDeleteFriendAck->Result = false;
		strcpy_s(pDeleteFriendAck->Reason, REASON_LEN, u8"��û�� ���������� ���½��ϴ�.");
	}

	player->SendAsync(pReplyPacket);
}

// ADD_FRIEND_REQUEST_ACK 128
void CommandFn::CmdAddFriendRequestAck(Player* player, ICommand* cmd) {
	const AddFriendRequestAck* pAddFriendRequestAck = cmd->CastCommand<AddFriendRequestAck*>();

	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	Player* pAccepter = player;	// ģ�� ��û�� ���� �Ǵ� �ź��ѻ��
	Player* pRequester = pChannel->FindPlayerByCharacterUID(pAddFriendRequestAck->RequestCharacterUID);

	// �Ѵ� �������̾�� ��������
	if (pRequester == nullptr || pAccepter == nullptr) {
		// ���ο��� null üũ��

		SendFn::SendServerMessageSyn(pAccepter, u8"������ ���������� �ʽ��ϴ�.");
		SendFn::SendServerMessageSyn(pRequester, u8"������ ���������� �ʽ��ϴ�.");
	}

	// ������ ���
	if (pAddFriendRequestAck->Accept) 
		SendFn::SendServerMessageSyn(pRequester, u8"������ ģ�� ��û�� �����Ͽ����ϴ�.");
	else
		SendFn::SendServerMessageSyn(pRequester, u8"������ ģ�� ��û�� �����Ͽ����ϴ�.");

	// 1. ������ ��� ģ�� ��û/������ ��� ģ�� ����� �������ش�.
	// 2. ���̹����̽��� ģ�� ���踦 �߰����ش�.
	if (pAddFriendRequestAck->Accept) {
		if (_Database->Query("insert into t_friendship (c_req_character_uid, c_ack_character_uid) values (?, ?)",
			pRequester->GetCharacterUID(), pAccepter->GetCharacterUID())->IsSuccess()) {

			// �ѹ��� �����°� ȿ�����̱� �ѵ� �ڵ� �ٽ� ¥���ؼ� �ϴ�.. �̷���
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
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "�߸��� �����Դϴ�.");
		player->Disconnect();
		return;
	}

	Room* pRoom = _World->GetRoomByPlayer(player);

	// �� ������ ����
	SendFn::SendRoomInfoAck(pRoom, player);

	// �濡 �ִ� ��� �����鿡�� �ش� �濡 �ִ� �÷��̾� �������� �����Ѵ�.
	SendFn::BroadcastUpdateRoomUserAck(pRoom, false);

	// player���� �ڽ��� ĳ���� ���� ����
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
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "�߸��� �����Դϴ�.");
		player->Disconnect();
		return;
	}

	Room* pRoom = _World->GetRoomByPlayer(player);

	const auto pReplyPacket = new StaticPacket<RoomGameStartAck>;
	RoomGameStartAck* pRoomGameStartAck = pReplyPacket->Get<0>();

	if (pRoomGameStartSyn->Intrude) {
		// �����Ͽ� ������ ���
		// ��� �̹� ��ȿ� �ְ� �� ���¸� ��Ʋ�ʵ� ������ ���̱ⶫ�� �÷��̾� ���¸� �ٱ��ָ� �� ^_^
		if (player->IsBattleState()) {
			pRoomGameStartAck->Result = false;
			strcpy_s(pRoomGameStartAck->Reason, REASON_LEN, u8"���Կ� �����Ͽ����ϴ�.\n�̹� ����� ��Ʋ���Դϴ�.(�۹�)");
			player->SendAsync(pReplyPacket);
		} else {
			
			pRoomGameStartAck->Result = true;
			player->InitializeRoomBattleIntruderState();
			player->LoadRoomCharacterInfo(pRoomGameStartAck->IntruderInfo);
			pRoom->Broadcast(pReplyPacket);
		}
	} else {
		if (pRoom == nullptr) {
			// �÷��̾ ���� ���� �������� ���� ��� 
			pRoomGameStartAck->Result = false;
			strcpy_s(pRoomGameStartAck->Reason, REASON_LEN, u8"����� ���� �� ������ �����ϴ�.");
			player->SendAsync(pReplyPacket);
		}
		else {
			// �濡 �ִ� ��� �����鿡�� ���� ���� ��Ŷ�� �����Ѵ�.
			if (pRoom->GetChannel()->StartBattle(pRoom)) {
				pRoomGameStartAck->Result = true;
			}
			else {
				pRoomGameStartAck->Result = false;
				strcpy_s(pRoomGameStartAck->Reason, REASON_LEN, u8"�̹� ������ �������Դϴ�. ������ �ֳ׿�.");
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
		SendFn::SendServerMessageSyn(player, u8"����� ���� �� ������ �����ϴ�.");
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
		SendFn::SendServerMessageSyn(player, u8"����� ���� �� ������ �����ϴ�.");
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
			// �� ���� �� ��� ��������.
			pChannel->RemoveRoom(pRoom->GetRoomUID());
		} else {
			// ������ �濡 �ִ� ������� �����ٰ� �˷�����.
			SendFn::BroadcastUpdateRoomUserAck(pRoom, false);
		}

		// �κ� �ִ� �÷��̾�鿡�� �� ������ ����������.
		SendFn::BroadcastUpdateRoomListAck(pChannel);
	} else {
		pRoomLeaveAck->Result = false;
		strcpy_s(pRoomLeaveAck->Reason, REASON_LEN, u8"����� ���� �� ������ �����ϴ�.");
	}

	player->SendAsync(pPacket);
}

// BATTLE_FIELD_LOAD_SYN 150
void CommandFn::CmdBattleFieldLoadSyn(Player* player, ICommand* cmd) {
	const BattleFieldLoadSyn* pBattleFieldLoadSyn = cmd->CastCommand<BattleFieldLoadSyn*>();

	// �÷��̾ ��Ʋ �ʵ忡 �����ϸ� ���� ���·� �ٲٰ� �ʿ��� ������ ��ġ�� �̾Ƽ� �ش�.
	Room* pRoom = _World->GetRoomByPlayer(player);
	const auto pReplyPacket = new StaticPacket<BattleFieldLoadAck>;
	BattleFieldLoadAck* pBattleFieldLoadAck = pReplyPacket->Get<0>();

	Random rand;
	TankMove initialMove{};
	pBattleFieldLoadAck->InitialMove.CharacterUID = player->GetCharacterUID();
	pBattleFieldLoadAck->InitialMove.X = rand.GenerateInt(0 + 50, MAP_WIDTH - 50);
	pBattleFieldLoadAck->InitialMove.Y = rand.GenerateInt(0 + 50, MAP_HEIGHT - 50);
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

	// ��Ʋ�ʵ� ���� ���� ������ �˷��ش�.
	SendFn::BroadcastUpdateRoomUserAck(pRoom, false);

	// player���� �ڽ��� ĳ���� ���� ����
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
		DebugAssert(false, "�÷��� ���� ä�� �Ǵ� ���� �����ϴ�.");
		return;
	}

	if (!pChannel->LeaveRoom(player)) {
		DebugAssert(false, "�÷��̾ ���� �����µ� �����߽��ϴ�. �濡 �÷��̾ �����ϴ�.");
	}

	if (!pRoom->IsEmpty()) {
		const auto pRoomBroadcastPacket = new StaticPacket<BattleFieldLeaveAck>;
		BattleFieldLeaveAck* pBattleFieldLeaveAck = pRoomBroadcastPacket->Get<0>();
		pBattleFieldLeaveAck->CharacterUID = player->GetCharacterUID();
		pRoom->Broadcast(pRoomBroadcastPacket);
	}

	// ��Ʋ�ʵ� ���� �����Ŵ� �κ� �����鿡�� ��������� �˷��ش�.
	SendFn::BroadcastUpdateRoomListAck(pChannel);
}

void CommandFn::CmdChatMessageSyn(Player* player, ICommand* cmd) {
	ChatMessageSyn* pChatMessageSyn = cmd->CastCommand<ChatMessageSyn*>();

	if (player->GetPlayerState() != pChatMessageSyn->PlayerState) {
		SendFn::SendServerMessageSyn(player, u8"ä���� ������ ��ġ�� �ƴմϴ�.");
		return;
	}

	const auto pBroadcastPacket = new StaticPacket<ChatMessageAck>;
	ChatMessageAck* pChatMessageAck = pBroadcastPacket->Get<0>();
	strcpy_s(pChatMessageAck->Message, MESSAGE_LEN, pChatMessageSyn->Message);

	// ä�� ������ ��ġ �߰��ɶ����� ���� �߰����� ��
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
		SendFn::SendServerMessageSyn(player, u8"ä�� ���ۿ� �����߽��ϴ�.");
		pBroadcastPacket->Release();
	}
}


void CommandFn::CmdBattleFieldFireSyn(Player* player, ICommand* cmd) {
	BattleFieldFireSyn* pBattleFieldFireSyn = cmd->CastCommand<BattleFieldFireSyn*>();
	player->AddFireCount(1);

	// �Ѿ��� ��� �ش� ���� �����鿡�� ��ε�ĳ���� ���ش�.
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
	// �÷��� ������Ʈ���� ų/���� ��� - EndWait���� �ݿ����ϵ��� �Ѵ�.
	if (pRoom && pRoom->IsBattleFieldState()) {
		Player* pKiller = pChannel->FindPlayerByCharacterUID(pBattleFieldDeathSyn->CharacterUID);
		if (pKiller) {
			pKiller->AddBattleKillCount(1);
			player->AddBattleDeathCount(1);
			player->SetRevivalLeftTime(BATTLE_REVIVAL_TIME);
			player->SetDeath(true);

			// ���� �ݿ��� �񵿱�� ����
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
