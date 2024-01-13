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
#include <TF/Client/Game/Scene/BattleFieldScene.h>

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

void R_GAME::RECV_SC_Disconnect(Session* session, ICommand* cmd) {
	session->Disconnect();
}

void R_GAME::RECV_SC_Register(Session* session, ICommand* cmd) {
	SC_Register* pCmd = (SC_Register*)cmd;
}

void R_GAME::RECV_SC_Logout(Session* session, ICommand* cmd) {
	Core::GameClient->SetAccountPrimaryKey(Const::InvalidValue);
	Core::GameClient->SetAccountId("");
	Core::GameClient->SetChannelPrimaryKey(Const::InvalidValue);
	Core::GameClient->SetCharacterPrimaryKey(Const::InvalidValue);
	Core::GameClient->SetRoomAccessId(Const::InvalidValue);
	Core::GameClient->SetPlayerState(PlayerState::Initialized);
	Director::getInstance()->getOpenGLView()->setViewName(Const::Window::ViewName);
	Director::getInstance()->replaceScene(LoginScene::create());
}

void R_GAME::RECV_SC_LoadChannelInfo(Session* session, ICommand* cmd) {
	SC_LoadChannelInfo* pCmd = (SC_LoadChannelInfo*)cmd;
	ChannelScene* pChannelScene = dynamic_cast<ChannelScene*>(Director::getInstance()->getRunningScene());
	if (pChannelScene == nullptr) {
		_LogWarn_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Channel));
		return;
	}

	Director::getInstance()->getOpenGLView()->setViewName(StringUtils::format("%s [계정: %s]", Const::Window::ViewName, Core::GameClient->GetAccountId().Source()));
	pChannelScene->refreshChannelList(pCmd->Info(), pCmd->Count);
}

void R_GAME::RECV_SC_JoinChannel(Session* session, ICommand* cmd) {
	SC_JoinChannel* pCmd = (SC_JoinChannel*)cmd;
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
	Director::getInstance()->replaceScene(ChannelScene::create());
}

void R_GAME::RECV_SC_LoadCharacterInfo(Session* session, ICommand* cmd) {
	SC_LoadCharacterInfo* pCmd = (SC_LoadCharacterInfo*)cmd;
	CharacterSelectScene* pCharacterSelectScene = dynamic_cast<CharacterSelectScene*>(Director::getInstance()->getRunningScene());
	if (pCharacterSelectScene == nullptr) {
		_LogWarn_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::CharacterSelect));
		return;
	}

	Director::getInstance()->getOpenGLView()->setViewName(StringUtils::format("%s [계정: %s]", Const::Window::ViewName, Core::GameClient->GetAccountId().Source()));
	pCharacterSelectScene->refreshCharacterList(pCmd->Info(), pCmd->Count);
}

void R_GAME::RECV_SC_LeaveLobby(Session* session, ICommand* cmd) {
	Core::GameClient->SetRoomAccessId(Const::InvalidValue);
	Core::GameClient->SetCharacterPrimaryKey(Const::InvalidValue);
	Core::GameClient->SetCharacterInfo({});
	Core::GameClient->SetPlayerState(PlayerState::CharacterSelect);
	Director::getInstance()->replaceScene(CharacterSelectScene::create());
}

void R_GAME::RECV_SC_SelectCharacter(Session* session, ICommand* cmd) {
	SC_SelectCharacterAndJoinLobby* pCmd = (SC_SelectCharacterAndJoinLobby*)cmd;
	Core::GameClient->SetCharacterInfo(pCmd->info);
	LobbyScene* pLobbyScene = LobbyScene::create();
	Core::GameClient->SetPlayerState(PlayerState::Lobby);
	Director::getInstance()->getOpenGLView()->setViewName(StringUtils::format("%s [계정: %s][캐릭터: %s]", Const::Window::ViewName, Core::GameClient->GetAccountId().Source(), pCmd->info.Name.Source));
	Director::getInstance()->replaceScene(pLobbyScene);
	pLobbyScene->refreshCharacterInfo(pCmd->info);
}

void R_GAME::RECV_SC_UpdateCharacterInfo(Session* session, ICommand* cmd) {
	SC_UpdateCharacterInfo* pCmd = (SC_UpdateCharacterInfo*)cmd;
	LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
	if (pLobbyScene == nullptr) {
		_LogWarn_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
		return;
	}
	pLobbyScene->refreshCharacterInfo(pCmd->Info);
}

void R_GAME::RECV_SC_UpdateRoomList(Session* session, ICommand* cmd) {
	SC_UpdateRoomList* pCmd = (SC_UpdateRoomList*)cmd;
	LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
	if (pLobbyScene == nullptr) {
		_LogWarn_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
		return;
	}
	pLobbyScene->refreshRoomList(pCmd->Info(), pCmd->Count);
}

