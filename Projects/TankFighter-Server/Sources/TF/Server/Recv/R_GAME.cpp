/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 11:20:00 AM
 * =====================
 *
 */


#include "Pch.h"
#include "R_GAME.h"

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

	if (!Q_GAME::CheckAccountExist(iAccountPrimaryKey, pCmd->Id.Source, pCmd->Password.Source)) {
		S_GAME::SEND_SC_ServerMessage("아이디 또는 비밀번호가 일치하지 않습니다.");
		S_GAME::SEND_SC_Login(false, Const::InvalidValue);
		return;
	}

	if (Core::World->GetPlayerByAccountId(pCmd->Id.Source) != nullptr) {
		S_GAME::SEND_SC_ServerMessage("현재 접속중인 계정입니다.");
		S_GAME::SEND_SC_Login(false, Const::InvalidValue);
		return;
	}

	Q_GAME::UpdateLoginDate(iAccountPrimaryKey);

	Player* pPlayer = Player::Pop();
	pSession->SetPlayer(pPlayer);

	pPlayer->SetSession(pSession);
	pPlayer->SetAccountPrimaryKey(iAccountPrimaryKey);
	pPlayer->SetAccountId(pCmd->Id.Source);
	pPlayer->OnConnected();
	Core::World->AddPlayer(pPlayer);
	S_GAME::SEND_SC_Login(true, iAccountPrimaryKey);
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

void R_GAME::RECV_CS_Logout(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_Logout* pCmd = (CS_Logout*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr)
		return;

	if (pPlayer->GetAccountPrimaryKey() != pCmd->AccountPrimaryKey) {
		return;
	}

	pPlayer->LeaveChannel();
	pPlayer->OnDisconnected();

	S_GAME::SetInformation(session, SendStrategy::SendAsync);
	S_GAME::SEND_SC_Logout();
}

void R_GAME::RECV_CS_LoadChannelInfo(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_LoadChannelInfo* pCmd = (CS_LoadChannelInfo*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr)
		return;

	pPlayer->SetPlayerState(PlayerState::Channel);
	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);
	const Vector<ChannelInfo> infoList = Core::World->GetChannelInfoList();
	S_GAME::SEND_SC_LoadChannelInfo(infoList);
}

void R_GAME::RECV_CS_JoinChannel(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_JoinChannel* pCmd = (CS_JoinChannel*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) 
		return;

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);

	Channel* pChannel = Core::World->GetChannelByPrimaryKey(pCmd->ChannelPrimaryKey);
	int iAddResult = pChannel->Join(pPlayer);
	String szEchoMessage = String::Null;

	if (iAddResult == Const::Failed::Channel::AddFailedAlreadyExist)
		szEchoMessage = "채널 참가에 실패했습니다.";
	else if (iAddResult == Const::Failed::Channel::AddFailedFull)
		szEchoMessage = "채널 참가에 실패했습니다. \n인원이 가득 찼습니다.";

	if (!szEchoMessage.IsNull()) {
		S_GAME::SEND_SC_ServerMessage(szEchoMessage);
	}

	S_GAME::SEND_SC_SelectChannel(pCmd->ChannelPrimaryKey);
}

void R_GAME::RECV_CS_LeaveChannel(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_LeaveChannel* pCmd = (CS_LeaveChannel*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) 
		return;

	Channel* pChannel = pPlayer->GetChannel();
	if (pChannel == nullptr) 
		return;
	if (pChannel->GetPrimaryKey() != pCmd->ChannelPrimaryKey) 
		return;

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);

	pPlayer->LeaveChannel();

	S_GAME::SEND_SC_LeaveChannel();
}


void R_GAME::RECV_CS_LoadCharacterInfo(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_LoadCharacterInfo* pCmd = (CS_LoadCharacterInfo*)cmd;
	Player* pPlayer = pSession->GetPlayer();

	if (pPlayer == nullptr) 
		return;

	const int iAccountPrimaryKey = pPlayer->GetAccountPrimaryKey();
	if (iAccountPrimaryKey != pCmd->AccountPrimaryKey) 
		return;

	const int iChannelPrimaryKey = pPlayer->GetChannelPrimaryKey();
	if (iChannelPrimaryKey != pCmd->ChannelPrimaryKey) 
		return;

	pPlayer->SetPlayerState(PlayerState::CharacterSelect);

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);
	S_GAME::SEND_SC_LoadCharacterInfo(iAccountPrimaryKey, iChannelPrimaryKey);
}

