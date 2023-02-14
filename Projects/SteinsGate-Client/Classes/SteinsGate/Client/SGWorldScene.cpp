/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 9:51:57 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGWorldScene.h"

#include <SteinsGate/Client/SGActorBox.h>
#include <SteinsGate/Client/SGLoginScene.h>
#include <SteinsGate/Client/SGGameScene.h>
#include <SteinsGate/Client/SGChannelSelectScene.h>

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
{}
SGWorldScene::~SGWorldScene() {}


bool SGWorldScene::init() {

	if (!Scene::init())
		return false;

	initEventListeners();
	changeScene(SceneType::Login);
	scheduleUpdate();

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



void SGWorldScene::update(float dt) {
	updateScene(dt);
}



void SGWorldScene::updateScene(float dt) {

	if (m_pRunningScene == nullptr) {
		return;
	}

	m_pRunningScene->update(dt);

	if (m_pRunningScene->getType() != m_eReservedScene) {
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
	CCLOG("%.f %.f", mouseEvent->getCursorX(), mouseEvent->getCursorY());
}

void SGWorldScene::onMouseDown(SGEventMouse* mouseEvent) {
	
}

void SGWorldScene::onMouseUp(SGEventMouse* mouseEvent) {
	
}

void SGWorldScene::onMouseScroll(SGEventMouse* mouseEvent) {
	
}


void SGWorldScene::reserveScene(SceneType_t sceneType) {
	m_eReservedScene = sceneType;
}

void SGWorldScene::changeScene(SceneType_t sceneType) {
	if (m_pRunningScene) {
		this->removeChild(m_pRunningScene);
	}

	switch (sceneType) {
	case SceneType::Login: m_pRunningScene = SGLoginScene::create(); break;
	case SceneType::ChannelSelect: m_pRunningScene = SGChannelSelectScene::create(); break;
	case SceneType::Game: m_pRunningScene = SGGameScene::create(); break;
	default: DebugAssertMsg(false, "[SGWorldScene] 이상한 씬 타입입니다."); return;
	}

	this->addChild(m_pRunningScene);
}

enum UIControlType
{
	Button,
	ToggleButton,
	Image,
	Label,
	TextBox,
	ScrollBar,
	None
};

struct UIElementInfo
{
	UIControlType ControlType;
	int Code;
	SGString Name;
	float X;
	float Y;
	float Width;
	float Height;
	int ZOrder;
	int Tag;
	int NpkIndex;
	int ImgIndex;
};

// 토글 버튼도 동일
struct UIButtonInfo : UIElementInfo
{
	int Normal;			// ElementIndex
	int OnMouseOver;
	int Pressed;
	int Disabled;
};

struct UIGroupInfo
{
	int Code;
	float X;
	float Y;
	float Width;
	float Height;
	SGString Name;
	SGVector<int> vElements;
};

struct UIElement;
struct UIGroup
{
	bool Loaded;
	SGVector<UIElement*> vElements;
	SGNode* NodeBase;
};

struct UIElement
{
	virtual void Load() = 0;

	UIElementInfo* m_pInfo;
};

struct UIButtonNode : cocos2d::ui::Button
{
	static UIButtonNode* create(SGFrameTexture* Normal,
		SGFrameTexture* Over,
		SGFrameTexture* Pressed,
		SGFrameTexture* Disabled) {}
};

struct UIButton : UIElement
{
	void Load() override {
		
	}

	UIButtonNode* CreateNode() {
		return UIButtonNode::create(Normal, Over, Pressed, Disabled);
	}

	SGFrameTexture* Normal;
	SGFrameTexture* Over;
	SGFrameTexture* Pressed;
	SGFrameTexture* Disabled;
};


struct UIManager
{
	
};

struct UICacheShop
{
	
};
