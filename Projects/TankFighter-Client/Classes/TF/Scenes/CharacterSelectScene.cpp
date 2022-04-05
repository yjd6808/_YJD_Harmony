#include <TF/Scenes/CharacterSelectScene.h>
#include <Common/Command.h>

Scene* CharacterSelectScene::createScene()
{
    return CharacterSelectScene::create();
}

bool CharacterSelectScene::init()
{
    if ( !Scene::init() )
        return false;

    m_pCharacterSelectLayer = CharacterSelectLayer::create();
    m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);

	this->addChild(m_pCharacterSelectLayer, 0);
    this->addChild(m_pGridLayer, 1);
    return true;
}

void CharacterSelectScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    CCLOG("%d 커맨드 수신", cmd->GetCommand());
    switch (cmd->GetCommand()) {
    case LOAD_CHARACTER_INFO_ACK: m_pCharacterSelectLayer->CmdLoadCharacterInfoAck(cmd); break;
    case CREATE_CHARACTER_ACK: m_pCharacterSelectLayer->CmdCreateCharacterAck(cmd); break;
    case DELETE_CHARACTER_ACK: m_pCharacterSelectLayer->CmdDeleteCharacterAck(cmd); break;
    case SELECT_CHARACTER_ACK: m_pCharacterSelectLayer->CmdSelectCharacterAck(cmd); break;
    }
}


