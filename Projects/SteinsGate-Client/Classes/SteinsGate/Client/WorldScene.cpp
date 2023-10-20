/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 9:51:57 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "WorldScene.h"


#include <SteinsGate/Client/LoginScene.h>
#include <SteinsGate/Client/GameScene.h>
#include <SteinsGate/Client/ChannelSelectScene.h>
#include <SteinsGate/Client/UILayer.h>

#include <SteinsGate/Client/ActorManager.h>
#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/ActorListenerManager.h>
#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/UIManager.h>
#include <SteinsGate/Client/FontManager.h>
#include <SteinsGate/Client/WndMessage.h>
#include <SteinsGate/Client/Win32Helper.h>



USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;
USING_NS_JNET;

// ==============================================================
// 퍼블릭 스태틱
// ==============================================================

WorldScene* WorldScene::get() {
	static WorldScene* scene;

	if (scene == nullptr) {
		scene = dbg_new WorldScene();

		if (scene && scene->init()) {
			scene->autorelease();
			return scene;
		}

		CC_SAFE_DELETE(scene);
		return nullptr;
	}

	return scene;
}

MapLayer* WorldScene::getMap() {
	DebugAssertMsg(m_pRunningScene && m_pRunningScene->getType() == SceneType::Game, "게임 씬이 실행중이지 않을때 맵을 가져올려고 시도했습니다.");
	return ((SGGameScene*)m_pRunningScene)->getMap();
}

MimicCamera* WorldScene::getCamera() {
	return getMap()->getCamera();
}


// ==============================================================
// 멤버
// ==============================================================

WorldScene::WorldScene()
	: m_pRunningScene(nullptr)
	, m_eReservedScene(SceneType::Login)
	, m_pUILayer(nullptr)
{}

WorldScene::~WorldScene() {
	
}

bool WorldScene::init() {

	if (!Scene::init())
		return false;

	initEventListeners();
	InitLayers();
	reserveScene(SceneType::Login);
	scheduleUpdate();	// 즉시 update 1회 호출함

	return true;
}


void WorldScene::initEventListeners() {

	const auto pKeyboardListener = EventListenerKeyboard::create();
	pKeyboardListener->onKeyPressed = CC_CALLBACK_2(WorldScene::onKeyPressed, this);
	pKeyboardListener->onKeyReleased = CC_CALLBACK_2(WorldScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pKeyboardListener, this);

	const auto pMouseListener = EventListenerMouse::create();
	pMouseListener->onMouseDown = CC_CALLBACK_1(WorldScene::onMouseDown, this);
	pMouseListener->onMouseScroll = CC_CALLBACK_1(WorldScene::onMouseScroll, this);
	pMouseListener->onMouseUp = CC_CALLBACK_1(WorldScene::onMouseUp, this);
	pMouseListener->onMouseMove = CC_CALLBACK_1(WorldScene::onMouseMove, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pMouseListener, this);
	
	const auto pFocusedListener = EventListenerCustom::create(GLViewImpl::EVENT_WINDOW_FOCUSED, CC_CALLBACK_1(WorldScene::onWndFocused, this));
	const auto pLostFocusedListener = EventListenerCustom::create(GLViewImpl::EVENT_WINDOW_UNFOCUSED, CC_CALLBACK_1(WorldScene::onWndLostFocused, this));
	const auto pResizedListener = EventListenerCustom::create(GLViewImpl::EVENT_WINDOW_RESIZED, CC_CALLBACK_1(WorldScene::onWndResized, this));
	const auto pCursorEnterListener = EventListenerCustom::create(GLViewImpl::EVENT_CURSOR_ENTER, CC_CALLBACK_1(WorldScene::onWndCursorEnter, this));
	const auto pCursorLeaveListener = EventListenerCustom::create(GLViewImpl::EVENT_CURSOR_LEAVE, CC_CALLBACK_1(WorldScene::onWndCursorLeave, this));

	_eventDispatcher->addEventListenerWithSceneGraphPriority(pFocusedListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pLostFocusedListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pResizedListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pCursorEnterListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pCursorLeaveListener, this);
}



void WorldScene::InitLayers() {
	m_pUILayer = UILayer::create();
	this->addChild(m_pUILayer, 1000);

	m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);
	m_pGridLayer->setAnchorPoint(Vec2::ZERO);
	m_pGridLayer->setVisible(false);
	this->addChild(m_pGridLayer, 1001);
}


