/*
 * 작성자 : 윤정도
 */

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

    m_pRoomLayer = RoomLayer::create();
	this->addChild(m_pRoomLayer, 0);
    return true;
}

void RoomScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    CCLOG("%d 커맨드 수신 (%s)", cmd->GetCommand(), "RoomScene");
    SynchronizedScene::SynchronizedOnReceived(cmd);

    switch (cmd->GetCommand()) {
	case LOAD_ROOM_INFO_ACK:     m_pRoomLayer->CmdLoadRoomInfoAck(cmd);     break;
    case UPDATE_ROOMINFO_ACK:    m_pRoomLayer->CmdUpdateRoomInfoAck(cmd);   break;
    case ROOM_LEAVE_ACK:         m_pRoomLayer->CmdRoomLeaveAck(cmd);        break;
    case ROOM_GAME_START_ACK:    m_pRoomLayer->CmdRoomGameStartAck(cmd);    break;
    }
}


