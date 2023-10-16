/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 11:20:00 AM
 * =====================
 *
 */


#include "Pch.h"
#include "R_GAME.h"

#include <source_location>
#include <TF/Common/Command.h>

#include <TF/Server/Contents/Player.h>
#include <TF/Server/Contents/Room.h>
#include <TF/Server/Contents/Channel.h>
#include <TF/Server/Contents/Character.h>
#include <TF/Server/Contents/World.h>

#include <TF/Server/Query/Q_GAME.h>
#include <TF/Server/Send/S_GAME.h>
#include <TF/Server/Host/GameSession.h>

USING_NS_JNET;
USING_NS_JC;

void R_GAME::RECV_CS_Login(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_Login* pCmd = (CS_Login*)cmd;
	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);

	int iAccountPrimaryKey = Const::InvalidValue;
	bool bSuccess = true;

	if (!Q_GAME::CheckAccountExist(iAccountPrimaryKey, pCmd->Id.Source, pCmd->Password.Source)) {
		bSuccess = false;
		S_GAME::SEND_SC_ServerMessage("아이디 또는 비밀번호가 일치하지 않습니다.");
	}

	if (bSuccess) {
		Q_GAME::UpdateLoginDate(iAccountPrimaryKey);

		Player* pPlayer = Player::Pop();
		pSession->SetPlayer(pPlayer);

		pPlayer->SetAccountPrimaryKey(iAccountPrimaryKey);
		pPlayer->OnConnected();
	}

	S_GAME::SEND_SC_Login(bSuccess, iAccountPrimaryKey);
}


void R_GAME::RECV_CS_Register(Session* session, ICommand* cmd) {
	CS_Register* pCmd = (CS_Register*)cmd;
	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);

	bool bAlreadyExist = false;
	bool bRegisterSuccess = false;
	const char* szMessage;

	if (Q_GAME::CheckAccountIdExist(pCmd->Id.Source)) {
		bAlreadyExist = true;
		szMessage = "이미 해당 아이디가 존재합니다.";
	}

	if (!bAlreadyExist && Q_GAME::InsertAccountInfo(pCmd->Id.Source, pCmd->Password.Source)) {
		bRegisterSuccess = true;
		szMessage = "회원가입에 성공하였습니다.";
	} else {
		szMessage = "회원가입에 실패하였습니다.";
	}

	S_GAME::SEND_SC_ServerMessage(szMessage);
	S_GAME::SEND_SC_Register(bRegisterSuccess);
}

void R_GAME::RECV_CS_LoadChannelInfo(Session* session, ICommand* cmd) {
	CS_LoadChannelInfo* pCmd = (CS_LoadChannelInfo*)cmd;
	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);
	const Vector<ChannelInfo> infoList = Core::World->GetChannelInfoList();
	S_GAME::SEND_SC_LoadChannelInfo(&infoList[0], infoList.Size());
}

void R_GAME::RECV_CS_SelectChannel(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_SelectChannel* pCmd = (CS_SelectChannel*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) return;

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);

	Channel* pChannel = Core::World->GetChannelByPrimaryKey(pCmd->ChannelPrimaryKey);
	int iAddResult = pChannel->AddPlayer(pPlayer);
	String szEchoMessage = String::Null;
	pPlayer->SetChannel(pChannel);

	if (iAddResult == Const::Failed::Channel::AddFailedAlreadyExist)
		szEchoMessage = "채널 참가에 실패했습니다.";
	else if (iAddResult == Const::Failed::Channel::AddFailedFull)
		szEchoMessage = "채널 참가에 실패했습니다. \n인원이 가득 찼습니다.";

	if (!szEchoMessage.IsNull()) {
		S_GAME::SEND_SC_ServerMessage(szEchoMessage);
	}

	S_GAME::SEND_SC_SelectChannel(pCmd->ChannelPrimaryKey);
}

void R_GAME::RECV_CS_LeaveChannel(Session* session, JNetwork::ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_LeaveChannel* pCmd = (CS_LeaveChannel*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) return;

	Channel* pChannel = pPlayer->GetChannel();
	if (pChannel == nullptr) return;
	if (pChannel->GetPrimaryKey() != pCmd->ChannelPrimaryKey) return;

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);

	if (pChannel->RemovePlayer(pPlayer)) {
		S_GAME::SEND_SC_LeaveChannel();
	}

	pPlayer->SetChannel(nullptr);
}


