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
    m_pGridLayer = GridLayer::create(50, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);

	this->addChild(m_pLoginLayer, 0);
    this->addChild(m_pGridLayer, 1);
    return true;
}

void LoginScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    CCLOG("%d 커맨드 수신", cmd->GetCommand());
    switch (cmd->GetCommand()) {
    case LOGIN_ACK: m_pLoginLayer->CmdLoginAck(cmd); break;
    case REGISTER_ACK: m_pLoginLayer->CmdRegisterAck(cmd); break;
    }
}


