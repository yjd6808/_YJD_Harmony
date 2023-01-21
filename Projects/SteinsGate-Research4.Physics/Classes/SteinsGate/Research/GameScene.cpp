/*
 * 작성자 : 윤정도
 */

#include "GameScene.h"

using namespace cocos2d;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

void GameScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    switch (keyCode) {
    case EventKeyboard::KeyCode::KEY_ESCAPE:
        Director::getInstance()->setDisplayStats(!Director::getInstance()->isDisplayStats());
        m_pGridLayer->setVisible(!m_pGridLayer->isVisible());
        break;
    }
}

bool GameScene::init()
{
    m_pWorldLayer = WorldLayer::create();
    this->addChild(m_pWorldLayer);


    m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);
    m_pGridLayer->setAnchorPoint(Vec2::ZERO);
    m_pGridLayer->setVisible(false);
    this->addChild(m_pGridLayer, 1);


    EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    return true;
}
