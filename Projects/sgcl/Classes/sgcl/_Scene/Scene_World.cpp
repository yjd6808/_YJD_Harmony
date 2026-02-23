/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 9:51:57 AM
 * =====================
 *
 */


#include "Core.h"
#include "Scene_World.h"

#include <sg/_API/sgapiBase.h>
#include <sg/_Util/DescLoaderMgr.h>

#include <sgcl/_Layer/Layer_Grid.h>
#include <sgcl/_Core/AppConfig_Props.h>

#include <sgcl/_Scene/Scene_Login.h>
#include <sgcl/_Scene/Scene_Game.h>
#include <sgcl/_Scene/Scene_ChannelSelect.h>
#include <sgcl/_Layer/Layer_UI.h>
#include <sgcl/_Net/NetCore.h>
#include <sgcl/Contents.h>
#include <sgcl/SteinsGateApp.h>

#include <sgcl/ActorManager.h>
#include <sgcl/HostPlayer.h>
#include <sgcl/ImagePackManager.h>
#include <sgcl/ActorListenerManager.h>
#include <sgcl/UIManager.h>
#include <sgcl/FontManager.h>
#include <sgcl/WndMessage.h>
#include <sgcl/Win32Helper.h>


USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
WorldScene* WorldScene::Get()
{
	static WorldScene* scene;

	if (scene == nullptr)
	{
		scene = dbg_new WorldScene();

		if (scene && scene->init())
		{
			scene->autorelease();
			return scene;
		}

		CC_SAFE_DELETE(scene);
		return nullptr;
	}

	return scene;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapLayer* WorldScene::GetMap()
{
	jc_assert_msg(pRunningScene_ && pRunningScene_->GetType() == SceneType::Game, "게임 씬이 실행중이지 않을때 맵을 가져올려고 시도했습니다.");
	return static_cast<SGGameScene*>(pRunningScene_)->GetMap();
}

//////////////////////////////////////////////////////////////////////////////////////////
MimicCamera* WorldScene::GetCamera()
{
	return GetMap()->GetCamera();
}

//////////////////////////////////////////////////////////////////////////////////////////
WorldScene::WorldScene()
: pRunningScene_(nullptr)
, reservedScene_(SceneType::Login)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
WorldScene::~WorldScene()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool WorldScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	InitEventListeners();
	InitLayers();
	ReserveScene(SceneType::Login);
	scheduleUpdate(); // 즉시 update 1회 호출함

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::InitEventListeners()
{
	const auto pKeyboardListener = EventListenerKeyboard::create();
	pKeyboardListener->onKeyPressed = CC_CALLBACK_2(WorldScene::OnKeyPressed, this);
	pKeyboardListener->onKeyReleased = CC_CALLBACK_2(WorldScene::OnKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pKeyboardListener, this);

	const auto pMouseListener = EventListenerMouse::create();
	pMouseListener->onMouseDown = CC_CALLBACK_1(WorldScene::OnMouseDown, this);
	pMouseListener->onMouseScroll = CC_CALLBACK_1(WorldScene::OnMouseScroll, this);
	pMouseListener->onMouseUp = CC_CALLBACK_1(WorldScene::OnMouseUp, this);
	pMouseListener->onMouseMove = CC_CALLBACK_1(WorldScene::OnMouseMove, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pMouseListener, this);

	const auto pFocusedListener = EventListenerCustom::create(GLViewImpl::EVENT_WINDOW_FOCUSED, CC_CALLBACK_1(WorldScene::OnWndFocused, this));
	const auto pLostFocusedListener = EventListenerCustom::create(GLViewImpl::EVENT_WINDOW_UNFOCUSED, CC_CALLBACK_1(WorldScene::OnWndLostFocused, this));
	const auto pResizedListener = EventListenerCustom::create(GLViewImpl::EVENT_WINDOW_RESIZED, CC_CALLBACK_1(WorldScene::OnWndResized, this));
	const auto pCursorEnterListener = EventListenerCustom::create(GLViewImpl::EVENT_CURSOR_ENTER, CC_CALLBACK_1(WorldScene::OnWndCursorEnter, this));
	const auto pCursorLeaveListener = EventListenerCustom::create(GLViewImpl::EVENT_CURSOR_LEAVE, CC_CALLBACK_1(WorldScene::OnWndCursorLeave, this));

	_eventDispatcher->addEventListenerWithSceneGraphPriority(pFocusedListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pLostFocusedListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pResizedListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pCursorEnterListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pCursorLeaveListener, this);
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::InitLayers()
{
	pUILayer_ = UILayer::Create();
	addChild(pUILayer_, 1000);

	pGridLayer_ = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);
	pGridLayer_->setAnchorPoint(Vec2::ZERO);
	pGridLayer_->setVisible(false);
	addChild(pGridLayer_, 1001);
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::update(float _dt)
{
	// 델타타임(float dt)은 long long 타입의 마이크로초단위 델타값을 float 타입으로, 단위 및 타입 변환을 수행한 것이므로
	// 다시 복구 변환가능함. 코코스엔진의 아래 코드 참고할 것
	// Director::drawScene() -> Director::calculateDeltaTime()
	TimeSpan tsdt = _s64(_dt * 1'000'000);

	g_cVM.Go(tsdt);

	UpdateNet(_dt);
	UpdateTime(_dt);
	UpdateScene(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::UpdateScene(float _dt)
{
	if (pRunningScene_)
	{
		pRunningScene_->update(_dt);
	}

	if (pUILayer_)
	{
		pUILayer_->update(_dt);
	}

	// 초기 세팅 안된 상태거나, 다른 상태로 전환이 예약된 경우
	if (pRunningScene_ == nullptr || pRunningScene_->GetType() != reservedScene_)
	{
		ChangeScene(reservedScene_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::UpdateNet(float _dt)
{
	g_cNet.PollNetEvents();
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::UpdateTime(float _dt)
{
	sg::Contents.TimeManager->UpdateAppTime();
	sg::Contents.TimeManager->UpdateServerTime();
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::OnWndMessageReceived(int _code, WPARAM _wParam, LPARAM _lParam)
{
	const char* codeName = WndMessage::GetName(_code);
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::OnWndFocused(cc::EventCustom* _pCustom)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::OnWndLostFocused(cc::EventCustom* _pCustom)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::OnWndResized(cc::EventCustom* _pCustom)
{
	cc::size size = _director->getOpenGLView()->getFrameSize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::OnWndCursorEnter(cc::EventCustom* _pCustom)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::OnWndCursorLeave(cc::EventCustom* _pCustom)
{
	const cc::vec2 leavePos = Win32Helper::GetCursorPos();
	EventMouse* pEventMouse = dbg_new EventMouse(EventMouse::MouseEventType::MOUSE_MOVE);
	pEventMouse->setCursorPosition(leavePos.x, leavePos.y);

	if (pUILayer_)
	{
		pUILayer_->OnMouseMove(pEventMouse);
	}

	CC_SAFE_DELETE(pEventMouse);
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) const
{
	if (_keyCode == EventKeyboard::KeyCode::KEY_F1)
	{
		g_cAppProps.ToggleBool(CONF_PROPS_DRAW_BODY_BOUNDING_BOX);
	}
	else if (_keyCode == EventKeyboard::KeyCode::KEY_F2)
	{
		g_cAppProps.ToggleBool(CONF_PROPS_DRAW_THICKNESS_BOX);
	}
	else if (_keyCode == EventKeyboard::KeyCode::KEY_F3)
	{
		g_cAppProps.ToggleBool(CONF_PROPS_DRAW_ATTACK_BOX);
	}
	else if (_keyCode == EventKeyboard::KeyCode::KEY_F4)
	{
		g_cAppProps.ToggleBool(CONF_PROPS_DRAW_EFFECT);
	}
	else if (_keyCode == EventKeyboard::KeyCode::KEY_F5)
	{
		g_cAppProps.ToggleBool(CONF_PROPS_DRAW_UI_STATIC);
	}
	else if (_keyCode == EventKeyboard::KeyCode::KEY_F6)
	{
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

		g_cApp.SetDesignResolutionSize(640.0f, 480.0f);
		g_cApp.SetFrameSize(960.0f, 720.0f);
	}
	else if (_keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		if (pGridLayer_ == nullptr)
		{
			return;
		}

		const bool displaying = Director::getInstance()->isDisplayStats();
		Director::getInstance()->setDisplayStats(!displaying);
		pGridLayer_->setVisible(!pGridLayer_->isVisible());
	}

	if (pRunningScene_)
	{
		pRunningScene_->OnKeyPressed(_keyCode, _pEvent);
	}

	if (pUILayer_)
	{
		pUILayer_->onKeyPressed(_keyCode, _pEvent);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) const
{
	if (pRunningScene_)
	{
		pRunningScene_->OnKeyReleased(_keyCode, _pEvent);
	}

	if (pUILayer_)
	{
		pUILayer_->onKeyReleased(_keyCode, _pEvent);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::OnMouseMove(cc::EventMouse* _pMouseEvent) const
{
	if (pRunningScene_)
	{
		pRunningScene_->OnMouseMove(_pMouseEvent);
	}

	if (pUILayer_)
	{
		pUILayer_->OnMouseMove(_pMouseEvent);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::OnMouseDown(cc::EventMouse* _pMouseEvent) const
{
	if (pRunningScene_)
	{
		pRunningScene_->OnMouseDown(_pMouseEvent);
	}

	if (pUILayer_)
	{
		pUILayer_->OnMouseDown(_pMouseEvent);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::OnMouseUp(cc::EventMouse* _pMouseEvent) const
{
	if (pRunningScene_)
	{
		pRunningScene_->OnMouseUp(_pMouseEvent);
	}

	if (pUILayer_)
	{
		pUILayer_->OnMouseUp(_pMouseEvent);
	}

	g_cUIMgr.DragEnd();
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::OnMouseScroll(cc::EventMouse* _pMouseEvent) const
{
	if (pRunningScene_)
	{
		pRunningScene_->OnMouseScroll(_pMouseEvent);
	}

	if (pUILayer_)
	{
		pUILayer_->OnMouseScroll(_pMouseEvent);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::onExit()
{
	// ======================================================
	// 리소스 정리
	// ======================================================

	// 삭제전 마지막 발악, 모든 UI 리소스 정리
	pUILayer_->ClearUnload();
	// 강종시 하위 씬들의 onExit을 수동호출해주자.
	Scene::onExit();

	// 자식노드 모두 정리 (onExit에서 제거하는줄 알았는데 아니네; 그냥 재귀 onExit 호출함.. 하..)
	// 씬 정리되기전에 모든 레퍼런스 카운트가 0가 되어야함.
	removeAllChildren();

	FinalizeClientCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	FinalizeJCore();
	Winsock::Finalize();
	sgapiBase::Free();
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::ReserveScene(SceneType_t _sceneType)
{
	reservedScene_ = _sceneType;
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::ChangeScene(SceneType_t _sceneType)
{
	_LogDebug_("-- 씬변경 시작");

	if (pRunningScene_)
	{
		removeChild(pRunningScene_);
	}

	// 씬전환 시 UI 리소스 모두 해제
	pUILayer_->ClearUnload();
	pRunningScene_ = CreateScene(_sceneType);
	addChild(pRunningScene_);
	_LogDebug_("-- 씬전환 완료");
}

//////////////////////////////////////////////////////////////////////////////////////////
void WorldScene::Terminate()
{
	// 이렇게 종료하면 게임엔진의 메모리릭이 간혹 다다다닥 뜨는데.. 윈도우 타이틀바로 종료하면 깔끔하게 정리잘됨.
	// Director::getInstance()->end();

	// 코코스 게임엔진 소스코드의 Application::run() 함수를보면 GLViewImpl::windowShouldClose로 윈도우가 닫혔는지 체크하는
	// 코드가 있던데.. 이걸 true로 변경해주면 될 듯?
	// 기존의 GLViewImpl::end() 는 메인윈도우를 완전히 해제해버려서 glview->isOpenGLReady() 조건이 만족하지 못해서
	// 리소스 정리를 안해준다. 그래서 따로 close 함수를 하나 추가함
	Director::getInstance()->getOpenGLView()->close();
}

//////////////////////////////////////////////////////////////////////////////////////////
SceneBase* WorldScene::CreateScene(SceneType_t _sceneType)
{
	SceneBase* pCreatedScene = nullptr;

	switch (_sceneType)
	{
	case SceneType::Login: pCreatedScene = SGLoginScene::Create();
		break;
	case SceneType::ChannelSelect: pCreatedScene = SGChannelSelectScene::create();
		break;
	case SceneType::Game: pCreatedScene = SGGameScene::create();
		break;
	default: jc_assert_msg(false, "[SGWorldScene] 이상한 씬 타입입니다.");
		return nullptr;
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

//////////////////////////////////////////////////////////////////////////////////////////
UILayer& WorldScene::GetUILayer() const
{
	jc_assert_msg(pUILayer_, "UI 레이어는 무조건 게임내내 생성되어있어야 합니다.");
	return *pUILayer_;
}