void R_GAME::RECV_CS_LoadCharacterInfo(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_LoadCharacterInfo* pCmd = (CS_LoadCharacterInfo*)cmd;
	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) return;

	const int iAccountPrimaryKey = pPlayer->GetAccountPrimaryKey();
	if (iAccountPrimaryKey != pCmd->AccountPrimaryKey) return;

	const int iChannelPrimaryKey = pPlayer->GetChannelPrimaryKey();
	if (iChannelPrimaryKey != pCmd->ChannelPrimaryKey) return;

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);
	S_GAME::SEND_SC_LoadCharacterInfo(iAccountPrimaryKey, iChannelPrimaryKey);
}

void R_GAME::RECV_CS_CreateCharacter(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_CreateCharacter* pCmd = (CS_CreateCharacter*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) return;

	const int iAccountPrimaryKey = pPlayer->GetAccountPrimaryKey();
	if (iAccountPrimaryKey != pCmd->AccountPrimaryKey) return;

	const int iChannelPrimaryKey = pPlayer->GetChannelPrimaryKey();
	if (iChannelPrimaryKey != pCmd->ChannelPrimaryKey) return;

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);

	if (Q_GAME::CheckCharacterNameExist(pCmd->CharacterName.Source, iChannelPrimaryKey)) {
		S_GAME::SEND_SC_ServerMessage("해당 캐릭터명이 이미 존재합니다.");
		return;
	}

	if (Q_GAME::CreateCharacter(iAccountPrimaryKey, iChannelPrimaryKey, pCmd->CharacterName.Source, 0, 0, 0, 0, 100'000)) {
		S_GAME::SEND_SC_LoadCharacterInfo(iAccountPrimaryKey, iChannelPrimaryKey);
	} else {
		S_GAME::SEND_SC_ServerMessage("캐릭터 생성에 실패했습니다.");
	}
}
void R_GAME::RECV_CS_DeleteCharacter(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_DeleteCharacter* pCmd = (CS_DeleteCharacter*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) return;

	const int iAccountPrimaryKey = pPlayer->GetAccountPrimaryKey();
	if (iAccountPrimaryKey != pCmd->AccountPrimaryKey) return;

	const int iChannelPrimaryKey = pPlayer->GetChannelPrimaryKey();
	if (iChannelPrimaryKey != pCmd->ChannelPrimaryKey) return;

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);

	if (Q_GAME::DeleteCharacter(iAccountPrimaryKey, iChannelPrimaryKey, pCmd->CharacterName.Source)) {
		S_GAME::SEND_SC_LoadCharacterInfo(iAccountPrimaryKey, iChannelPrimaryKey);
	} else {
		S_GAME::SEND_SC_ServerMessage("캐릭터 삭제에 실패했습니다.");
	}
}

void R_GAME::RECV_CS_SelectCharacter(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_SelectCharacter* pCmd = (CS_SelectCharacter*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) return;

	const int iAccountPrimaryKey = pPlayer->GetAccountPrimaryKey();
	if (iAccountPrimaryKey != pCmd->AccountPrimaryKey) return;

	const int iChannelPrimaryKey = pPlayer->GetChannelPrimaryKey();
	if (iChannelPrimaryKey != pCmd->ChannelPrimaryKey) return;

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);

	const Qry::SelectCharacterInfoResult selectQryResult = Q_GAME::SelectCharacterInfo(iAccountPrimaryKey, iChannelPrimaryKey, pCmd->CharacterPrimaryKey);
	if (!selectQryResult.HasBindedResult) {
		S_GAME::SEND_SC_ServerMessage("캐릭터 정보가 잘못되었습니다.");
		return;
	}

	CharacterInfo info;
	info.PrimaryKey = selectQryResult.PrimaryKey;
	info.Name = selectQryResult.Name;
	info.Win = selectQryResult.Win;
	info.Lose = selectQryResult.Lose;
	info.Kill = selectQryResult.Kill;
	info.Death = selectQryResult.Death;
	info.Money = selectQryResult.Money;
	Character* pCharacter = Character::Pop();
	pPlayer->SetCharacter(pCharacter);
	S_GAME::SEND_SC_SelectCharacter(info);
}

void R_GAME::RECV_CS_JoinLobby(Session* session, ICommand* cmd) {
	CS_JoinLobby* pCmd = (CS_JoinLobby*)cmd;
}

