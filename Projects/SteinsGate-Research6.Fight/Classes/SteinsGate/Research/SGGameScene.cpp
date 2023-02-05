/*
 * 작성자 : 윤정도
 *
 * Cocos2d-x 자체는 다 쓰레드 세이프 하지 않음
 *
 */

#include "Tutturu.h"
#include "SGGameScene.h"

#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGHostPlayer.h>
#include <SteinsGate/Research/SGActorBox.h>
#include <SteinsGate/Research/SGGlobal.h>
#include <SteinsGate/Research/SGWorldScene.h>

USING_NS_CC;


SGGameScene::SGGameScene()
	: SGSceneBase(SceneType::Game)
	, m_pMapLayer(nullptr)
	, m_pGridLayer(nullptr)
{}

void SGGameScene::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {

    if (m_pMapLayer)
        m_pMapLayer->onKeyPressed(keyCode, event);

    if (keyCode == EventKeyboard::KeyCode::KEY_F1)
        SGGlobal::get()->toggleDrawBodyBoundingBox();
    else if (keyCode == EventKeyboard::KeyCode::KEY_F2)
        SGGlobal::get()->toggleDrawThicknessBox();
    else if (keyCode == EventKeyboard::KeyCode::KEY_F3)
        SGGlobal::get()->toggleDrawAttackBox();
    else if (keyCode == EventKeyboard::KeyCode::KEY_F4)
        SGGlobal::get()->toggleDrawEffect();
    else if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
        SGHostPlayer::get()->getCharacter()->cleanUpImmediate();
        SGActorBox::get()->clearAll();
        SGWorldScene::get()->changeScene(SceneType::Login);
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
    if (!Scene::init())
        return false;

    cmdLoadChar();
    cmdEnterMap();

    return true;
}

void SGGameScene::cmdLoadChar() {
    SGCharacterInfo info;
    SGDataManager* pConfig = SGDataManager::get();
    SGCharBaseInfo* pBaseInfo = pConfig->getCharBaseInfo(CharType::Gunner);

    for (int i = 0; i < VisualType::Max; ++i) {
        info.VisualInfo.NpkIndex[i] = pBaseInfo->DefaultVisualNpkIndex[i];
        info.VisualInfo.ImgIndex[i] = pBaseInfo->DefaultVisualImgIndex[i];
    }
    SGCharacter* pCharacter = SGActorBox::get()->createCharacter(CharType::Gunner, 300, 250, info);
    SGHostPlayer* pPlayer = SGHostPlayer::get();
    pPlayer->setCharacter(pCharacter);
    pPlayer->initActionManager();
    pPlayer->initController();
    pPlayer->runBaseAction(BaseAction::Idle);
}

void SGGameScene::cmdEnterMap() {
    SGHostPlayer* pHost = SGHostPlayer::get();
    if (m_pMapLayer) {
        m_pMapLayer->release();
        pHost->setMapLayer(nullptr);
        m_pMapLayer->cleanup();
    }
    
    m_pMapLayer = SGMapLayer::create(1);
    m_pMapLayer->setAnchorPoint(Vec2::ZERO);
    m_pMapLayer->setScale(SGDataManager::get()->getClientInfo()->GameScale);
    this->addChild(m_pMapLayer);
    
    pHost->setMapLayer(m_pMapLayer);
    m_pMapLayer->addChild(pHost->getCharacter());
    pHost->getCharacter()->setMapLayer(m_pMapLayer);

    SGActorBox::get()->registerCharacter(pHost->getCharacter());

    m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);
    m_pGridLayer->setAnchorPoint(Vec2::ZERO);
    m_pGridLayer->setVisible(false);
    m_pMapLayer->addChild(m_pGridLayer, 1);
    SGActorBox::get()->updateZOrder();
}