void R_GAME::RECV_CS_CreateCharacter(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_CreateCharacter* pCmd = (CS_CreateCharacter*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) 
		return;

	const int iAccountPrimaryKey = pPlayer->GetAccountPrimaryKey();
	if (iAccountPrimaryKey != pCmd->AccountPrimaryKey) 
		return;

	const int iChannelPrimaryKey = pPlayer->GetChannelPrimaryKey();
	if (iChannelPrimaryKey != pCmd->ChannelPrimaryKey) 
		return;

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
	if (pPlayer == nullptr) 
		return;

	const int iAccountPrimaryKey = pPlayer->GetAccountPrimaryKey();
	if (iAccountPrimaryKey != pCmd->AccountPrimaryKey) 
		return;

	const int iChannelPrimaryKey = pPlayer->GetChannelPrimaryKey();
	if (iChannelPrimaryKey != pCmd->ChannelPrimaryKey) 
		return;

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);

	if (Q_GAME::DeleteCharacter(iAccountPrimaryKey, iChannelPrimaryKey, pCmd->CharacterName.Source)) {
		S_GAME::SEND_SC_LoadCharacterInfo(iAccountPrimaryKey, iChannelPrimaryKey);
	} else {
		S_GAME::SEND_SC_ServerMessage("캐릭터 삭제에 실패했습니다.");
	}
}

void R_GAME::RECV_CS_SelectCharacterAndJoinLobby(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_SelectCharacterAndJoinLobby* pCmd = (CS_SelectCharacterAndJoinLobby*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) 
		return;

	const int iAccountPrimaryKey = pPlayer->GetAccountPrimaryKey();
	if (iAccountPrimaryKey != pCmd->AccountPrimaryKey) 
		return;

	const int iChannelPrimaryKey = pPlayer->GetChannelPrimaryKey();
	if (iChannelPrimaryKey != pCmd->ChannelPrimaryKey) 
		return;

	const Qry::SelectCharacterInfoResult selectQryResult = Q_GAME::SelectCharacterInfo(iAccountPrimaryKey, iChannelPrimaryKey, pCmd->CharacterPrimaryKey);
	if (!selectQryResult.HasBindedResult) {
		S_GAME::SetInformation(session, SendStrategy::SendAsync);
		S_GAME::SEND_SC_ServerMessage("캐릭터 정보가 잘못되었습니다.");
		return;
	}

	ChannelLobby* pLobby = Core::World->GetChannelLobbyByPrimaryKey(pCmd->ChannelPrimaryKey);
	if (pLobby == nullptr) {
		DebugAssert(false);
		return;
	}

	CharacterInfo info;
	info.PrimaryKey = selectQryResult.PrimaryKey;
	info.Name = selectQryResult.Name;
	info.WinCount = selectQryResult.Win;
	info.LoseCount = selectQryResult.Lose;
	info.KillCount = selectQryResult.Kill;
	info.DeathCount = selectQryResult.Death;
	info.Money = selectQryResult.Money;
	Character* pCharacter = Character::Pop();
	pCharacter->SetPlayer(pPlayer);
	pCharacter->SetInfo(info, false);
	pCharacter->LoadFriendList(pLobby);

	// Join수행전 PlayerState를 Lobby로 수정해놓음(Join()에서 플레이어 목록을 브로드 캐스트할때 현재 참가한 유저기 LobbyState여야지 유저 정보가 포함되어 브로드캐스트 되기 때문)
	pPlayer->SetPlayerState(PlayerState::Lobby);
	pPlayer->SetCharacter(pCharacter);

	if (!pLobby->Join(pPlayer)) {
		S_GAME::SetInformation(session, SendStrategy::SendAsync);
		S_GAME::SEND_SC_Disconnect(pSession);
		return;
	}

	S_GAME::SetInformation(session, SendStrategy::SendAsync);
	S_GAME::SEND_SC_SelectCharacterAndJoinLobby(info);
}

