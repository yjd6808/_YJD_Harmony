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

void BattleFieldScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    CCLOG("%d 커맨드 수신 (%s)", cmd->GetCommand(), "BattleFieldScene");
    SynchronizedScene::SynchronizedOnReceived(cmd);


    switch (cmd->GetCommand()) {
        //case 
    //case LOAD_CHARACTER_INFO_ACK: m_pCharacterSelectLayer->CmdLoadCharacterInfoAck(cmd); break;
    }
}


