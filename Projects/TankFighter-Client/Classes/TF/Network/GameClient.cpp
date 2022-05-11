#include <TF/Network/GameClient.h>
#include <Common/Enum.h>

#include <TF/Scenes/LoginScene.h>
#include <TF/Scenes/BattleFieldScene.h>
#include <TF/Scenes/CharacterSelectScene.h>
#include <TF/Scenes/LobbyScene.h>
#include <TF/Scenes/RoomScene.h>
#include <TF/Scenes/ChannelScene.h>


GameClient::~GameClient() {
	DeleteSafe(m_EventListener);
	DeleteSafe(m_Parser);
}

GameClient* GameClient::GetInstance() {
	if (ms_pInstance == nullptr) {
		ms_pInstance = new GameClient();
		ms_pInstance->m_Parser = new CommandParser();
		ms_pInstance->m_EventListener = new GameClientEventListener();
		ms_pInstance->InitializeParser();
		ms_pInstance->SetEventListener(ms_pInstance->m_EventListener);
	}

	return ms_pInstance;
}

void GameClient::InitializeParser() {
	// 쓸일이 없다.
}

void GameClient::UpdateCharacterInfo(CharacterInfo& info) {
	Memory::CopyUnsafe(&m_CharacterInfo, &info, sizeof(info));
}

void GameClient::ChangeScene(SceneType sceneType) {
	Scene* pScene = Director::getInstance()->getRunningScene();

	if (pScene == nullptr) {
		DebugAssert(false, "멍미? 씬이 없어요");
		return;
	}

	pScene->unscheduleUpdate();
	pScene->getEventDispatcher()->removeAllEventListeners();

	switch (sceneType) {
	case SceneType::Login:				Director::getInstance()->replaceScene(LoginScene::createScene());				break;
	case SceneType::Channel:			Director::getInstance()->replaceScene(ChannelScene::createScene());				break;
	case SceneType::CharacterSelect:	Director::getInstance()->replaceScene(CharacterSelectScene::createScene());		break;
	case SceneType::Lobby:				Director::getInstance()->replaceScene(LobbyScene::createScene());				break;
	case SceneType::Room:				Director::getInstance()->replaceScene(RoomScene::createScene());				break;
	case SceneType::BattleField:		Director::getInstance()->replaceScene(BattleFieldScene::createScene());			break;
	}
}