void R_GAME::RECV_CS_LoadLobbyInfo(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_LoadLobbyInfo* pCmd = (CS_LoadLobbyInfo*)cmd;
	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) 
		return;


	const int iAccountPrimaryKey = pPlayer->GetAccountPrimaryKey();
	if (iAccountPrimaryKey != pCmd->AccountPrimaryKey) 
		return;

	Channel* pChannel = pPlayer->GetChannel();
	if (pChannel == nullptr) 
		return;
	if (pChannel->GetPrimaryKey() != pCmd->ChannelPrimaryKey) 
		return;

	Character* pCharacter = pPlayer->GetCharacter();
	if (pCharacter == nullptr) 
		return;

	pPlayer->SetPlayerState(PlayerState::Lobby);
	ChannelLobby* pLobby = Core::World->GetChannelLobbyByPrimaryKey(pCmd->ChannelPrimaryKey);

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(session, SendStrategy::SendAlloc);
	S_GAME::SEND_SC_UpdateCharacterInfo(pCharacter);
	S_GAME::SEND_SC_UpdateRoomList(pLobby);
	S_GAME::SEND_SC_UpdateFriendList(pCharacter);
	S_GAME::SEND_SC_UpdatePlayerList(pLobby);
}

void R_GAME::RECV_CS_LeaveLobby(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_LeaveLobby* pCmd = (CS_LeaveLobby*)cmd;
	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) return;

	const int iAccountPrimaryKey = pPlayer->GetAccountPrimaryKey();
	if (iAccountPrimaryKey != pCmd->AccountPrimaryKey) 
		return;

	Channel* pChannel = pPlayer->GetChannel();
	if (pChannel == nullptr) 
		return;
	if (pChannel->GetPrimaryKey() != pCmd->ChannelPrimaryKey) 
		return;

	Character* pCharacter = pPlayer->GetCharacter();
	if (pCharacter->GetPrimaryKey() != pCmd->CharacterPrimaryKey) 
		return;

	if (pPlayer->LeaveLobby()) {
		S_GAME::AutoFlush _;
		S_GAME::SetInformation(session, SendStrategy::SendAlloc);
		S_GAME::SEND_SC_LeaveLobby();
	}
}


void R_GAME::RECV_CS_AddFriend(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_AddFriend* pCmd = (CS_AddFriend*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr)
		return;

	// 로비여부 검사
	ChannelLobby* pLobby = pPlayer->GetChannelLobby();
	if (pLobby == nullptr)
		return;

	S_GAME::SetInformation(session, SendStrategy::SendAsync);

	Player* pTargetPlayer = pLobby->GetPlayerByName(pCmd->FriendName.Source);
	if (pTargetPlayer == nullptr) {
		S_GAME::SEND_SC_ServerMessage("현재 로비에 접속되어있지 않은 유저입니다.");
		return;
	}
	if (pTargetPlayer == pPlayer) {
		S_GAME::SEND_SC_ServerMessage("자기 자신에게 친구추가할 수 없습니다.");
		return;
	}

	Character* pCharacter = pPlayer->GetCharacter();
	if (pCharacter == nullptr) {
		S_GAME::SEND_SC_ServerMessage("상대방 플레이어 정보를 찾을 수 없습니다.");
		return;
	}

	Character* pTargetCharacter = pTargetPlayer->GetCharacter();
	if (pTargetCharacter == nullptr) {
		S_GAME::SEND_SC_ServerMessage("상대방 캐릭터 정보를 찾을 수 없습니다.");
		return;
	}

	if (pCharacter->IsFriend(pTargetCharacter->GetPrimaryKey())) {
		S_GAME::SEND_SC_ServerMessage("이미 친구추가된 상태입니다.");
		return;
	}

	// 상대방에게 친구요청을 보냄
	S_GAME::SetInformation(pTargetPlayer->GetSession(), SendStrategy::SendAsync);
	S_GAME::SEND_SC_AddFriendRequest(pCharacter);

	// 친구요청을 보냈다고 에코해줌
	S_GAME::SetInformation(pSession, SendStrategy::SendAsync);
	S_GAME::SEND_SC_ServerMessage("친구 요청을 성공적으로 보냈습니다.");
}

