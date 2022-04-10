/*
 * 작성자 : 윤정도
 */

#pragma once

#include <TF/SourceHeader.h>
#include <TF/Scenes/ChannelLayer.h>
#include <TF/Scenes/GridLayer.h>
#include <TF/Scenes/SynchronizedScene.h>

class ChannelScene : public SynchronizedScene
{
public:
    static Scene* createScene();

    bool init() override;
    CREATE_FUNC(ChannelScene);

    bool SynchronizedOnReceived(JNetwork::ICommand* cmd) override;

private:
    ChannelLayer* m_pChannelLayer;
};

