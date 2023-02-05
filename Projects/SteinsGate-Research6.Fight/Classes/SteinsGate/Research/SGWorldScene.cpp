/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 9:51:57 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGWorldScene.h"

#include <SteinsGate/Research/SGActorBox.h>
#include <SteinsGate/Research/SGLoginScene.h>
#include <SteinsGate/Research/SGGameScene.h>
#include <SteinsGate/Research/SGServerSelectScene.h>

USING_NS_CC;
USING_NS_JC;

// ==============================================================
// 퍼블릭 스태틱
// ==============================================================

SGWorldScene* SGWorldScene::get() {
	static SGWorldScene* scene;

	if (scene == nullptr) {
		scene = new (std::nothrow) SGWorldScene();

		if (scene && scene->init()) {
			scene->autorelease();
			return scene;
		}

		CC_SAFE_DELETE(scene);
		return nullptr;
	}

	return scene;
}


// ==============================================================
// 멤버
// ==============================================================

SGWorldScene::SGWorldScene()
	: m_pRunningScene(nullptr)
{}
SGWorldScene::~SGWorldScene() {}

bool SGWorldScene::init() {

	if (!Scene::init())
		return false;

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(SGWorldScene::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(SGWorldScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	changeScene(SceneType::Login);
	scheduleUpdate();

	return true;
}
void SGWorldScene::update(float dt) {
	if (m_pRunningScene)
		m_pRunningScene->update(dt);
}

void SGWorldScene::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	if (m_pRunningScene)
		m_pRunningScene->onKeyPressed(keyCode, event);
}

void SGWorldScene::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	if (m_pRunningScene)
		m_pRunningScene->onKeyReleased(keyCode, event);
}

void SGWorldScene::changeScene(SceneType_t sceneType) {
	if (m_pRunningScene) {
		this->removeChild(m_pRunningScene);
	}

	switch (sceneType) {
	case SceneType::Login: m_pRunningScene = SGLoginScene::create(); break;
	case SceneType::ServerSelect: m_pRunningScene = SGServerSelectScene::create(); break;
	case SceneType::Game: m_pRunningScene = SGGameScene::create(); break;
	default: DebugAssertMsg(false, "[SGWorldScene] 이상한 씬 타입입니다."); return;
	}

	this->addChild(m_pRunningScene);
}