void R_GAME::RECV_SC_UpdatePlayerList(Session* session, ICommand* cmd) {
	SC_UpdatePlayerList* pCmd = (SC_UpdatePlayerList*)cmd;
	LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
	if (pLobbyScene == nullptr) {
		_LogWarn_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
		return;
	}

	pLobbyScene->refreshPlayerList(pCmd->Info(), pCmd->Count);
}

void R_GAME::RECV_SC_UpdateFriendList(Session* session, ICommand* cmd) {
	SC_UpdateFriendList* pCmd = (SC_UpdateFriendList*)cmd;
	LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
	if (pLobbyScene == nullptr) {
		_LogWarn_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
		return;
	}
	pLobbyScene->refreshFriendList(pCmd->Info(), pCmd->Count);
}

void R_GAME::RECV_SC_CreateRoom(Session* session, ICommand* cmd) {
	SC_CreateRoom* pCmd = (SC_CreateRoom*)cmd;
	LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
	if (pLobbyScene == nullptr) {
		_LogWarn_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
		return;
	}

	Core::GameClient->SetRoomAccessId(pCmd->RoomAccessId);
	Core::GameClient->SetPlayerState(PlayerState::Room);
	Director::getInstance()->replaceScene(RoomScene::create(Core::Room));
}

void R_GAME::RECV_SC_JoinRoom(Session* session, ICommand* cmd) {
	SC_JoinRoom* pCmd = (SC_JoinRoom*)cmd;
	LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
	if (pLobbyScene == nullptr) {
		_LogWarn_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
		return;
	}

	Core::GameClient->SetRoomAccessId(pCmd->RoomAccessId);
	Core::GameClient->SetPlayerState(PlayerState::Room);
	Director::getInstance()->replaceScene(RoomScene::create(Core::Room));
}

void R_GAME::RECV_SC_RoomGameStart(Session* session, ICommand* cmd) {
	SC_RoomGameStart* pCmd = (SC_RoomGameStart*)cmd;
	Core::GameClient->SetPlayerState(PlayerState::BattleField);
	Director::getInstance()->replaceScene(BattleFieldScene::create(Core::Room));
}


void R_GAME::RECV_SC_RoomGameReady(Session* session, ICommand* cmd) {
	SC_RoomGameReady* pCmd = (SC_RoomGameReady*)cmd;
	const int iMemberIndex = Core::Room->updateRoomMemberReadyState(pCmd->CharacterPrimaryKey, pCmd->Ready);

	RoomScene* pRoomScene = dynamic_cast<RoomScene*>(Director::getInstance()->getRunningScene());
	if (pRoomScene == nullptr) {
		_LogWarn_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Room));
		return;
	}
	pRoomScene->refreshRoomMemberReadyState(iMemberIndex);
}

void R_GAME::RECV_SC_RoomGameIntrude(Session* session, ICommand* cmd) {
	SC_RoomGameIntrude* pCmd = (SC_RoomGameIntrude*)cmd;
	Core::GameClient->SetIntruder(true);
	Core::GameClient->SetPlayerState(PlayerState::BattleField);
	Director::getInstance()->replaceScene(BattleFieldScene::create(Core::Room));
}

void R_GAME::RECV_SC_RoomLeave(Session* session, ICommand* cmd) {
	SC_RoomLeave* pCmd = (SC_RoomLeave*)cmd;
	Core::GameClient->SetIntruder(false);
	Core::GameClient->SetRoomAccessId(Const::InvalidValue);
	Core::GameClient->SetPlayerState(PlayerState::Lobby);
	Director::getInstance()->replaceScene(LobbyScene::create());
}

void R_GAME::RECV_SC_RoomGameEnd(Session* session, ICommand* cmd) {
	SC_RoomGameEnd* pCmd = (SC_RoomGameEnd*)cmd;
	
	if (Core::GameClient->GetPlayerState() == PlayerState::BattleField) {
		Core::GameClient->SetPlayerState(PlayerState::Room);
		Director::getInstance()->replaceScene(RoomScene::create(Core::Room));
	} else if (Core::GameClient->GetPlayerState() == PlayerState::Room) {
		Core::Room->setState(RoomState::ReadyWait);

		RoomScene* pRoomScene = dynamic_cast<RoomScene*>(Director::getInstance()->getRunningScene());
		if (pRoomScene == nullptr) {
			_LogWarn_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Room));
			return;
		}
		pRoomScene->refreshRoomInfo();
	}
}