void R_GAME::RECV_CS_CreateRoom(Session* session, ICommand* cmd) {
	CS_CreateRoom* pCmd = (CS_CreateRoom*)cmd;
}
void R_GAME::RECV_CS_JoinRoom(Session* session, ICommand* cmd) {
	CS_JoinRoom* pCmd = (CS_JoinRoom*)cmd;
}
void R_GAME::RECV_CS_AddFriend(Session* session, ICommand* cmd) {
	CS_AddFriend* pCmd = (CS_AddFriend*)cmd;
}
void R_GAME::RECV_CS_AddFriendRequest(Session* session, ICommand* cmd) {
	CS_AddFriendRequest* pCmd = (CS_AddFriendRequest*)cmd;
}
void R_GAME::RECV_CS_DeleteFriend(Session* session, ICommand* cmd) {
	CS_DeleteFriend* pCmd = (CS_DeleteFriend*)cmd;
}
void R_GAME::RECV_CS_LoadRoomInfo(Session* session, ICommand* cmd) {
	CS_LoadRoomInfo* pCmd = (CS_LoadRoomInfo*)cmd;
}
void R_GAME::RECV_CS_RoomGameStart(Session* session, ICommand* cmd) {
	CS_RoomGameStart* pCmd = (CS_RoomGameStart*)cmd;
}
void R_GAME::RECV_CS_RoomGameReady(Session* session, ICommand* cmd) {
	CS_RoomGameReady* pCmd = (CS_RoomGameReady*)cmd;
}
void R_GAME::RECV_CS_RoomGameReadyCancel(Session* session, ICommand* cmd) {
	CS_RoomGameReadyCancel* pCmd = (CS_RoomGameReadyCancel*)cmd;
}
void R_GAME::RECV_CS_RoomLeave(Session* session, ICommand* cmd) {
	CS_RoomLeave* pCmd = (CS_RoomLeave*)cmd;
}
void R_GAME::RECV_CS_BattleFieldLoad(Session* session, ICommand* cmd) {
	CS_BattleFieldLoad* pCmd = (CS_BattleFieldLoad*)cmd;
}
void R_GAME::RECV_CS_BattileFieldTankMove(Session* session, ICommand* cmd) {
	CS_BattileFieldTankMove* pCmd = (CS_BattileFieldTankMove*)cmd;
}
void R_GAME::RECV_CS_BattileFieldTankUpdate(Session* session, ICommand* cmd) {
	CS_BattileFieldTankUpdate* pCmd = (CS_BattileFieldTankUpdate*)cmd;
}
void R_GAME::RECV_CS_BattleFieldPlayWaitEnd(Session* session, ICommand* cmd) {
	CS_BattleFieldPlayWaitEnd* pCmd = (CS_BattleFieldPlayWaitEnd*)cmd;
}
void R_GAME::RECV_CS_BattleFieldPlayingEnd(Session* session, ICommand* cmd) {
	CS_BattleFieldPlayingEnd* pCmd = (CS_BattleFieldPlayingEnd*)cmd;
}
void R_GAME::RECV_CS_BattleFieldEndWaitEnd(Session* session, ICommand* cmd) {
	CS_BattleFieldEndWaitEnd* pCmd = (CS_BattleFieldEndWaitEnd*)cmd;
}
void R_GAME::RECV_CS_BattleFieldLeave(Session* session, ICommand* cmd) {
	CS_BattleFieldLeave* pCmd = (CS_BattleFieldLeave*)cmd;
}
void R_GAME::RECV_CS_ChatMessage(Session* session, ICommand* cmd) {
	CS_ChatMessage* pCmd = (CS_ChatMessage*)cmd;
}
void R_GAME::RECV_CS_BattleFieldFire(Session* session, ICommand* cmd) {
	CS_BattleFieldFire* pCmd = (CS_BattleFieldFire*)cmd;
}
void R_GAME::RECV_CS_BattleFieldDeath(Session* session, ICommand* cmd) {
	CS_BattleFieldDeath* pCmd = (CS_BattleFieldDeath*)cmd;
}
void R_GAME::RECV_CS_BattleFieldRevival(Session* session, ICommand* cmd) {
	CS_BattleFieldRevival* pCmd = (CS_BattleFieldRevival*)cmd;
}
void R_GAME::RECV_CS_BattleFieldStatisticsUpdate(Session* session, ICommand* cmd) {
	CS_BattleFieldStatisticsUpdate* pCmd = (CS_BattleFieldStatisticsUpdate*)cmd;
}
void R_GAME::RECV_CS_TcpRTT(Session* session, ICommand* cmd) {
	CS_TcpRTT* pCmd = (CS_TcpRTT*)cmd;
	}