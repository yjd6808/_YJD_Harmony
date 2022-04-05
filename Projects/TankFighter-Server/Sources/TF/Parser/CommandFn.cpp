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


// LOGIN_SYN 100 Ŀ�ǵ�
void CommandFn::CmdLoginSyn(Player* player, ICommand* cmd) {
	LoginSyn* pLoginSyn = cmd->CastCommand<LoginSyn*>();

	const auto spQuery = _Database->Query("select * from t_account where c_id = ? and c_pass = md5(?)", pLoginSyn->Id, pLoginSyn->Password);
	auto* pLoginAckPacket = new Packet<LoginAck>;
	LoginAck* pLoginAck = pLoginAckPacket->Get<0>();

	if (spQuery->GetResultRowCount()) {
		int iAccountUID = spQuery->Result().GetInt(0, 0);

		pLoginAck->UID = iAccountUID;

		if (_World->IsPlayerExist(iAccountUID)) {
			strcpy_s(pLoginAck->Reason, REASON_LEN, u8"�α��ο� �����Ͽ����ϴ�. �̹� �������Դϴ�.");
			pLoginAck->Result = false;
			goto SEND;
		} 

		pLoginAck->Result = true;

		// �α��� �ð� ������Ʈ ���� - ���⼭ ��� �� �ʿ� �����Ƿ� �ٷ� Release
		_Database->QueryAsync("update t_account set c_last_login_time = ? where c_UID = ?", DateTime::Now(), iAccountUID)->Release();

		// ���� �ش� �÷��̾�� ���忡 ���Ѵ�.
		player->SetAccountUID(iAccountUID);
		player->UpdateLoggedInTime();
		_World->AddNewPlayer(player);
	} else {
		pLoginAck->Result = false;
		strcpy_s(pLoginAck->Reason, REASON_LEN, u8"�α��ο� �����Ͽ����ϴ�. ��ġ�ϴ� ID/PW�� �����ϴ�.");
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

	// �÷��̾� ä�� UID ���� �� ä�ο� �߰�
	if (pSelectedChannel->TryAddPlayer(player)) {
		pSelectChannelAck->Result = true;
		pSelectChannelAck->ChanneldUID = pSelectChannelSyn->ChanneldUID;
		strcpy_s(pSelectChannelAck->Reason, REASON_LEN, u8"ä�� ���� ����");
	} else {
		pSelectChannelAck->Result = false;
		strcpy_s(pSelectChannelAck->Reason, REASON_LEN, u8"�ο��� �� á���ϴ�.");
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
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "�߸��� �����Դϴ�.");
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
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "�߸��� �����Դϴ�.");
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
		strcpy_s(pSelectCharacterAck->Reason, REASON_LEN, u8"�����Ͻ� ĳ������ ������ �������� �ʽ��ϴ�.");
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
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "�߸��� �����Դϴ�.");
		player->Session()->Disconnect();
		return;
	}

	const auto pReplyPacket = new Packet<CreateCharacterAck>;
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
	player->Session()->SendAsync(pReplyPacket);
}


// DELETE_CHARACTER_SYN 112
void CommandFn::CmdDeleteCharacterSyn(Player* player, ICommand* cmd) {
	const DeleteCharacterSyn* pDeleteCharacterSyn = cmd->CastCommand<DeleteCharacterSyn*>();

	const int iAccountUID = pDeleteCharacterSyn->AccountUID;
	const int iChannelUID = pDeleteCharacterSyn->ChannelUID;

	if (iAccountUID != player->GetAccountUID() ||
		iChannelUID != player->GetChannelUID()) {
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "�߸��� �����Դϴ�.");
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
			strcpy_s(pDeleteCharacterAck->Reason, REASON_LEN, u8"ĳ���� ������ �����Ͽ����ϴ�. account_uid, channel_uid mismatch");
			goto SEND;
		}

		pDeleteCharacterAck->Result = true;
	} else {
		strcpy_s(pDeleteCharacterAck->Reason, REASON_LEN, u8"�ش� �г����� ĳ���Ͱ� �������� �ʽ��ϴ�.");
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
		Console::WriteLine(ConsoleColor::LIGHTGRAY, "�߸��� �����Դϴ�.");
		player->Session()->Disconnect();
		return;
	}

	player->SetCharacterUID(iCharacterUID);
	player->SetPlayerState(PlayerState::Lobby);

	// player���� �ڽ��� ĳ���� ���� ����
	SendFn::SendUpdateCharacterInfoAck(player);

	// player���� �� ����Ʈ ���� ����
	SendFn::SendUpdateFriendListAck(player, iCharacterUID);

	// player���� ģ���� ��ϵ� ĳ���� ������ ����
	SendFn::SendUpdateRoomListAck(player, iChannelUID);
}


