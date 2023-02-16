/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 9:51:57 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGWorldScene.h"


#include <SteinsGate/Client/SGLoginScene.h>
#include <SteinsGate/Client/SGGameScene.h>
#include <SteinsGate/Client/SGChannelSelectScene.h>
#include <SteinsGate/Client/SGUILayer.h>

#include <SteinsGate/Client/SGActorBox.h>
#include <SteinsGate/Client/SGHostPlayer.h>
#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGActorListenerManager.h>
#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGUIManager.h>


USING_NS_CC;
USING_NS_JC;

// ==============================================================
// 퍼블릭 스태틱
// ==============================================================

SGWorldScene* SGWorldScene::get() {
	static SGWorldScene* scene;

	if (scene == nullptr) {
		scene = dbg_new SGWorldScene();

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
	, m_eReservedScene(SceneType::Login)
	, m_pMouseListener(nullptr)
	, m_pKeyboardListener(nullptr)
	, m_pUILayer(nullptr)
{}

SGWorldScene::~SGWorldScene() {
	
}


bool SGWorldScene::init() {

	if (!Scene::init())
		return false;

	initEventListeners();
	InitUILayer();
	reserveScene(SceneType::Login);
	scheduleUpdate();	// 즉시 update 1회 호출함

	return true;
}

void SGWorldScene::initEventListeners() {
	m_pKeyboardListener = EventListenerKeyboard::create();
	m_pKeyboardListener->onKeyPressed = CC_CALLBACK_2(SGWorldScene::onKeyPressed, this);
	m_pKeyboardListener->onKeyReleased = CC_CALLBACK_2(SGWorldScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_pKeyboardListener, this);

	m_pMouseListener = EventListenerMouse::create();
	m_pMouseListener->onMouseDown = CC_CALLBACK_1(SGWorldScene::onMouseDown, this);
	m_pMouseListener->onMouseScroll = CC_CALLBACK_1(SGWorldScene::onMouseScroll, this);
	m_pMouseListener->onMouseUp = CC_CALLBACK_1(SGWorldScene::onMouseUp, this);
	m_pMouseListener->onMouseMove = CC_CALLBACK_1(SGWorldScene::onMouseMove, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_pMouseListener, this);
}

void SGWorldScene::InitUILayer() {
	m_pUILayer = SGUILayer::create();
	this->addChild(m_pUILayer, 1000);
}


void SGWorldScene::update(float dt) {
	updateScene(dt);
}



void SGWorldScene::updateScene(float dt) {

	if (m_pRunningScene)
		m_pRunningScene->update(dt);

	// 초기 세팅 안된 상태거나, 다른 상태로 전환이 예약된 경우
	if (m_pRunningScene == nullptr || m_pRunningScene->getType() != m_eReservedScene) {
		changeScene(m_eReservedScene);
	}
}

void SGWorldScene::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	if (m_pRunningScene)
		m_pRunningScene->onKeyPressed(keyCode, event);

}

void SGWorldScene::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	if (m_pRunningScene)
		m_pRunningScene->onKeyReleased(keyCode, event);
}

void SGWorldScene::onMouseMove(SGEventMouse* mouseEvent) {

	if (m_pRunningScene)
		m_pRunningScene->onMouseMove(mouseEvent);

	if (m_pUILayer)
		m_pUILayer->onMouseMove(mouseEvent);
}

void SGWorldScene::onMouseDown(SGEventMouse* mouseEvent) {
	if (m_pRunningScene)
		m_pRunningScene->onMouseDown(mouseEvent);

	if (m_pUILayer)
		m_pUILayer->onMouseDown(mouseEvent);
}

void SGWorldScene::onMouseUp(SGEventMouse* mouseEvent) {
	if (m_pRunningScene)
		m_pRunningScene->onMouseUp(mouseEvent);

	if (m_pUILayer)
		m_pUILayer->onMouseUp(mouseEvent);
}

void SGWorldScene::onMouseScroll(SGEventMouse* mouseEvent) {
	if (m_pRunningScene)
		m_pRunningScene->onMouseScroll(mouseEvent);

	if (m_pUILayer)
		m_pUILayer->onMouseScroll(mouseEvent);
}

void SGWorldScene::onExit() {
	

	m_pUILayer->clearUnload();	// 삭제전 마지막 발악, 모든 UI 리소스 정리

	delete SGUIManager::get();
	delete SGActorBox::get();
	delete SGActorListenerManager::get();
	delete SGHostPlayer::get();
	delete SGDataManager::get();
	delete SGImagePackManager::get();
	delete SGGlobal::get();

	Scene::onExit();	// 호출 필수네.. 자식 객체들 모두 정리; (이후로 자식노드 사용불가능 - m_pUILayer)

	_LogInfo_("월드 씬 종료");
}


void SGWorldScene::reserveScene(SceneType_t sceneType) {
	m_eReservedScene = sceneType;
}

void SGWorldScene::changeScene(SceneType_t sceneType) {
	if (m_pRunningScene) {
		this->removeChild(m_pRunningScene);
	}

	// 씬전환 시 UI 리소스 모두 해제
	m_pUILayer->clearUnload();
	m_pRunningScene = createScene(sceneType);
	this->addChild(m_pRunningScene);
}

SGSceneBase* SGWorldScene::createScene(SceneType_t sceneType) {
	SGSceneBase* pCreatedScene = nullptr;

	switch (sceneType) {
	case SceneType::Login:				pCreatedScene = SGLoginScene::create(); break;
	case SceneType::ChannelSelect:		pCreatedScene = SGChannelSelectScene::create(); break;
	case SceneType::Game:				pCreatedScene = SGGameScene::create(); break;
	default: DebugAssertMsg(false, "[SGWorldScene] 이상한 씬 타입입니다."); return nullptr;
	}

	return pCreatedScene;
}

SGUILayer* SGWorldScene::getUILayer() {
	DebugAssertMsg(m_pUILayer, "UI 레이어는 무조건 게임내내 생성되어있어야 합니다.");
	return m_pUILayer;
}