void R_GAME::RECV_CS_AddFriendRequest(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_AddFriendRequest* pCmd = (CS_AddFriendRequest*)cmd;

	Player* pAcceptPlayer = pSession->GetPlayer();
	if (pAcceptPlayer == nullptr)
		return;

	Character* pRequestCharactor = Character::GetByAccessId(pCmd->RequestCharacterAccessId);
	Character* pAcceptCharactor = pAcceptPlayer->GetCharacter();

	if (pAcceptCharactor == nullptr)
		return;
	if (pRequestCharactor == nullptr)
		return;

	Player* pRequestPlayer = pRequestCharactor->GetPlayer();


	// 거절한 경우
	if (!pCmd->Accept) {
		if (pRequestPlayer) {
			S_GAME::SetInformation(pRequestPlayer->GetSession(), SendStrategy::SendAsync);
			S_GAME::SEND_SC_ServerMessage("상대방이 친구 요청을 거절했습니다.");
		}
		return;
	}

	const int iAcceptCharacterPrimaryKey = pAcceptCharactor->GetPrimaryKey();
	const int iRequestCharacterPrimaryKey = pRequestCharactor->GetPrimaryKey();

	if (!Q_GAME::AddFriendship(iRequestCharacterPrimaryKey, iAcceptCharacterPrimaryKey)) {
		S_GAME::SetInformation(pAcceptPlayer->GetSession(), SendStrategy::SendAsync);
		S_GAME::SEND_SC_ServerMessage("친구추가 도중 오류가 발생하였습니다.");

		if (pRequestPlayer) {
			S_GAME::SetInformation(pRequestPlayer->GetSession(), SendStrategy::SendAsync);
			S_GAME::SEND_SC_ServerMessage("친구추가 도중 오류가 발생하였습니다.");
		}
		return;
	}

	pRequestCharactor->AddFriend(pAcceptCharactor);
	pAcceptCharactor->AddFriend(pRequestCharactor);


	// 수락자 친구 목록 업데이트
	S_GAME::SetInformation(pAcceptPlayer->GetSession(), SendStrategy::SendAsync);
	S_GAME::SEND_SC_UpdateFriendList(pAcceptCharactor);

	// 요청자 친구 목록 업데이트
	if (pRequestPlayer) {
		S_GAME::SetInformation(pRequestPlayer->GetSession(), SendStrategy::SendAsync);
		S_GAME::SEND_SC_UpdateFriendList(pRequestCharactor);
	}

}

