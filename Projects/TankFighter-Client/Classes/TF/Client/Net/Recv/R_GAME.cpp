/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 12:35:25 PM
 * =====================
 *
 */

#include "Pch.h"
#include "R_GAME.h"

#include <TF/Common/Command.h>

#include <TF/Client/Game/Scene/ChannelScene.h>
#include <TF/Client/Game/Scene/CharacterSelectScene.h>
#include <TF/Client/Game/Scene/LoginScene.h>
#include <TF/Client/Game/Scene/LobbyScene.h>
#include <TF/Client/Game/Scene/RoomScene.h>

#include <TF/Client/Net/Send/S_GAME.h>

USING_NS_CC;
USING_NS_JC;
USING_NS_JNET;

void R_GAME::RECV_SC_Login(Session* session, ICommand* cmd){
	SC_Login* pCmd = (SC_Login*)cmd;
	if (!pCmd->Result) return;

	if (pCmd->AccountPrimaryKey == Const::InvalidValue) {
		DebugAssert(pCmd->AccountPrimaryKey != Const::InvalidValue);
		return;
	}

	Core::GameClient->SetPlayerState(PlayerState::Channel);
	Core::GameClient->SetAccountPrimaryKey(pCmd->AccountPrimaryKey);
	Director::getInstance()->replaceScene(ChannelScene::create());

}
void R_GAME::RECV_SC_Register(Session* session, ICommand* cmd) {
	SC_Register* pCmd = (SC_Register*)cmd;
}

void R_GAME::RECV_SC_LoadChannelInfo(Session* session, ICommand* cmd) {
	SC_LoadChannelInfo* pCmd = (SC_LoadChannelInfo*)cmd;
	ChannelScene* pChannelScene = dynamic_cast<ChannelScene*>(Director::getInstance()->getRunningScene());
	if (pChannelScene == nullptr) {
		_LogError_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Channel));
		return;
	}

	pChannelScene->refreshChannelList(pCmd->Info, pCmd->Count);
}

void R_GAME::RECV_SC_SelectChannel(Session* session, ICommand* cmd) {
	SC_SelectChannel* pCmd = (SC_SelectChannel*)cmd;
	if (pCmd->ChannelPrimaryKey <= Const::InvalidValue) return;
	Core::GameClient->SetChannelPrimaryKey(pCmd->ChannelPrimaryKey);
	Core::GameClient->SetPlayerState(PlayerState::CharacterSelect);
	Director::getInstance()->replaceScene(CharacterSelectScene::create());
}

void R_GAME::RECV_SC_LeaveChannel(Session* session, ICommand* cmd) {
	SC_LeaveChannel* pCmd = (SC_LeaveChannel*)cmd;
	Core::GameClient->SetChannelPrimaryKey(Const::InvalidValue);
	Core::GameClient->SetCharacterInfo({});
	Core::GameClient->SetCharacterPrimaryKey(Const::InvalidValue);
	Core::GameClient->SetRoomAccessId(Const::InvalidValue);
	Core::GameClient->SetPlayerState(PlayerState::Channel);
	Director::getInstance()->getOpenGLView()->setViewName(Const::Window::ViewName);
	Director::getInstance()->replaceScene(ChannelScene::create());
}

void R_GAME::RECV_SC_LoadCharacterInfo(Session* session, ICommand* cmd) {
	SC_LoadCharacterInfo* pCmd = (SC_LoadCharacterInfo*)cmd;
	CharacterSelectScene* pCharacterSelectScene = dynamic_cast<CharacterSelectScene*>(Director::getInstance()->getRunningScene());
	if (pCharacterSelectScene == nullptr) {
		_LogError_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::CharacterSelect));
		return;
	}

	pCharacterSelectScene->refreshCharacterList(pCmd->Info, pCmd->Count);
}

void R_GAME::RECV_SC_LeaveLobby(Session* session, ICommand* cmd) {
	Core::GameClient->SetRoomAccessId(Const::InvalidValue);
	Core::GameClient->SetCharacterPrimaryKey(Const::InvalidValue);
	Core::GameClient->SetCharacterInfo({});
	Core::GameClient->SetPlayerState(PlayerState::CharacterSelect);
	Director::getInstance()->getOpenGLView()->setViewName(Const::Window::ViewName);
	Director::getInstance()->replaceScene(CharacterSelectScene::create());
}

void R_GAME::RECV_SC_SelectCharacter(Session* session, ICommand* cmd) {
	SC_SelectCharacter* pCmd = (SC_SelectCharacter*)cmd;
	Core::GameClient->SetCharacterInfo(pCmd->info);
	LobbyScene* pLobbyScene = LobbyScene::create();
	Core::GameClient->SetPlayerState(PlayerState::Lobby);
	Director::getInstance()->getOpenGLView()->setViewName(StringUtils::format("%s [%s]", Const::Window::ViewName, pCmd->info.Name.Source));
	Director::getInstance()->replaceScene(pLobbyScene);
	pLobbyScene->refreshCharacterInfo(pCmd->info);
}

void R_GAME::RECV_SC_UpdateCharacterInfo(Session* session, ICommand* cmd) {
	SC_UpdateCharacterInfo* pCmd = (SC_UpdateCharacterInfo*)cmd;
	LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
	if (pLobbyScene == nullptr) {
		_LogError_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
		return;
	}
	pLobbyScene->refreshCharacterInfo(pCmd->Info);
}

void R_GAME::RECV_SC_UpdateRoomList(Session* session, ICommand* cmd) {
	SC_UpdateRoomList* pCmd = (SC_UpdateRoomList*)cmd;
	LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
	if (pLobbyScene == nullptr) {
		_LogError_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
		return;
	}
	pLobbyScene->refreshRoomList(pCmd->Info, pCmd->Count);
}

