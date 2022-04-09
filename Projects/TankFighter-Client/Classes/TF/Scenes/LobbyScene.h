/*
 * 작성자 : 윤정도
 */

#pragma once

#include <TF/SourceHeader.h>
#include <TF/Scenes/LobbyLayer.h>
#include <TF/Scenes/GridLayer.h>
#include <TF/Scenes/SynchronizedScene.h>

class LobbyScene : public SynchronizedScene
{
public:
    static Scene* createScene();

    bool init() override;
    CREATE_FUNC(LobbyScene);

    void SynchronizedOnReceived(JNetwork::ICommand* cmd) override;

private:
    LobbyLayer* m_pLobbyLayer;
};