void WorldScene::update(float dt) {
	// 델타타임(float dt)은 long long 타입의 마이크로초단위 델타값을 float 타입으로, 단위 및 타입 변환을 수행한 것이므로
	// 다시 복구 변환가능함. 코코스엔진의 아래 코드 참고할 것
	// Director::drawScene() -> Director::calculateDeltaTime()
	TimeSpan tsdt = Int64(dt * 1'000'000);

	updateCLI(dt);
	updateTime(dt);
	updateNet(dt);
	updateScene(dt);
}

void WorldScene::updateScene(float dt) {
	
	if (m_pRunningScene)
		m_pRunningScene->update(dt);

	if (m_pUILayer)
		m_pUILayer->update(dt);

	// 초기 세팅 안된 상태거나, 다른 상태로 전환이 예약된 경우
	if (m_pRunningScene == nullptr || m_pRunningScene->getType() != m_eReservedScene) {
		changeScene(m_eReservedScene);
	}

}

void WorldScene::updateNet(float dt) {
	Core::Net->pollNetEvents();
}

void WorldScene::updateTime(float dt) {
	Core::Contents.TimeManager->updateAppTime();
	Core::Contents.TimeManager->updateServerTime();
}

void WorldScene::updateCLI(float dt) {
	Core::CLIThread->ProcessInputs();
}

void WorldScene::onWndMessageReceived(int code, WPARAM wParam, LPARAM lParam) {
	const char* codeName = WndMessage::GetName(code);
	
}

void WorldScene::onWndFocused(SGEventCustom* custom) {
}

void WorldScene::onWndLostFocused(SGEventCustom* custom) {
}


void WorldScene::onWndResized(SGEventCustom* custom) {
	SGSize size = _director->getOpenGLView()->getFrameSize();
}

void WorldScene::onWndCursorEnter(SGEventCustom* custom) {
}

void WorldScene::onWndCursorLeave(SGEventCustom* custom) {
	const SGVec2 leavePos = Win32Helper::getCursorPos();
	EventMouse* pEventMouse = dbg_new EventMouse(EventMouse::MouseEventType::MOUSE_MOVE);
	pEventMouse->setCursorPosition(leavePos.x, leavePos.y);

	if (m_pUILayer)
		m_pUILayer->onMouseMove(pEventMouse);

	CC_SAFE_DELETE(pEventMouse);
}

void WorldScene::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) const {

	if (keyCode == EventKeyboard::KeyCode::KEY_F1)
		Global::Get()->toggleDrawBodyBoundingBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F2)
		Global::Get()->toggleDrawThicknessBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F3)
		Global::Get()->toggleDrawAttackBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F4)
		Global::Get()->toggleDrawEffect();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F5)
		Global::Get()->toggleDrawUIStatic();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F6) {

		// TODO: 게임 해상도 변경 기능 구현
		// UI의 경우 대대적인 코드 수정인 필요하다.
		// 내가 m_UISize라는 변수를 추가해서 게임 해상도에 맞게 UI 해상도를 변경시켜주도록 구현했는데
		// Cocos2d-x에서 자체적으로 관리하는 해상도(GLViewImpl의 DesignResolution)에
		// 맞게 자동으로 UI 해상도가 변경되도록 하기 위해서는 2가지 방법이 우선 떠올랐다.
		// 
		// 1. 현재 구현에서 UI 해상도를 게임 해상도에 맞게 자동으로 변경시키는 방법
		// 현재 내가 구현한 UI 프레임워크는 처음 UI컨트롤을 생성할 때 _contentSize를 해상도에 맞게 변경시켜주는데
		// UI의 원본 크기 정보를 저장할 변수를 하나더 추가해놓고 _contentSize를 해상도에 맞게 다시 재설정해주는 방법이 있을 수 있겠다.
		// 이에 따라 m_UISize를 또 변경해줘야함 코드수정을 별로하지 않고 현재 구현한 기능을 개선가능하다.
		//
		// 2. 현재 구현 방법을 폐기하고 새롭게 UI 해상도 기능 구현
		// m_UISize를 사용하지 않고 기존 노드 변수인 _contentSize와 _scale을 활용해서 UI가 "해상도에 맞게" 보이도록 해줘야함.
		// 코드를 좀 많이 수정해줘야할 것 같다.

		Core::App->SetDesignResolutionSize(640.0f, 480.0f);
		Core::App->SetFrameSize(960.0f, 720.0f);
	} else if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
		if (m_pGridLayer == nullptr) {
			return;
		}

		const bool bDisplaying = Director::getInstance()->isDisplayStats();
		Director::getInstance()->setDisplayStats(!bDisplaying);
		m_pGridLayer->setVisible(!m_pGridLayer->isVisible());
	}

	if (m_pRunningScene)
		m_pRunningScene->onKeyPressed(keyCode, event);

	if (m_pUILayer)
		m_pUILayer->onKeyPressed(keyCode, event);

}