void R_GAME::RECV_SC_LoadRoomInfo(Session* session, ICommand* cmd) {
	SC_LoadRoomInfo* pCmd = (SC_LoadRoomInfo*)cmd;
	Core::Room->updateRoomInfo(pCmd->Info);
	
	RoomScene* pRoomScene = dynamic_cast<RoomScene*>(Director::getInstance()->getRunningScene());
	if (pRoomScene == nullptr) {
		// 배틀필드에서도 방정보를 요청할 수도 있으므로.. 꼭 RoomScene이 아니어도됨.
		return;
	}
	
	pRoomScene->refreshRoomInfo();
}


void R_GAME::RECV_SC_UpdateRoomMemberList(Session* session, ICommand* cmd) {
	SC_UpdateRoomMemberList* pCmd = (SC_UpdateRoomMemberList*)cmd;
	Core::Room->updateRoomMemberList(pCmd->Info(), pCmd->Count, pCmd->HostCharacterPrimaryKey);

	if (Core::GameClient->GetPlayerState() == PlayerState::Room) {
		RoomScene* pRoomScene = dynamic_cast<RoomScene*>(Director::getInstance()->getRunningScene());
		if (pRoomScene == nullptr) {
			return;
		}
		pRoomScene->refreshRoomMemberInfoList();
	} else if (Core::GameClient->GetPlayerState() == PlayerState::BattleField) {
		BattleFieldScene* pBattleFieldScene = dynamic_cast<BattleFieldScene*>(Director::getInstance()->getRunningScene());
		if (pBattleFieldScene == nullptr) {
			return;
		}
		pBattleFieldScene->refreshTanks();
	}
	
}

void R_GAME::RECV_SC_UpdateRoomMember(Session* session, ICommand* cmd) {
	SC_UpdateRoomMember* pCmd = (SC_UpdateRoomMember*)cmd;
	Core::Room->updateRoomMember(pCmd->Info);
	RoomScene* pRoomScene = dynamic_cast<RoomScene*>(Director::getInstance()->getRunningScene());
	if (pRoomScene == nullptr) {
		return;
	}
	pRoomScene->refreshRoomMemberInfoList();

}

void R_GAME::RECV_SC_AddFriendRequest(Session* session, ICommand* cmd) {
	SC_AddFriendRequest* pCmd = (SC_AddFriendRequest*)cmd;
	const int iRequestCharacterAccessId = pCmd->Info.AccessId;
	PopUp::createInRunningScene(StringUtils::format("%s 님이 친구 요청을 보냈습니다. 수락하십니까?", pCmd->Info.Name.Source).c_str(), false,
		[=] { S_GAME::SEND_CS_AddFriendRequest(true, iRequestCharacterAccessId); },
		[=] { S_GAME::SEND_CS_AddFriendRequest(false, iRequestCharacterAccessId); });

}

void R_GAME::RECV_SC_ServerMessage(Session* session, ICommand* cmd) {
	SC_ServerMessage* pCmd = (SC_ServerMessage*)cmd;
	PopUp::createInRunningScene(pCmd->Msg()->Source, false);
}

void R_GAME::RECV_SC_ChatMessage(Session* session, ICommand* cmd) {
	SC_ChatMessage* pCmd = (SC_ChatMessage*)cmd;

	if (pCmd->PlayerState == PlayerState::Lobby) {
		LobbyScene* pLobbyScene = dynamic_cast<LobbyScene*>(Director::getInstance()->getRunningScene());
		if (pLobbyScene == nullptr) {
			_LogWarn_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::Lobby));
			return;
		}

		pLobbyScene->addChatMssage(pCmd->Msg()->Source);
	} else if (pCmd->PlayerState == PlayerState::BattleField) {
		BattleFieldScene* pBattleFieldScene = dynamic_cast<BattleFieldScene*>(Director::getInstance()->getRunningScene());
		if (pBattleFieldScene == nullptr) {
			_LogWarn_("%s씬이 아닙니다.", BaseScene::getTypeName(BaseScene::Type::BattleField));
			return;
		}

		pBattleFieldScene->addChatMssage(pCmd->Msg()->Source);
	}
}

void R_GAME::RECV_SC_BattleFieldTankSpawn(Session* session, ICommand* cmd) {
	SC_BattleFieldTankSpawn* pCmd = (SC_BattleFieldTankSpawn*)cmd;
	BattleFieldScene* pBattleFieldScene = dynamic_cast<BattleFieldScene*>(Director::getInstance()->getRunningScene());
	if (pBattleFieldScene == nullptr) {
		return;
	}
	pBattleFieldScene->spawnTanks(&pCmd->Move, 1);
}

