/*
 * 작성자 : 윤정도
 */

#pragma once

#include <SteinsGate/Common/Engine/GridLayer.h>

#include <SteinsGate/Research/SGMapLayer.h>

class GameScene : cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    void onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    bool init() override;
    CREATE_FUNC(GameScene);

private:
    SGMapLayer* m_pWorldLayer;
    GridLayer* m_pGridLayer;
};