void WorldScene::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) const {
	if (m_pRunningScene)
		m_pRunningScene->onKeyReleased(keyCode, event);

	if (m_pUILayer)
		m_pUILayer->onKeyReleased(keyCode, event);
}

void WorldScene::onMouseMove(SGEventMouse* mouseEvent) const {
	if (m_pRunningScene)
		m_pRunningScene->onMouseMove(mouseEvent);

	if (m_pUILayer)
		m_pUILayer->onMouseMove(mouseEvent);
}

void WorldScene::onMouseDown(SGEventMouse* mouseEvent) const {
	if (m_pRunningScene)
		m_pRunningScene->onMouseDown(mouseEvent);

	if (m_pUILayer)
		m_pUILayer->onMouseDown(mouseEvent);
}

void WorldScene::onMouseUp(SGEventMouse* mouseEvent) const {
	if (m_pRunningScene)
		m_pRunningScene->onMouseUp(mouseEvent);

	if (m_pUILayer)
		m_pUILayer->onMouseUp(mouseEvent);

	Core::Contents.UIManager->dragEnd();
}

void WorldScene::onMouseScroll(SGEventMouse* mouseEvent) const {
	if (m_pRunningScene)
		m_pRunningScene->onMouseScroll(mouseEvent);

	if (m_pUILayer)
		m_pUILayer->onMouseScroll(mouseEvent);
}

void WorldScene::onExit() {
	// ======================================================
	// 리소스 정리
	// ======================================================

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
	FinalizeNetLogger();
	Winsock::Finalize();
}


void WorldScene::reserveScene(SceneType_t sceneType) {
	m_eReservedScene = sceneType;
}



void WorldScene::changeScene(SceneType_t sceneType) {
	_LogDebug_("-- 씬변경 시작");

	if (m_pRunningScene) {
		this->removeChild(m_pRunningScene);
	}

	
	// 씬전환 시 UI 리소스 모두 해제
	m_pUILayer->clearUnload();
	m_pRunningScene = createScene(sceneType);
	this->addChild(m_pRunningScene);
	_LogDebug_("-- 씬전환 완료");

}

void WorldScene::terminate() {
	// 이렇게 종료하면 게임엔진의 메모리릭이 간혹 다다다닥 뜨는데.. 윈도우 타이틀바로 종료하면 깔끔하게 정리잘됨.
	// Director::getInstance()->end();

	// 코코스 게임엔진 소스코드의 Application::run() 함수를보면 GLViewImpl::windowShouldClose로 윈도우가 닫혔는지 체크하는
	// 코드가 있던데.. 이걸 true로 변경해주면 될 듯?
	// 기존의 GLViewImpl::end() 는 메인윈도우를 완전히 해제해버려서 glview->isOpenGLReady() 조건이 만족하지 못해서
	// 리소스 정리를 안해준다. 그래서 따로 close 함수를 하나 추가함
	Director::getInstance()->getOpenGLView()->close();
}

SceneBase* WorldScene::createScene(SceneType_t sceneType) {
	SceneBase* pCreatedScene = nullptr;

	switch (sceneType) {
	case SceneType::Login:				pCreatedScene = SGLoginScene::create(); break;
	case SceneType::ChannelSelect:		pCreatedScene = SGChannelSelectScene::create(); break;
	case SceneType::Game:				pCreatedScene = SGGameScene::create(); break;
	default: DebugAssertMsg(false, "[SGWorldScene] 이상한 씬 타입입니다."); return nullptr;
	}

	// 씬을 생성하면 생성자에서 디폴트 카메라를 생성하고 자식으로 추가해버리는데
	// 월드 씬에서만 디폴트 카메라가 필요하므로 자식 씬에서는 카메라가 불필요하다.
	// 삭제하는 이유는 월드씬에서 부착한 이벤트들(마우스, 키보드 같은)이 카메라를 기준으로 실행되는데
	// 카메라가 여러개가 되면 이벤트도 여러번 호출되기때문에 중복 이벤트 실행 방지를 위해서이다.
	// 
	// 자세하게 이해하고 싶으면 아래 코드 참고
	// 1. void EventDispatcher::dispatchTouchEventToListeners
	// 2. void Camera::setScene
	// 3. SceneBase::removeAllChildren

	return pCreatedScene;
}

UILayer* WorldScene::getUILayer() const {
	DebugAssertMsg(m_pUILayer, "UI 레이어는 무조건 게임내내 생성되어있어야 합니다.");
	return m_pUILayer;
}
