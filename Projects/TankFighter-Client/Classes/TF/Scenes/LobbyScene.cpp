#include <TF/Scenes/LobbyScene.h>
#include <Common/Command.h>

Scene* LobbyScene::createScene()
{
    return LobbyScene::create();
}

bool LobbyScene::init()
{
    if ( !Scene::init() )
        return false;

    m_pLobbyLayer = LobbyLayer::create();
    m_pGridLayer = GridLayer::create(50, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);

	this->addChild(m_pLobbyLayer, 0);
    this->addChild(m_pGridLayer, 1);
    return true;
}

void LobbyScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    CCLOG("%d 커맨드 수신", cmd->GetCommand());
    switch (cmd->GetCommand()) {
    case UPDATE_CHARACTER_INFO_ACK:  m_pLobbyLayer->CmdUpdateCharacterInfoAck(cmd); break;
    case UPDATE_ROOMLIST_ACK:        m_pLobbyLayer->CmdUpdateRoomListAck(cmd); break;
    case UPDATE_FRIENDLIST_ACK:      m_pLobbyLayer->CmdUpdateFriendListAck(cmd); break;
    case CREATE_ROOM_ACK:            m_pLobbyLayer->; break;
    }
}


