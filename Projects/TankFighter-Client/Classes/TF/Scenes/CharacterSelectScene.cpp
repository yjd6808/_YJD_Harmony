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

bool CharacterSelectScene::SynchronizedOnReceived(JNetwork::ICommand* cmd) {
    if (!SynchronizedScene::SynchronizedOnReceived(cmd)) {
        return false;
    }

    CCLOG("%d 커맨드 수신 (%s)", cmd->GetCommand(), "CharacterSelectScene");

    switch (cmd->GetCommand()) {
    case LOAD_CHARACTER_INFO_ACK: m_pCharacterSelectLayer->CmdLoadCharacterInfoAck(cmd); break;
    case CREATE_CHARACTER_ACK: m_pCharacterSelectLayer->CmdCreateCharacterAck(cmd); break;
    case DELETE_CHARACTER_ACK: m_pCharacterSelectLayer->CmdDeleteCharacterAck(cmd); break;
    case SELECT_CHARACTER_ACK: m_pCharacterSelectLayer->CmdSelectCharacterAck(cmd); break;
    }

    return true;
}