// CREATE_ROOM_SYN 120
void CommandFn::CmdCreateRoomSyn(Player* player, ICommand* cmd) {
	const CreateRoomSyn* pCreateRoomSyn = cmd->CastCommand<CreateRoomSyn*>();

	// ���� ä�ο��� ���� �����Ѵ�.
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());
	Room* pRoom = pChannel->CreateRoom(player, pCreateRoomSyn->RoomName, ROOM_MAX_PLAYER_COUNT);


	// �ش� �÷��̾�� ������ �̵��� �� �ֵ��� ACK�� �����Ѵ�.
	const auto pReplyPacket = new Packet<CreateRoomAck>;
	CreateRoomAck* pCreateRoomAck = pReplyPacket->Get<0>();
	
	if (pRoom == nullptr) {
		pCreateRoomAck->Result = false;
		strcpy_s(pCreateRoomAck->Reason, REASON_LEN, u8"�� ������ �����Ͽ����ϴ�.");
	} else {
		pCreateRoomAck->Result = true;
		pCreateRoomAck->RoomUID = pRoom->GetRoomUID();
	}

	player->Session()->SendAsync(pReplyPacket);

	// ä���� �κ� �ִ� ��� �����鿡�� �渮��Ʈ ��� ������Ʈ ��Ŷ�� �����Ѵ�.
	SendFn::BroadcastUpdateRoomListAck(pChannel);
}


// JOIN_ROOM_SYN 121
void CommandFn::CmdJoinRoomSyn(Player* player, ICommand* cmd) {
	const JoinRoomSyn* pCreateRoomSyn = cmd->CastCommand<JoinRoomSyn*>();

	// ���� ä�ο��� ���� �����Ѵ�.
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());
	const auto pReplyPacket = new Packet<JoinRoomAck>;
	JoinRoomAck* pJoinRoomAck = pReplyPacket->Get<0>();
	Room* pRoom = pChannel->JoinRoom(pCreateRoomSyn->RoomUID, player);

	if (pRoom == nullptr) {
		pJoinRoomAck->Result = false;
		strcpy_s(pJoinRoomAck->Reason, REASON_LEN, u8"�ش� ���� �������� �ʰų� �� á���ϴ�.");
	} else {
		pJoinRoomAck->Result = true;
		pJoinRoomAck->RoomUID = pCreateRoomSyn->RoomUID;
	}
	player->Session()->SendAsync(pReplyPacket);

	// ä���� �κ� �ִ� ��� �����鿡�� �渮��Ʈ ��� ������Ʈ ��Ŷ�� �����Ѵ�.
	SendFn::BroadcastUpdateRoomListAck(pChannel);

	// �濡 �ִ� ��� �����鿡�� �ش� �濡 �ִ� �÷��̾� �������� �����Ѵ�.
	SendFn::BroadcastUpdateRoomUserAck(pRoom);
}