void R_GAME::RECV_SC_UpdatePlayerList(Session* session, ICommand* cmd) {
	SC_UpdatePlayerList* pCmd = (SC_UpdatePlayerList*)cmd;
	LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
	if (pLobbyScene == nullptr) {
		_LogError_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
		return;
	}

	pLobbyScene->refreshPlayerList(pCmd->Info, pCmd->Count);
}

void R_GAME::RECV_SC_UpdateFriendList(Session* session, ICommand* cmd) {
	SC_UpdateFriendList* pCmd = (SC_UpdateFriendList*)cmd;
	LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
	if (pLobbyScene == nullptr) {
		_LogError_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
		return;
	}
	pLobbyScene->refreshFriendList(pCmd->Info, pCmd->Count);
}

void R_GAME::RECV_SC_CreateRoom(Session* session, ICommand* cmd) {
	SC_CreateRoom* pCmd = (SC_CreateRoom*)cmd;
	LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
	if (pLobbyScene == nullptr) {
		_LogError_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
		return;
	}

	Core::GameClient->SetRoomAccessId(pCmd->RoomAccessId);
	Core::GameClient->SetPlayerState(PlayerState::Room);
	Director::getInstance()->replaceScene(RoomScene::create());
}

void R_GAME::RECV_SC_JoinRoom(Session* session, ICommand* cmd) {
	SC_JoinRoom* pCmd = (SC_JoinRoom*)cmd;
	LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
	if (pLobbyScene == nullptr) {
		_LogError_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
		return;
	}

	Core::GameClient->SetRoomAccessId(pCmd->RoomAccessId);
	Core::GameClient->SetPlayerState(PlayerState::Room);
	Director::getInstance()->replaceScene(RoomScene::create());
}

void R_GAME::RECV_SC_LoadRoomInfo(Session* session, ICommand* cmd) {
	SC_LoadRoomInfo* pCmd = (SC_LoadRoomInfo*)cmd;
	RoomScene* pRoomScene = dynamic_cast<RoomScene*>(Director::getInstance()->getRunningScene());
	if (pRoomScene == nullptr) {
		_LogError_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Room));
		return;
	}
	pRoomScene->refreshRoomInfo(pCmd->Info);
}


void R_GAME::RECV_SC_UpdateRoomMemberList(JNetwork::Session* session, JNetwork::ICommand* cmd) {
	SC_UpdateRoomMemberList* pCmd = (SC_UpdateRoomMemberList*)cmd;
	RoomScene* pRoomScene = dynamic_cast<RoomScene*>(Director::getInstance()->getRunningScene());
	if (pRoomScene == nullptr) {
		_LogError_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Room));
		return;
	}

	pRoomScene->refreshRoomMemberInfoList(pCmd->Info, pCmd->Count, pCmd->HostCharacterPrimaryKey);
}

void R_GAME::RECV_SC_AddFriendRequest(Session* session, ICommand* cmd) {
	SC_AddFriendRequest* pCmd = (SC_AddFriendRequest*)cmd;
	const int iRequestCharacterAccessId = pCmd->Info.AccessId;
	PopUp::createInRunningScene(StringUtils::format("%s 님이 친구 요청을 보냈습니다. 수락하십니까?", pCmd->Info.Name.Source).c_str(), false,
		[=] { S_GAME::SEND_CS_AddFriendRequest(true, iRequestCharacterAccessId); },
		[=] { S_GAME::SEND_CS_AddFriendRequest(false, iRequestCharacterAccessId); });

}

void R_GAME::RECV_SC_BattleFieldLoad(Session* session, ICommand* cmd) {
	SC_BattleFieldLoad* pCmd = (SC_BattleFieldLoad*)cmd;
}
void R_GAME::RECV_SC_BattleFieldLeave(Session* session, ICommand* cmd) {
	SC_BattleFieldLeave* pCmd = (SC_BattleFieldLeave*)cmd;
}

void R_GAME::RECV_SC_ServerMessage(Session* session, ICommand* cmd) {
	SC_ServerMessage* pCmd = (SC_ServerMessage*)cmd;
	PopUp::createInRunningScene(pCmd->Message.Source, false);
}

void R_GAME::RECV_SC_ChatMessage(Session* session, ICommand* cmd) {
	SC_ChatMessage* pCmd = (SC_ChatMessage*)cmd;

	if (pCmd->PlayerState == PlayerState::Lobby) {
		LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
		if (pLobbyScene == nullptr) {
			_LogError_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
			return;
		}

		pLobbyScene->addChatMssage(pCmd->Message.Source);
	}
}
void R_GAME::RECV_SC_BattleFieldFire(Session* session, ICommand* cmd) {
	SC_BattleFieldFire* pCmd = (SC_BattleFieldFire*)cmd;
}
void R_GAME::RECV_SC_BattleFieldDeath(Session* session, ICommand* cmd) {
	SC_BattleFieldDeath* pCmd = (SC_BattleFieldDeath*)cmd;
}
void R_GAME::RECV_SC_BattleFieldRevival(Session* session, ICommand* cmd) {
	//SC_BattleFieldRevival* pCmd = (SC_BattleFieldRevival*)cmd;
}
void R_GAME::RECV_SC_BattleFieldStatisticsUpdate(Session* session, ICommand* cmd) {
	//SC_BattleFieldStatisticsUpdate* pCmd = (SC_BattleFieldStatisticsUpdate*)cmd;
}
void R_GAME::RECV_SC_TcpRTT(Session* session, ICommand* cmd) {
	SC_TcpRTT* pCmd = (SC_TcpRTT*)cmd;
}