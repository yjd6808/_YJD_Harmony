#include <TF/Scenes/RoomScene.h>
#include <Common/Command.h>

Scene* RoomScene::createScene()
{
    return RoomScene::create();
}

bool RoomScene::init()
{
    if ( !Scene::init() )
        return false;

    m_pRoomPlayer = RoomLayer::create();
    m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);

	this->addChild(m_pRoomPlayer, 0);
    this->addChild(m_pGridLayer, 1);
    return true;
}

void RoomScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    CCLOG("%d 커맨드 수신", cmd->GetCommand());
    switch (cmd->GetCommand()) {
    //case LOAD_CHARACTER_INFO_ACK: m_pCharacterSelectLayer->CmdLoadCharacterInfoAck(cmd); break;
    }
}


