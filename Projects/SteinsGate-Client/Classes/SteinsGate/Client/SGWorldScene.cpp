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
#include <SteinsGate/Client/SGWndMessage.h>
#include <SteinsGate/Client/SGWin32Helper.h>

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

	const auto pKeyboardListener = EventListenerKeyboard::create();
	pKeyboardListener->onKeyPressed = CC_CALLBACK_2(SGWorldScene::onKeyPressed, this);
	pKeyboardListener->onKeyReleased = CC_CALLBACK_2(SGWorldScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pKeyboardListener, this);

	const auto pMouseListener = EventListenerMouse::create();
	pMouseListener->onMouseDown = CC_CALLBACK_1(SGWorldScene::onMouseDown, this);
	pMouseListener->onMouseScroll = CC_CALLBACK_1(SGWorldScene::onMouseScroll, this);
	pMouseListener->onMouseUp = CC_CALLBACK_1(SGWorldScene::onMouseUp, this);
	pMouseListener->onMouseMove = CC_CALLBACK_1(SGWorldScene::onMouseMove, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pMouseListener, this);
	
	const auto pFocusedListener = EventListenerCustom::create(GLViewImpl::EVENT_WINDOW_FOCUSED, CC_CALLBACK_1(SGWorldScene::onWndFocused, this));
	const auto pLostFocusedListener = EventListenerCustom::create(GLViewImpl::EVENT_WINDOW_UNFOCUSED, CC_CALLBACK_1(SGWorldScene::onWndLostFocused, this));
	const auto pResizedListener = EventListenerCustom::create(GLViewImpl::EVENT_WINDOW_RESIZED, CC_CALLBACK_1(SGWorldScene::onWndResized, this));
	const auto pCursorEnterListener = EventListenerCustom::create(GLViewImpl::EVENT_CURSOR_ENTER, CC_CALLBACK_1(SGWorldScene::onWndCursorEnter, this));
	const auto pCursorLeaveListener = EventListenerCustom::create(GLViewImpl::EVENT_CURSOR_LEAVE, CC_CALLBACK_1(SGWorldScene::onWndCursorLeave, this));

	_eventDispatcher->addEventListenerWithSceneGraphPriority(pFocusedListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pLostFocusedListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pResizedListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pCursorEnterListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pCursorLeaveListener, this);

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

	if (m_pUILayer)
		m_pUILayer->update(dt);

	// 초기 세팅 안된 상태거나, 다른 상태로 전환이 예약된 경우
	if (m_pRunningScene == nullptr || m_pRunningScene->getType() != m_eReservedScene) {
		changeScene(m_eReservedScene);
	}

}

void SGWorldScene::onWndMessageReceived(int code, WPARAM wParam, LPARAM lParam) {
	const char* codeName = SGWndMessage::GetName(code);
	
}

void SGWorldScene::onWndFocused(SGEventCustom* custom) {
}

void SGWorldScene::onWndLostFocused(SGEventCustom* custom) {
}


void SGWorldScene::onWndResized(SGEventCustom* custom) {
	SGSize size = _director->getOpenGLView()->getFrameSize();
}

void SGWorldScene::onWndCursorEnter(SGEventCustom* custom) {
}

void SGWorldScene::onWndCursorLeave(SGEventCustom* custom) {
	const SGVec2 leavePos = SGWin32Helper::getCursorPos();
	EventMouse* pEventMouse = dbg_new EventMouse(EventMouse::MouseEventType::MOUSE_MOVE);
	pEventMouse->setCursorPosition(leavePos.x, leavePos.y);

	if (m_pUILayer)
		m_pUILayer->onMouseMove(pEventMouse);

	CC_SAFE_DELETE(pEventMouse);
}

void SGWorldScene::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) const {

	if (keyCode == EventKeyboard::KeyCode::KEY_F1)
		SGGlobal::get()->toggleDrawBodyBoundingBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F2)
		SGGlobal::get()->toggleDrawThicknessBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F3)
		SGGlobal::get()->toggleDrawAttackBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F4)
		SGGlobal::get()->toggleDrawEffect();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F5)
		SGGlobal::get()->toggleDrawUIStatic();

	if (m_pRunningScene)
		m_pRunningScene->onKeyPressed(keyCode, event);

	if (m_pUILayer)
		m_pUILayer->onKeyPressed(keyCode, event);

}

void SGWorldScene::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) const {
	if (m_pRunningScene)
		m_pRunningScene->onKeyReleased(keyCode, event);

	if (m_pUILayer)
		m_pUILayer->onKeyReleased(keyCode, event);
}

void SGWorldScene::onMouseMove(SGEventMouse* mouseEvent) const {

	if (m_pRunningScene)
		m_pRunningScene->onMouseMove(mouseEvent);

	if (m_pUILayer)
		m_pUILayer->onMouseMove(mouseEvent);


	
}

void SGWorldScene::onMouseDown(SGEventMouse* mouseEvent) const {
	if (m_pRunningScene)
		m_pRunningScene->onMouseDown(mouseEvent);

	if (m_pUILayer)
		m_pUILayer->onMouseDown(mouseEvent);
}

void SGWorldScene::onMouseUp(SGEventMouse* mouseEvent) const {
	if (m_pRunningScene)
		m_pRunningScene->onMouseUp(mouseEvent);

	if (m_pUILayer)
		m_pUILayer->onMouseUp(mouseEvent);
}

void SGWorldScene::onMouseScroll(SGEventMouse* mouseEvent) const {
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

SGUILayer* SGWorldScene::getUILayer() const {
	DebugAssertMsg(m_pUILayer, "UI 레이어는 무조건 게임내내 생성되어있어야 합니다.");
	return m_pUILayer;
}
