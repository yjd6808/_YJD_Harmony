/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 9:51:57 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
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
#include <SteinsGate/Client/SGFontPackage.h>


USING_NS_CC;
USING_NS_CCUI;
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

SGMapLayer* SGWorldScene::getMap() {
	DebugAssertMsg(m_pRunningScene && m_pRunningScene->getType() == SceneType::Game, "게임 씬이 실행중이지 않을때 맵을 가져올려고 시도했습니다.");
	return ((SGGameScene*)m_pRunningScene)->getMap();
}

SGCamera* SGWorldScene::getCamera() {
	return getMap()->getCamera();
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

	SGDataManager::get()->loadAll();
	initEventListeners();
	InitUILayer();
	reserveScene(SceneType::Login);
	scheduleUpdate();	// 즉시 update 1회 호출함

	auto progressTimer = ProgressTimer::create(Sprite::create("progressbar.png"));
	progressTimer->setType(ProgressTimer::Type::BAR);
	progressTimer->setMidpoint(Vec2(0, 1)); // 시작점을 위쪽으로 설정
	progressTimer->setBarChangeRate(Vec2(1, 0)); // 가로방향으로만 변하도록 설정
	progressTimer->setPercentage(100); // 초기 진행률 100% 설정
	progressTimer->setPosition(Vec2()); // 위치 설정
	this->addChild(progressTimer);

	// Progress Timer 감소 액션 실행
	auto progressTo = ProgressTo::create(5.0f, 0); // 5초 동안 0%로 감소하는 액션
	progressTimer->runAction(progressTo);

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

	// 삭제전 마지막 발악, 모든 UI 리소스 정리
	m_pUILayer->clearUnload();	

	// 강종시 하위 씬들의 onExit을 수동호출해주자.
	Scene::onExit();		

	// 자식노드 모두 정리 (onExit에서 제거하는줄 알았는데 아니네; 그냥 재귀 onExit 호출함.. 하..)
	// 씬 정리되기전에 모든 레퍼런스 카운트가 0가 되어야함.
	removeAllChildren();

	FinalizeClientCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
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
