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
	Director::getInstance()->replaceScene(CharacterSelectScene::create());
}

void R_GAME::RECV_SC_LeaveChannel(Session* session, JNetwork::ICommand* cmd) {
	SC_LeaveChannel* pCmd = (SC_LeaveChannel*)cmd;
	Core::GameClient->SetChannelPrimaryKey(Const::InvalidValue);
	Core::GameClient->SetCharacterPrimaryKey(Const::InvalidValue);
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

void R_GAME::RECV_SC_SelectCharacter(Session* session, ICommand* cmd) {
	SC_SelectCharacter* pCmd = (SC_SelectCharacter*)cmd;
}
void R_GAME::RECV_SC_UpdateCharacterInfo(Session* session, ICommand* cmd) {
	SC_UpdateCharacterInfo* pCmd = (SC_UpdateCharacterInfo*)cmd;
}
void R_GAME::RECV_SC_UpdateRoomList(Session* session, ICommand* cmd) {
	SC_UpdateRoomList* pCmd = (SC_UpdateRoomList*)cmd;
}
void R_GAME::RECV_SC_UpdateFriendList(Session* session, ICommand* cmd) {
	SC_UpdateFriendList* pCmd = (SC_UpdateFriendList*)cmd;
}
void R_GAME::RECV_SC_CreateRoom(Session* session, ICommand* cmd) {
	SC_CreateRoom* pCmd = (SC_CreateRoom*)cmd;
}
void R_GAME::RECV_SC_JoinRoom(Session* session, ICommand* cmd) {
	SC_JoinRoom* pCmd = (SC_JoinRoom*)cmd;
}
void R_GAME::RECV_SC_UpdateRoomInfo(Session* session, ICommand* cmd) {
	SC_UpdateRoomInfo* pCmd = (SC_UpdateRoomInfo*)cmd;
}
void R_GAME::RECV_SC_AddFriend(Session* session, ICommand* cmd) {
	SC_AddFriend* pCmd = (SC_AddFriend*)cmd;
}
void R_GAME::RECV_SC_AddFriendRequest(Session* session, ICommand* cmd) {
	SC_AddFriendRequest* pCmd = (SC_AddFriendRequest*)cmd;
}
void R_GAME::RECV_SC_DeleteFriend(Session* session, ICommand* cmd) {
	SC_DeleteFriend* pCmd = (SC_DeleteFriend*)cmd;
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