void R_GAME::RECV_CS_DeleteFriend(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_DeleteFriend* pCmd = (CS_DeleteFriend*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr)
		return;

	Character* pCharacter = pPlayer->GetCharacter();
	if (pCharacter == nullptr)
		return;

	if (pCmd->DeleteCharacterPrimaryKey == pCharacter->GetPrimaryKey()) {
		S_GAME::SetInformation(pSession, SendStrategy::SendAsync);
		S_GAME::SEND_SC_ServerMessage("자기 자신은 친구 삭제할 수 없습니다.");
	}

	int iDeleteFriendCount = 0;
	if (!Q_GAME::DeleteFriend(pCharacter->GetPrimaryKey(), pCmd->DeleteCharacterPrimaryKey, &iDeleteFriendCount)) {
		S_GAME::SetInformation(pSession, SendStrategy::SendAsync);
		S_GAME::SEND_SC_ServerMessage("삭제중 오류가 발생하였습니다.");
		return;
	}

	if (iDeleteFriendCount <= 0) {
		return;
	}

	pCharacter->DeleteFriend(pCmd->DeleteCharacterPrimaryKey);


	// 요청자, 수락자 친구 목록 업데이트
	{
		S_GAME::AutoFlush _;
		S_GAME::SetInformation(pSession, SendStrategy::SendAlloc);
		S_GAME::SEND_SC_UpdateFriendList(pCharacter);
		S_GAME::SEND_SC_ServerMessage("성공적으로 제거하였습니다.");
	}

	// 삭제된 대상이 접속중인 경우 알려줌.
	ChannelLobby* pLobby = pPlayer->GetChannelLobby();
	if (pLobby == nullptr)
		return;

	Player* pDeletedPlayer = pLobby->GetPlayerByCharacterPrimaryKey(pCmd->DeleteCharacterPrimaryKey);
	if (pDeletedPlayer == nullptr)
		return;

	Character* pDeletedCharacter = pDeletedPlayer->GetCharacter();
	if (pDeletedCharacter == nullptr)
		return;

	{
		pDeletedCharacter->DeleteFriend(pCharacter->GetPrimaryKey());

		const String& szName = pCharacter->GetName();
		const char* szNameSource = szName.Source();
		const String szMsg = StringUtil::Format("%s가 당신을 친구 삭제하였습니다.", szNameSource);

		S_GAME::AutoFlush _;
		S_GAME::SetInformation(pDeletedPlayer->GetSession(), SendStrategy::SendAlloc);
		S_GAME::SEND_SC_UpdateFriendList(pDeletedCharacter);
		S_GAME::SEND_SC_ServerMessage(szMsg);
	}

}


void R_GAME::RECV_CS_CreateRoom(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_CreateRoom* pCmd = (CS_CreateRoom*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) 
		return;

	Channel* pChannel = pPlayer->GetChannel();
	if (pChannel == nullptr)
		return;

	ChannelLobby* pChannelLobby = pPlayer->GetChannelLobby();
	if (pChannelLobby == nullptr) 
		return;


	Room* pNewRoom = Room::Pop();
	if (!pChannelLobby->AddRoom(pNewRoom)) {
		DebugAssert(false);
		Room::Push(pNewRoom);
		return;
	}

	pNewRoom->SetLobby(pChannelLobby);
	pNewRoom->SetChannel(pChannel);
	pNewRoom->SetNameRaw(pCmd->RoomName.Source);
	const int iJoinResult = pNewRoom->Join(pPlayer);

	if (iJoinResult == Const::Success) {
		S_GAME::SetInformation(session, SendStrategy::SendAsync);
		S_GAME::SEND_SC_CreateRoom(pNewRoom->GetAccessId());
		return;
	}

	S_GAME::SetInformation(session, SendStrategy::SendAsync);
	S_GAME::SEND_SC_ServerMessage("방 생성에 실패했습니다.");
}

void R_GAME::RECV_CS_JoinRoom(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_JoinRoom* pCmd = (CS_JoinRoom*)cmd;
	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) return;

	// 로비여부 검사
	ChannelLobby* pLobby = pPlayer->GetChannelLobby();
	if (pLobby == nullptr) 
		return;

	Room* pRoom = Room::GetByAccessId(pCmd->RoomAccessId);

	const int iJoinResult = pRoom->Join(pPlayer);

	if (iJoinResult == Const::Success) {
		S_GAME::SetInformation(session, SendStrategy::SendAsync);
		S_GAME::SEND_SC_JoinRoom(pRoom->GetAccessId());
		return;
	}

	S_GAME::SetInformation(session, SendStrategy::SendAsync);
	if (iJoinResult == Const::Failed::Room::AddFailedAlreadyExist)	S_GAME::SEND_SC_ServerMessage("이미 참여중인 방입니다.");
	else if (iJoinResult == Const::Failed::Room::AddFailedClosed)	S_GAME::SEND_SC_ServerMessage("닫힌 방입니다.");
	else if (iJoinResult == Const::Failed::Room::AddFailedFull)		S_GAME::SEND_SC_ServerMessage("꽉찬 방입니다.");
}


