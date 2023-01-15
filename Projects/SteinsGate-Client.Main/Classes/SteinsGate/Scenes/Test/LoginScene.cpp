/*
 * 작성자 : 윤정도
 */

#include <SteinsGate/Scenes/Test/LoginScene.h>



using namespace cocos2d;

Scene* LoginScene::createScene()
{
    return LoginScene::create();
}

void LoginScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    switch (keyCode) {
    case EventKeyboard::KeyCode::KEY_ESCAPE:
        Director::getInstance()->setDisplayStats(!Director::getInstance()->isDisplayStats());
        m_pGridLayer->setVisible(!m_pGridLayer->isVisible());
        break;
    }
}

bool LoginScene::init()
{
    EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(LoginScene::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    m_pGridLayer = GridLayer::create(50, Color4F(Color3B::GREEN, 0.5f), GridLayer::GridEvent::ShowGridAndMousePoint);
    m_pGridLayer->setAnchorPoint(Vec2::ZERO);
    m_pGridLayer->setVisible(false);
    this->addChild(m_pGridLayer, 0);

    m_pLoginLayer = LoginLayer::create();
    m_pLoginLayer->setAnchorPoint(Vec2::ZERO);
    this->addChild(m_pLoginLayer, 1);
    return true;
}
