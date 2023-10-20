/*
 * 작성자: 윤정도
 * 생성일: 10/10/2023 5:09:29 PM
 * =====================
 *
 */


#include "Pch.h"
#include "C_GAME.h"

#include <TF/Client/Game/Scene/LoginScene.h>

USING_NS_CC;
USING_NS_JC;
USING_NS_JNET;

void C_GAME::ConnectedSynchronized() {

	if (Core::GameClient->IsReconnectMode()) {
		const JCore::String szMessage = StringUtil::Format("서버 접속 성공");
		PopUp::createInRunningScene(szMessage.Source(), true);
	}

	Core::GameClient->SetReconnectMode(false);
}

void C_GAME::ConnectFailedSynchronized(Int32U errorCode) {
	const JCore::String szMessage = StringUtil::Format("서버 접속 실패(%d)", errorCode);
	PopUp::createInRunningScene(szMessage.Source(), true);
	Core::GameClient->SetReconnectMode(false);
}

void C_GAME::DisconnectedSynchronized() {
	Core::GameClient->SetReconnectMode(false);
	Core::GameClient->SetAccountPrimaryKey(Const::InvalidValue);
	Core::GameClient->SetAccountId("");
	Core::GameClient->SetChannelPrimaryKey(Const::InvalidValue);
	Core::GameClient->SetCharacterPrimaryKey(Const::InvalidValue);
	Core::GameClient->SetRoomAccessId(Const::InvalidValue);
	Core::GameClient->SetPlayerState(PlayerState::Disconnected);
	Director::getInstance()->getOpenGLView()->setViewName(Const::Window::ViewName);
	Director::getInstance()->replaceScene(LoginScene::create());
}

