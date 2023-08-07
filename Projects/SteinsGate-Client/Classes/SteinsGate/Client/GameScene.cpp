/*
 * 작성자 : 윤정도
 *
 * Cocos2d-x 자체는 다 쓰레드 세이프 하지 않음
 *
 */

#include "Tutturu.h"
#include "GameScene.h"

#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/ActorBox.h>
#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/WorldScene.h>

USING_NS_CC;


SGGameScene::SGGameScene()
	: SceneBase()
	, m_pMapLayer(nullptr)
	, m_pGridLayer(nullptr)
{}

SGGameScene::~SGGameScene() {
    CC_SAFE_RELEASE(m_pGridLayer);
}

void SGGameScene::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {

    if (m_pMapLayer)
        m_pMapLayer->onKeyPressed(keyCode, event);

   
    if (keyCode == EventKeyboard::KeyCode::KEY_F8) {
        WorldScene::get()->reserveScene(SceneType::Login);
    } else if (keyCode == EventKeyboard::KeyCode::KEY_F7) {
		WorldScene::get()->reserveScene(SceneType::ChannelSelect);
    } else if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
        if (m_pGridLayer == nullptr) {
            return;
        }

        Director::getInstance()->setDisplayStats(!Director::getInstance()->isDisplayStats());
        m_pGridLayer->setVisible(!m_pGridLayer->isVisible());
    }
}

void SGGameScene::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
    if (m_pMapLayer)
        m_pMapLayer->onKeyReleased(keyCode, event);
}

void SGGameScene::update(float dt) {
    if (m_pMapLayer)
        m_pMapLayer->update(dt);
}

bool SGGameScene::init()
{
    if (!SceneBase::init())
        return false;

    m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);
    m_pGridLayer->setAnchorPoint(Vec2::ZERO);
    m_pGridLayer->setVisible(false);
    m_pGridLayer->retain();
    //this->addChild(m_pGridLayer);
    
    m_pMapLayer = MapLayer::create();
    m_pMapLayer->setAnchorPoint(Vec2::ZERO);
    m_pMapLayer->addChild(m_pGridLayer, 1);
    this->addChild(m_pMapLayer);

    CoreActorBox_v->init(m_pMapLayer);                   // 맵 레이어 초기화

    cmdEnterMap();
    cmdLoadChar();

    return true;
}

void SGGameScene::cmdLoadChar() {

    CorePlayer_v->init();
    CorePlayer_v->setPositionRealCenter(300, 200);
    CorePlayer_v->setMapLayer(m_pMapLayer);
    CorePlayer_v->setCleanUpFlag(Actor::CF_ReleaseActorSprite);

    CoreActorBox_v->registerPlayerOnMap(CorePlayer_v);
    m_pMapLayer->getCamera()->setFollowTarget(CorePlayer_v);
}

void SGGameScene::cmdEnterMap() {
    m_pMapLayer->loadMap(10000);
}

void SGGameScene::onExit() {
    CorePlayer_v->cleanUp();
    ActorBox::Get()->clearAll();
    SceneBase::onExit();
}
