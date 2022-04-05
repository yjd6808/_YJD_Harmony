#include <TF/Scenes/ChannelScene.h>
#include <Common/Command.h>

Scene* ChannelScene::createScene()
{
    return ChannelScene::create();
}

bool ChannelScene::init()
{
    if ( !Scene::init() )
        return false;

    m_pChannelLayer = ChannelLayer::create();
    m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);

	this->addChild(m_pChannelLayer, 0);
    this->addChild(m_pGridLayer, 1);
    return true;
}

void ChannelScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    CCLOG("%d 커맨드 수신", cmd->GetCommand());
    switch (cmd->GetCommand()) {
    case LOAD_CHANNEL_INFO_ACK: m_pChannelLayer->CmdLoadChannelInfoAck(cmd);    break;
    case SELECT_CHANNEL_ACK:    m_pChannelLayer->CmdSelectChannelAck(cmd);      break;
    }
}


