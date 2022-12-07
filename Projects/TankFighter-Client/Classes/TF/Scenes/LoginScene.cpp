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
    if ( !SynchronizedScene::init())
        return false;

    m_pLoginLayer = LoginLayer::create();
    m_pLoginLayer->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pLoginLayer, 0);
    return true;
}

bool LoginScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    if (!SynchronizedScene::SynchronizedOnReceived(cmd)) {
        return false;
    }

    CCLOG("%d 커맨드 수신 (%s)", cmd->GetCommand(), "LoginScene");

    switch (cmd->GetCommand()) {
    case LOGIN_ACK: m_pLoginLayer->CmdLoginAck(cmd); break;
    case REGISTER_ACK: m_pLoginLayer->CmdRegisterAck(cmd); break;
    }

    return true;
}


