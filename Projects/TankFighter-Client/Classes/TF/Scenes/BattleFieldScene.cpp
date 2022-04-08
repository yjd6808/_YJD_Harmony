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
    if ( !Scene::init() )
        return false;

    m_pBattleFieldLayer = BattleFieldLayer::create();
    m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);

	this->addChild(m_pBattleFieldLayer, 0);
    this->addChild(m_pGridLayer, 1);
    return true;
}

void BattleFieldScene::SynchronizedOnReceived(ICommand* cmd) {
    CCLOG("%d 커맨드 수신 (%s)", cmd->GetCommand(), "BattleFieldScene");
    SynchronizedScene::SynchronizedOnReceived(cmd);


    switch (cmd->GetCommand()) {
    case BATTLE_FIELD_LOAD_ACK:             m_pBattleFieldLayer->CmdBattleFieldLoadAck(cmd); break;
    case BATTLE_FIELD_LEAVE_ACK:            m_pBattleFieldLayer->CmdBattleFieldLeaveAck(cmd); break;
    case BATTLE_FIELD_TANK_UPDATE_SYN:      m_pBattleFieldLayer->CmdBattleFieldTankUpdateSyn(cmd); break;
    }
}


