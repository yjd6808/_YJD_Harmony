/*
 * 작성자 : 윤정도
 */

#include <TF/Scenes/CharacterSelectScene.h>
#include <Common/Command.h>

Scene* CharacterSelectScene::createScene()
{
    return CharacterSelectScene::create();
}

bool CharacterSelectScene::init()
{
    if ( !SynchronizedScene::init())
        return false;

    m_pCharacterSelectLayer = CharacterSelectLayer::create();
	this->addChild(m_pCharacterSelectLayer, 0);
    return true;
}

void CharacterSelectScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    CCLOG("%d 커맨드 수신 (%s)", cmd->GetCommand(), "CharacterSelectScene");
	SynchronizedScene::SynchronizedOnReceived(cmd);

    switch (cmd->GetCommand()) {
    case LOAD_CHARACTER_INFO_ACK: m_pCharacterSelectLayer->CmdLoadCharacterInfoAck(cmd); break;
    case CREATE_CHARACTER_ACK: m_pCharacterSelectLayer->CmdCreateCharacterAck(cmd); break;
    case DELETE_CHARACTER_ACK: m_pCharacterSelectLayer->CmdDeleteCharacterAck(cmd); break;
    case SELECT_CHARACTER_ACK: m_pCharacterSelectLayer->CmdSelectCharacterAck(cmd); break;
    }
}


