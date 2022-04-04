#pragma once

#include <TF/SourceHeader.h>
#include <TF/Scenes/LobbyLayer.h>
#include <TF/Scenes/GridLayer.h>
#include <TF/Network/SynchronizedScene.h>

class LobbyScene : public SynchronizedScene
{
public:
    static Scene* createScene();

    bool init() override;
    CREATE_FUNC(LobbyScene);

    void SynchronizedOnReceived(JNetwork::ICommand* cmd) override;

private:
    LobbyLayer* m_pLobbyLayer;
    GridLayer* m_pGridLayer;
};

