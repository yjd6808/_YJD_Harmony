#pragma once

#include <TF/SourceHeader.h>
#include <TF/Scenes/RoomLayer.h>
#include <TF/Scenes/GridLayer.h>
#include <TF/Scenes/SynchronizedScene.h>

class RoomScene : public SynchronizedScene
{
public:
    static Scene* createScene();

    bool init() override;
    CREATE_FUNC(RoomScene);

    void SynchronizedOnReceived(JNetwork::ICommand* cmd) override;

private:
    RoomLayer* m_pRoomLayer;
    GridLayer* m_pGridLayer;
};

