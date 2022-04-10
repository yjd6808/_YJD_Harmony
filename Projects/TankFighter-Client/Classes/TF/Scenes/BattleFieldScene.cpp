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

bool BattleFieldScene::SynchronizedOnReceived(ICommand* cmd) {
    if (!SynchronizedScene::SynchronizedOnReceived(cmd)) {
        return false;
    }

    // 캐릭터 움직이는 패킷은 출력안함
    // 통계 갱신도 출력안함
    if (cmd->GetCommand() != BATTLE_FIELD_TANK_UPDATE_SYN && cmd->GetCommand() != BATTLE_FILED_STATISTICS_UPDATE_SYN) {
        CCLOG("%d 커맨드 수신 (%s)", cmd->GetCommand(), "BattleFieldScene");
    }


    switch (cmd->GetCommand()) {
    case ROOM_GAME_START_ACK:                   m_pBattleFieldLayer->CmdRoomGameStartAck(cmd);                  break;
    case UPDATE_CHARACTER_INFO_ACK:             m_pBattleFieldLayer->CmdUpdateCharacterInfoAck(cmd);            break;
    case UPDATE_ROOMINFO_ACK:                   m_pBattleFieldLayer->CmdUpdateRoomInfoAck(cmd);                 break;
    case BATTLE_FIELD_LOAD_ACK:                 m_pBattleFieldLayer->CmdBattleFieldLoadAck(cmd);                break;
    case BATTLE_FIELD_LEAVE_ACK:                m_pBattleFieldLayer->CmdBattleFieldLeaveAck(cmd);               break;
    case BATTLE_FIELD_TANK_UPDATE_SYN:          m_pBattleFieldLayer->CmdBattleFieldTankUpdateSyn(cmd);          break;
    case BATTLE_FIELD_PLAYWAIT_END_SYN:         m_pBattleFieldLayer->CmdBattleFieldPlayWaitEndSyn(cmd);         break;
    case BATTLE_FIELD_PLAYING_END_SYN:          m_pBattleFieldLayer->CmdBattleFieldPlayingEndSyn(cmd);          break;
    case BATTLE_FIELD_ENDWAIT_END_SYN:          m_pBattleFieldLayer->CmdBattleFieldEndWaitEndSyn(cmd);          break;
    case BATTLE_FIELD_FIRE_ACK:                 m_pBattleFieldLayer->CmdBattleFieldFireAck(cmd);                break;
    case BATTLE_FIELD_DEATH_ACK:                m_pBattleFieldLayer->CmdBattleFieldDeathAck(cmd);               break;
    case BATTLE_FIELD_REVIVAL_SYN:              m_pBattleFieldLayer->CmdBattleFieldRevivalSyn(cmd);             break;
    case BATTLE_FILED_STATISTICS_UPDATE_SYN:    m_pBattleFieldLayer->CmdBattleFieldStatisticsUpdateSyn(cmd);    break;
    case CHAT_MESSAGE_ACK:                      m_pBattleFieldLayer->CmdChatMessageAck(cmd);                    break;
    }

    return true;
}