void R_GAME::RECV_CS_RoomLeave(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_RoomLeave* pCmd = (CS_RoomLeave*)cmd;

	Room* pRoom = Room::GetByAccessId(pCmd->RoomAccessId);
	if (pRoom == nullptr) {
		return;
	}

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr)
		return;

	pPlayer->LeaveRoom();

	S_GAME::SetInformation(pSession, SendStrategy::SendAsync);
	S_GAME::SEND_SC_RoomLeave();
}

// Create 또는 Join 이후 방씬으로 전환완료된 후 클라가 각종 정보들 로딩을 요청함.
void R_GAME::RECV_CS_LoadRoomInfo(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_LoadRoomInfo* pCmd = (CS_LoadRoomInfo*)cmd;

	Room* pRoom = Room::GetByAccessId(pCmd->RoomAccessId);
	if (pRoom == nullptr) {
		return;
	}

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr)
		return;


	pRoom->BroadcastRoomMemberListInfo();

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(pSession, SendStrategy::SendAlloc);
	S_GAME::SEND_SC_LoadRoomInfo(pRoom);
	S_GAME::SEND_SC_UpdateCharacterInfo(pPlayer->GetCharacter());
}

void R_GAME::RECV_CS_RoomGameReady(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_RoomGameReady* pCmd = (CS_RoomGameReady*)cmd;

	Room* pRoom = Room::GetByAccessId(pCmd->RoomAccessId);
	if (pRoom == nullptr) {
		return;
	}

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr)
		return;

	Character* pCharacter = pPlayer->GetCharacter();
	if (pPlayer == nullptr)
		return;

	pCharacter->SetReady(pCmd->Ready);
	S_GAME::SEND_SC_RoomGameReadyBroadcast(pRoom, pCharacter->GetPrimaryKey(), pCmd->Ready);
}


void R_GAME::RECV_CS_RoomGameStart(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_RoomGameStart* pCmd = (CS_RoomGameStart*)cmd;

	Room* pRoom = Room::GetByAccessId(pCmd->RoomAccessId);
	if (pRoom == nullptr) {
		return;
	}

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer != pRoom->GetHostPlayer()) {
		return;
	}

	Character* pCharacter = pPlayer->GetCharacter();
	if (pCharacter->GetPrimaryKey() != pCmd->CharacterPrimaryKey) {
		return;
	}

	Channel* pChannel =  pPlayer->GetChannel();
	if (pChannel == nullptr) {
		return;
	}

	if (!pRoom->IsAllReady()) {
		S_GAME::SetInformation(pSession, SendStrategy::SendAsync);
		S_GAME::SEND_SC_ServerMessage("모든 플레이어가 준비되지 않았습니다.");
		return;
	}

	if (!pChannel->StartBattle(pRoom)) {
		S_GAME::SetInformation(pSession, SendStrategy::SendAsync);
		S_GAME::SEND_SC_ServerMessage("게임을 시작할 수 없습니다.");
	}
}

void R_GAME::RECV_CS_RoomGameIntrude(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_RoomGameIntrude* pCmd = (CS_RoomGameIntrude*)cmd;

	Room* pRoom = Room::GetByAccessId(pCmd->RoomAccessId);
	if (pRoom == nullptr) {
		return;
	}

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr) {
		return;
	}

	Character* pCharacter = pPlayer->GetCharacter();
	if (pCharacter->GetPrimaryKey() != pCmd->CharacterPrimaryKey) {
		return;
	}

	pPlayer->OnBattleBegin();

	S_GAME::SetInformation(pSession, SendStrategy::SendAsync);
	S_GAME::SEND_SC_RoomGameIntrude();
}

