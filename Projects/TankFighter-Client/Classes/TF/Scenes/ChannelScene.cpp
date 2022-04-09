/*
 * 작성자 : 윤정도
 */

#include <TF/Scenes/ChannelScene.h>
#include <Common/Command.h>

Scene* ChannelScene::createScene()
{
    return ChannelScene::create();
}

bool ChannelScene::init()
{
    if ( !SynchronizedScene::init())
        return false;

    m_pChannelLayer = ChannelLayer::create();
	this->addChild(m_pChannelLayer, 0);
    return true;
}

void ChannelScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    CCLOG("%d 커맨드 수신 (%s)", cmd->GetCommand(), "ChannelScene");
    SynchronizedScene::SynchronizedOnReceived(cmd);
    
    switch (cmd->GetCommand()) {
    case LOAD_CHANNEL_INFO_ACK: m_pChannelLayer->CmdLoadChannelInfoAck(cmd);    break;
    case SELECT_CHANNEL_ACK:    m_pChannelLayer->CmdSelectChannelAck(cmd);      break;
    }
}