// ADD_FRIEND_SYN 122
void CommandFn::CmdAddFriendSyn(Player* player, ICommand* cmd) {
	const AddFriendSyn* pAddFriendSyn = cmd->CastCommand<AddFriendSyn*>();
	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	// �÷��̾ ���� ä���� �������� Ȯ���ϸ鼭 ģ�� ��û ��Ŷ�� ������.
	Player* pFind =pChannel->PlayerFindIf([pAddFriendSyn](Player* playerInChannel)->bool {
		return playerInChannel->CheckNameEqual(pAddFriendSyn->FriendName);
	});

	const auto pReplyPacket = new Packet<AddFriendAck>;
	AddFriendAck* pAddFriendAck = pReplyPacket->Get<0>();
	if (pFind == nullptr) {
		pAddFriendAck->Result = false;
		strcpy_s(pAddFriendAck->Reason, REASON_LEN, u8"�ش� �÷��̾ ���� ä�ο� ���������� �ʽ��ϴ�.");
	} else {
		pAddFriendAck->Result = true;
		strcpy_s(pAddFriendAck->Reason, REASON_LEN, u8"��û�� ���������� ���½��ϴ�.");

		// ģ�� ��û ��󿡰� ������ ģ�� ��û������ �˷��ش�.
		const auto pRequestPacket = new Packet<AddFriendRequestSyn>;
		AddFriendRequestSyn* pAddFriendRequestSyn = pRequestPacket->Get<0>();
		pAddFriendRequestSyn->RequestCharacterUID = player->GetCharacterUID();
		pFind->Session()->SendAsync(pRequestPacket);
	}

	// �÷��̾�� ��û ���� ����� �۽��Ѵ�.
	player->Session()->SendAsync(pReplyPacket);
}

// DELETE_FRIEND_SYN 123
void CommandFn::CmdDeleteFriendSyn(Player* player, ICommand* cmd) {
	const DeleteFriendSyn* pDeleteFriendSyn = cmd->CastCommand<DeleteFriendSyn*>();

	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	Player* pRequest = player;	// ģ�� ������ ��û�� ���
	Player* pDeleted = pChannel->FindPlayerByCharacterUID(pDeleteFriendSyn->DeleteCharacterUID);

	int iRequesterCharacterUID = pRequest->GetCharacterUID();
	int iDeletedCharacterUID = pDeleted->GetCharacterUID();

	const auto pReplyPacket = new Packet<DeleteFriendAck>;
	DeleteFriendAck* pDeleteFriendAck = pReplyPacket->Get<0>();

	if (_Database->Query("delete from t_friendship where (c_req_character_uid = ? and c_ack_character_uid = ?) or (c_ack_character_uid = ? and c_req_character_uid = ?)",
		iRequesterCharacterUID, iDeletedCharacterUID, iDeletedCharacterUID, iRequesterCharacterUID)->IsSuccess()) {
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

	player->Session()->SendAsync(pReplyPacket);
}

// ADD_FRIEND_REQUEST_ACK 128
void CommandFn::CmdFriendRequestAck(Player* player, ICommand* cmd) {
	const AddFriendRequestAck* pAddFriendRequestAck = cmd->CastCommand<AddFriendRequestAck*>();

	Channel* pChannel = _World->GetChannel(player->GetChannelUID());

	Player* pAccepter = player;	// ģ�� ��û�� ���� �Ǵ� �ź��ѻ��
	Player* pRequester = pChannel->FindPlayerByCharacterUID(pAddFriendRequestAck->RequestCharacterUID);
	 
	// ��û ���� ����� ���� �������̰�
	if (pRequester) {
		const auto pRequestPacket = new Packet<AddFriendRequestResultSyn>;
		AddFriendRequestResultSyn* pAddFriendRequestResultSyn = pRequestPacket->Get<0>();

		// ������ ���
		if (pAddFriendRequestAck->Accept) {
			pAddFriendRequestResultSyn->Result = true;
			strcpy_s(pAddFriendRequestResultSyn->Reason, REASON_LEN, u8"������ ģ�� ��û�� �����Ͽ����ϴ�.");
		} else { // ������ ���
			pAddFriendRequestResultSyn->Result = false;
			strcpy_s(pAddFriendRequestResultSyn->Reason, REASON_LEN, u8"������ ģ�� ��û�� �����Ͽ����ϴ�.");
		}

		pRequester->Session()->SendAsync(pRequestPacket);
	}

	// 1. ������ ��� ģ�� ��û/������ ��� ģ�� ����� �������ش�.
	// 2. ���̹����̽��� ģ�� ���踦 �߰����ش�.
	if (pAddFriendRequestAck->Accept) {
		_Database->QueryAsync("insert into t_friendship (c_req_character_uid, c_ack_character_uid) values (?, ?)", 
			pRequester->GetCharacterUID(), pAccepter->GetCharacterUID())->Release();

		// �ѹ��� �����°� ȿ�����̱� �ѵ� �ڵ� �ٽ� ¥���ؼ� �ϴ�.. �̷���
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




