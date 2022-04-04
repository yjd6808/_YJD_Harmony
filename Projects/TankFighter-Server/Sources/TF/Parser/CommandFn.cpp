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



void CommandFn::CmdLoginSyn(Player* player, ICommand* cmd) {
	LoginSyn* pLoginSyn = cmd->CastCommand<LoginSyn*>();

	const auto spQuery = _Database->Query("select * from t_account where c_id = ? and c_pass = md5(?)", pLoginSyn->Id, pLoginSyn->Password);
	auto* pLoginAckPacket = new Packet<LoginAck>;
	LoginAck* pLoginAck = pLoginAckPacket->Get<0>();

	if (spQuery->GetResultRowCount()) {
		int iAccountUID = spQuery->Result().GetInt(1, 0);

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

void CommandFn::CmdLoadChannelInfoSyn(Player* player, ICommand* cmd) {
	const LoadChannelInfoSyn* pRegisterSyn = cmd->CastCommand<LoadChannelInfoSyn*>();

	const Vector<Channel*> channels = _World->GetChannels();

	const auto pReplyPacket = new Packet<LoadChannelInfoAck>();
	LoadChannelInfoAck* pLoadChannelInfoAck = pReplyPacket->Get<0>();
	pLoadChannelInfoAck->Count = channels.Size();

	for (int i = 0; i < channels.Size(); i++) {
		strcpy_s(pLoadChannelInfoAck->Info[i].Name, NAME_LEN, channels[i]->GetChnnelName().Source());
		pLoadChannelInfoAck->Info[i].UID = channels[i]->GetChannelUID();
		pLoadChannelInfoAck->Info[i].MaxPlayerCount = channels[i]->GetMaxPlayerCount();
		pLoadChannelInfoAck->Info[i].PlayerCount = channels[i]->GetPlayerCount();
	}

	player->Session()->SendAsync(pReplyPacket);
}

void CommandFn::CmdSelectChannelSyn(Player* player, ICommand* cmd) {
	const SelectChannelSyn* pSelectChannelSyn = cmd->CastCommand<SelectChannelSyn*>();
	const auto pReplyPacket = new Packet<SelectChannelAck>();
	SelectChannelAck* pSelectChannelAck = pReplyPacket->Get<0>();

	Channel* pSelectedChannel =  _World->GetChannel(pSelectChannelSyn->ChanneldUID);
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

	SendFn::SendLoadCharacterInfoAck(player);
}

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


void CommandFn::CmdTankMoveSyn(Player* session, ICommand* cmd) {
	TcpTankMoveSyn* pTankMoveSyn =  cmd->CastCommand<TcpTankMoveSyn*>();
}

void CommandFn::CmdTankMoveAck(Player* session, ICommand* cmd) {
	TcpTankMoveSyn* pTankMoveSyn = cmd->CastCommand<TcpTankMoveSyn*>();
}