void R_GAME::RECV_SC_BattleFieldTankList(Session* session, ICommand* cmd) {
	SC_BattleFieldTankList* pCmd = (SC_BattleFieldTankList*)cmd;
	BattleFieldScene* pBattleFieldScene = dynamic_cast<BattleFieldScene*>(Director::getInstance()->getRunningScene());
	if (pBattleFieldScene == nullptr) {
		return;
	}
	pBattleFieldScene->spawnTanks(pCmd->Move(), pCmd->Count);
}

void R_GAME::RECV_SC_BattleFieldTimeSync(Session* session, ICommand* cmd) {
	SC_BattleFieldTimeSync* pCmd = (SC_BattleFieldTimeSync*)cmd;
	BattleFieldScene* pBattleFieldScene = dynamic_cast<BattleFieldScene*>(Director::getInstance()->getRunningScene());
	if (pBattleFieldScene == nullptr) {
		return;
	}
	
	pBattleFieldScene->syncTime(pCmd->Elapsed);
	pBattleFieldScene->refreshUIByRoomState();
}

void R_GAME::RECV_SC_BattleFieldMove(Session* session, ICommand* cmd) {
	SC_BattleFieldMove* pCmd = (SC_BattleFieldMove*)cmd;
	BattleFieldScene* pBattleFieldScene = dynamic_cast<BattleFieldScene*>(Director::getInstance()->getRunningScene());
	if (pBattleFieldScene == nullptr) {
		return;
	}
	pBattleFieldScene->syncMove(pCmd->Move);
}

void R_GAME::RECV_SC_BattleFieldFire(Session* session, ICommand* cmd) {
	SC_BattleFieldFire* pCmd = (SC_BattleFieldFire*)cmd;
	BattleFieldScene* pBattleFieldScene = dynamic_cast<BattleFieldScene*>(Director::getInstance()->getRunningScene());
	if (pBattleFieldScene == nullptr) {
		return;
	}
	pBattleFieldScene->spawnBullet(pCmd->BulletInfo);
}

void R_GAME::RECV_SC_BattleFieldDeath(Session* session, ICommand* cmd) {
	SC_BattleFieldDeath* pCmd = (SC_BattleFieldDeath*)cmd;
	BattleFieldScene* pBattleFieldScene = dynamic_cast<BattleFieldScene*>(Director::getInstance()->getRunningScene());
	if (pBattleFieldScene == nullptr) {
		return;
	}
	pBattleFieldScene->removeTank(pCmd->DeadCharacterPrimaryKey);
}

void R_GAME::RECV_SC_BattleFieldStateChanged(Session* session, ICommand* cmd) {
	SC_BattleFieldStateChanged* pCmd = (SC_BattleFieldStateChanged*)cmd;
	Core::Room->setState(pCmd->State);
	BattleFieldScene* pBattleFieldScene = dynamic_cast<BattleFieldScene*>(Director::getInstance()->getRunningScene());
	if (pBattleFieldScene == nullptr) {
		return;
	}
	pBattleFieldScene->refreshUIByRoomState();
	pBattleFieldScene->onRoomStateChanged(pCmd->State);
}


void R_GAME::RECV_SC_BattleFieldStatisticsUpdate(Session* session, ICommand* cmd) {
	SC_BattleFieldStatisticsUpdate* pCmd = (SC_BattleFieldStatisticsUpdate*)cmd;
	BattleFieldScene* pBattleFieldScene = dynamic_cast<BattleFieldScene*>(Director::getInstance()->getRunningScene());
	if (pBattleFieldScene == nullptr) {
		return;
	}
	pBattleFieldScene->refreshStatistics(pCmd->Statistics(), pCmd->Count);
	
}

void R_GAME::RECV_SC_BattleFieldLeave(Session* session, ICommand* cmd) {
	SC_BattleFieldLeave* pCmd = (SC_BattleFieldLeave*)cmd;
	BattleFieldScene* pBattleFieldScene = dynamic_cast<BattleFieldScene*>(Director::getInstance()->getRunningScene());
	if (pBattleFieldScene == nullptr) {
		return;
	}
	pBattleFieldScene->removeTank(pCmd->CharacterPrimaryKey);
}

void R_GAME::RECV_SC_RoomGameJudge(Session* session, ICommand* cmd) {
	SC_RoomGameJudge* pCmd = (SC_RoomGameJudge*)cmd;
	BattleFieldScene* pBattleFieldScene = dynamic_cast<BattleFieldScene*>(Director::getInstance()->getRunningScene());
	if (pBattleFieldScene == nullptr) {
		return;
	}
	pBattleFieldScene->judge(pCmd->WinnerCharacterPrimaryKey);
}

void R_GAME::RECV_SC_TcpRTT(Session* session, ICommand* cmd) {
	SC_TcpRTT* pCmd = (SC_TcpRTT*)cmd;
}
