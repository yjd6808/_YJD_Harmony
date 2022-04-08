/*
 * 작성자 : 윤정도
 */

#include <TF/Scenes/LoginScene.h>
#include <Common/Command.h>

Scene* LoginScene::createScene()
{
    return LoginScene::create();
}

bool LoginScene::init()
{
    if ( !Scene::init() )
        return false;

    m_pLoginLayer = LoginLayer::create();
    m_pLoginLayer->setAnchorPoint(Vec2::ZERO);
    m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);
    m_pGridLayer->setAnchorPoint(Vec2::ZERO);

	this->addChild(m_pLoginLayer, 0);
    this->addChild(m_pGridLayer, 1);
    return true;
}

void LoginScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    CCLOG("%d 커맨드 수신 (%s)", cmd->GetCommand(), "LoginScene");
    SynchronizedScene::SynchronizedOnReceived(cmd);

    switch (cmd->GetCommand()) {
    case LOGIN_ACK: m_pLoginLayer->CmdLoginAck(cmd); break;
    case REGISTER_ACK: m_pLoginLayer->CmdRegisterAck(cmd); break;
    }
}


