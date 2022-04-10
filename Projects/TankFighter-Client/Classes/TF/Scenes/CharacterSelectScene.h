/*
 * 작성자 : 윤정도
 */

#pragma once

#include <TF/SourceHeader.h>
#include <TF/Scenes/CharacterSelectLayer.h>
#include <TF/Scenes/GridLayer.h>
#include <TF/Scenes/SynchronizedScene.h>

class CharacterSelectScene : public SynchronizedScene
{
public:
    static Scene* createScene();

    bool init() override;
    CREATE_FUNC(CharacterSelectScene);

    bool SynchronizedOnReceived(JNetwork::ICommand* cmd) override;

private:
    CharacterSelectLayer* m_pCharacterSelectLayer;
};

