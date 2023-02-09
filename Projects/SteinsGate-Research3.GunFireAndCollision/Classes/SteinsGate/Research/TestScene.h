/*
 * 작성자 : 윤정도
 */

#pragma once

#include <SteinsGate/Common/Engine/GridLayer.h>
#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/WorldLayer.h>

class TestScene : cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    bool init() override;
    CREATE_FUNC(TestScene);

    WorldLayer* getTestLayer() { return m_pTestLayer; }
private:
    WorldLayer* m_pTestLayer;
    GridLayer* m_pGridLayer;
};

