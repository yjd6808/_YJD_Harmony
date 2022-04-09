/*
 * 작성자 : 윤정도
 */

#include <TF/Scenes/RoomScene.h>
#include <Common/Command.h>

Scene* BattleFieldScene::createScene()
{
    return BattleFieldScene::create();
}

bool BattleFieldScene::init()
{
    if ( !SynchronizedScene::init() )
        return false;

    m_pBattleFieldLayer = BattleFieldLayer::create();
	this->addChild(m_pBattleFieldLayer, 0);
    return true;
}

void BattleFieldScene::SynchronizedOnReceived(ICommand* cmd) {
    CCLOG("%d 커맨드 수신 (%s)", cmd->GetCommand(), "BattleFieldScene");
    SynchronizedScene::SynchronizedOnReceived(cmd);


    switch (cmd->GetCommand()) {
    case ROOM_GAME_START_ACK:               m_pBattleFieldLayer->CmdRoomGameStartAck(cmd);          break;
    case BATTLE_FIELD_LOAD_ACK:             m_pBattleFieldLayer->CmdBattleFieldLoadAck(cmd);        break;
    case BATTLE_FIELD_LEAVE_ACK:            m_pBattleFieldLayer->CmdBattleFieldLeaveAck(cmd);       break;
    case BATTLE_FIELD_TANK_UPDATE_SYN:      m_pBattleFieldLayer->CmdBattleFieldTankUpdateSyn(cmd);  break;
    case BATTLE_FIELD_PLAYWAIT_END_SYN:     m_pBattleFieldLayer->CmdBattleFieldPlayWaitEndSyn(cmd); break;
    case BATTLE_FIELD_PLAYING_END_SYN:      m_pBattleFieldLayer->CmdBattleFieldPlayingEndSyn(cmd);  break;
    case BATTLE_FIELD_ENDWAIT_END_SYN:      m_pBattleFieldLayer->CmdBattleFieldEndWaitEndSyn(cmd);  break;
    case CHAT_MESSAGE_ACK:                  m_pBattleFieldLayer->CmdChatMessageAck(cmd);            break;
    }
}


