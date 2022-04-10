/*
 * 작성자 : 윤정도
 */

#pragma once

#include <TF/SourceHeader.h>
#include <TF/Scenes/BattleFieldLayer.h>
#include <TF/Scenes/GridLayer.h>
#include <TF/Scenes/SynchronizedScene.h>

class BattleFieldScene : public SynchronizedScene
{
public:
    static Scene* createScene();

    bool init() override;
    CREATE_FUNC(BattleFieldScene);

    bool SynchronizedOnReceived(JNetwork::ICommand* cmd) override;

private:
    BattleFieldLayer* m_pBattleFieldLayer;
};

