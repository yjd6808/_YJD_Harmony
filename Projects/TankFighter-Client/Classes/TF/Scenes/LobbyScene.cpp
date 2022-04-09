/*
 * 작성자 : 윤정도
 */

#include <TF/Scenes/LobbyScene.h>
#include <Common/Command.h>

Scene* LobbyScene::createScene()
{
    return LobbyScene::create();
}

bool LobbyScene::init()
{
    if ( !SynchronizedScene::init() )
        return false;

    m_pLobbyLayer = LobbyLayer::create();
	this->addChild(m_pLobbyLayer, 0);
    return true;
}

void LobbyScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    CCLOG("%d 커맨드 수신 (%s)", cmd->GetCommand(), "LobbyScene");
    SynchronizedScene::SynchronizedOnReceived(cmd);

    switch (cmd->GetCommand()) {
    case UPDATE_CHARACTER_INFO_ACK:  m_pLobbyLayer->CmdUpdateCharacterInfoAck(cmd); break;
    case UPDATE_ROOMLIST_ACK:        m_pLobbyLayer->CmdUpdateRoomListAck(cmd); break;
    case UPDATE_FRIENDLIST_ACK:      m_pLobbyLayer->CmdUpdateFriendListAck(cmd); break;
    case CREATE_ROOM_ACK:            m_pLobbyLayer->CmdCreateRoomAck(cmd); break;
    case JOIN_ROOM_ACK:              m_pLobbyLayer->CmdJoinRoomAck(cmd); break;
    case ADD_FRIEND_ACK:             m_pLobbyLayer->CmdAddFriendAck(cmd); break;
    case ADD_FRIEND_REQUEST_SYN:     m_pLobbyLayer->CmdAddFriendRequestSyn(cmd); break;
    }
}


