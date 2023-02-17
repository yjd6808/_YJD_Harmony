/*
 * 작성자 : 윤정도
 *
 * Cocos2d-x 자체는 다 쓰레드 세이프 하지 않음
 *
 */

#include "Tutturu.h"
#include "SGGameScene.h"

#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGHostPlayer.h>
#include <SteinsGate/Client/SGActorBox.h>
#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGWorldScene.h>

USING_NS_CC;


SGGameScene::SGGameScene()
	: SGSceneBase()
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
        SGWorldScene::get()->reserveScene(SceneType::Login);
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
    if (!SGSceneBase::init())
        return false;

    m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);
    m_pGridLayer->setAnchorPoint(Vec2::ZERO);
    m_pGridLayer->setVisible(false);
    m_pGridLayer->retain();
    

    cmdEnterMap();
    cmdLoadChar();

    return true;
}

SGGameScene::~SGGameScene() {
    CC_SAFE_RELEASE(m_pGridLayer);
}

void SGGameScene::cmdLoadChar() {
    SGCharacterInfo info;
    SGDataManager* pConfig = SGDataManager::get();
    SGCharInfo* pBaseInfo = pConfig->getCharInfo(CharType::Gunner);

    for (int i = 0; i < VisualType::Max; ++i) {
        info.VisualInfo.NpkIndex[i] = pBaseInfo->DefaultVisualNpkIndex[i];
        info.VisualInfo.ImgIndex[i] = pBaseInfo->DefaultVisualImgIndex[i];
    }
    SGHostPlayer* pPlayer = SGHostPlayer::get();
    SGCharacter* pCharacter = SGActorBox::get()->createCharacterOnMap(CharType::Gunner, 300, 250, info);

    pPlayer->setCharacter(pCharacter);
    pPlayer->initActionManager();
    pPlayer->initController();
    pPlayer->runBaseAction(BaseAction::Idle);
}

void SGGameScene::cmdEnterMap() {
    SGHostPlayer* pHost = SGHostPlayer::get();
    SGActorBox* pActorBox = SGActorBox::get();

    if (m_pMapLayer) {
        m_pMapLayer->release();
    }
    
    m_pMapLayer = SGMapLayer::create();
    m_pMapLayer->setAnchorPoint(Vec2::ZERO);

    pActorBox->init(m_pMapLayer);                   // 맵 레이어 초기화
    pHost->setMapLayer(m_pMapLayer);
    
    m_pMapLayer->addChild(m_pGridLayer, 1);
    m_pMapLayer->loadMap(1);
    this->addChild(m_pMapLayer);
}

void SGGameScene::onExit() {
    //removeAllChildren();
    SGActorBox::get()->clearAll();
    //SGSceneBase::onExit();
}
