/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:33:05 PM
 * =====================
 *
 */

#include "Pch.h"
#include "BaseScene.h"

#include <TF/Client/Util/Color.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_CCUI;

BaseScene::BaseScene()
	: m_pUILayer(nullptr)
	, m_pGridLayer(nullptr)
	, m_pNetInfo(nullptr)
{}

bool BaseScene::init() {

	if (!Scene::init()) {
		return false;
	}

	m_pUILayer = Layer::create();
	m_pUILayer->setContentSize(Director::getInstance()->getWinSize());
	m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);
	m_pGridLayer->setVisible(false);

	m_pNetInfo = Text::create("", Const::Resource::FontName, 13);
	m_pNetInfo->setVisible(false);
	m_pNetInfo->setColor(ColorList::Arcticlime_v);
	m_pNetInfo->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	m_pNetInfo->setPosition({ 0, Const::Window::Height });
	
	this->addChild(m_pUILayer, 0, Const::Layer::ID_UI);
	this->addChild(m_pGridLayer, 1, Const::Layer::ID_Grid);
	this->addChild(m_pNetInfo, 2);

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(BaseScene::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(BaseScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	this->scheduleUpdate();

	return true;

}

void BaseScene::update(float delta) {
	const TimeSpan elapsed = Int64(delta * 1'000'000);

	if (Core::NetCore)
		Core::NetCore->update(elapsed);

	static TimeCounterF s_pingCounter(TimeCounterAttribute::TimeOverReset);
	s_pingCounter.Elapsed += delta;
	if (s_pingCounter.ElapsedMiliSeconds(Const::Ping::Delay)) {
		
	}
}

struct TT
{
	int item1;
	JCore::String item2;
};

static HashMap<EventKeyboard::KeyCode, bool> m_KeyPressedMap;
static LinkedList<TT> code;

void BaseScene::onKeyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

	if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
		code.ForEach([](const TT& tuple) { _LogDebug_("%d:%s", tuple.item1, tuple.item2.Source()); });
		m_KeyPressedMap.Clear();
		_LogDebug_("성공");
		return;
	}

	if (m_KeyPressedMap.Insert(keyCode, true)) {
		code.PushBack({ int(keyCode), "IN" });
	}


	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_ESCAPE:
		Director::getInstance()->setDisplayStats(!Director::getInstance()->isDisplayStats());
		if (m_pGridLayer)
			m_pGridLayer->setVisible(!m_pGridLayer->isVisible());

		if (m_pNetInfo)
			m_pNetInfo->setVisible(!m_pNetInfo->isVisible());
		break;
	}
}

void BaseScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (m_KeyPressedMap.Remove(keyCode)) {
		code.PushBack({ int(keyCode), "OUT" });
	}
}