void R_GAME::RECV_CS_LoadBattleFieldInfo(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_LoadBattleFieldInfo* pCmd = (CS_LoadBattleFieldInfo*)cmd;

	Room* pRoom = Room::GetByAccessId(pCmd->RoomAccessId);
	if (pRoom == nullptr) {
		return;
	}

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr)
		return;

	Character* pCharacter = pPlayer->GetCharacter();
	if (pCharacter == nullptr)
		return;

	if (pPlayer->GetRoom() != pRoom) {
		S_GAME::SetInformation(pSession, SendStrategy::SendAsync);
		S_GAME::SEND_SC_Disconnect(pSession);
		return;
	}


	pRoom->BroadcastRoomMemberInfo(pCharacter);
	pRoom->BroadcastBattleFieldTankSpawn(pCharacter, false);
	pRoom->BroadcastBattleSatistics();

	S_GAME::AutoFlush _;
	S_GAME::SetInformation(pSession, SendStrategy::SendAlloc);
	S_GAME::SEND_SC_BattleFieldTimeSync(pRoom->GetBattleFieldTime());
	S_GAME::SEND_SC_BattleFieldTankList(pRoom);
	S_GAME::SEND_SC_LoadRoomInfo(pRoom);
}

void R_GAME::RECV_CS_BattleFieldFire(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_BattleFieldFire* pCmd = (CS_BattleFieldFire*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr)
		return;

	Room* pRoom = pPlayer->GetRoom();
	if (pRoom == nullptr) {
		return;
	}

	pRoom->BroadcastBattleFieldFire(pCmd->BulletInfo);
}

void R_GAME::RECV_CS_BattleFieldDeath(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_BattleFieldDeath* pCmd = (CS_BattleFieldDeath*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr)
		return;

	Character* pDeadCharacter = pPlayer->GetCharacter();
	if (pPlayer == nullptr)
		return;

	Room* pRoom = pPlayer->GetRoom();
	if (pRoom == nullptr) {
		return;
	}

	Character* pKillerCharacter = pRoom->GetCharacterByCharacterPrimaryKey(pCmd->KillerCharacterPrimaryKey);
	if (pKillerCharacter == nullptr) {
		return;
	}

	const int iKillerCharacterPrimaryKey = pCmd->KillerCharacterPrimaryKey;
	const int iDeadCharacterPrimaryKey = pDeadCharacter->GetPrimaryKey();

	Q_GAME::AddKillCount(iKillerCharacterPrimaryKey, 1);
	Q_GAME::AddDeathCount(iDeadCharacterPrimaryKey, 1);

	pKillerCharacter->AddBattleKillCount(1);
	pDeadCharacter->AddBattleDeathCount(1);

	pDeadCharacter->SetDeath(true);

	pRoom->BroadcastBattleFieldDeath(iDeadCharacterPrimaryKey);
	pRoom->BroadcastBattleSatistics();
}

void R_GAME::RECV_CS_BattleFieldMove(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_BattleFieldMove* pCmd = (CS_BattleFieldMove*)cmd;

	Player* pPlayer = pSession->GetPlayer();
	if (pPlayer == nullptr)
		return;

	Room* pRoom = pPlayer->GetRoom();
	if (pRoom == nullptr)
		return;

	Character* pCharacter = pPlayer->GetCharacter();
	if (pCharacter == nullptr)
		return;

	if (pCharacter->GetPrimaryKey() != pCmd->Move.CharacterPrimaryKey) {
		DebugAssert(false);
		return;
	}

	pCharacter->SetMove(pCmd->Move);
	pRoom->BroadcastBattleFieldMove(pCmd->Move);
}

void R_GAME::RECV_CS_ChatMessage(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_ChatMessage* pCmd = (CS_ChatMessage*)cmd;

	Player* pPlayer = pSession->GetPlayer();

	if (pPlayer == nullptr)
		return;

	if (pPlayer->GetPlayerState() != pCmd->PlayerState)
		return;


	if (pCmd->PlayerState == PlayerState::Lobby) {
		S_GAME::SEND_SC_ChatMessageBroadcastLobby(pPlayer->GetChannelLobby(), pCmd->Message.Source);
	} else if (pCmd->PlayerState == PlayerState::BattleField) {
		S_GAME::SEND_SC_ChatMessageBroadcastBattleField(pPlayer->GetRoom(), pCmd->Message.Source);
	}

}

void R_GAME::RECV_CS_TcpRTT(Session* session, ICommand* cmd) {
	GameSession* pSession = (GameSession*)session;
	CS_TcpRTT* pCmd = (CS_TcpRTT*)cmd;
}
