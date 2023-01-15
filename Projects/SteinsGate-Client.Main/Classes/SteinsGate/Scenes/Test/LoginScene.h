/*
 * 작성자 : 윤정도
 */

#pragma once

#include <SteinsGate/Tutturu.h>
#include <SteinsGate/Scenes/Test/LoginLayer.h>
#include <SteinsGate/Common/Engine/GridLayer.h>

class LoginScene : cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    bool init() override;
    CREATE_FUNC(LoginScene);
private:
    LoginLayer* m_pLoginLayer;
    GridLayer* m_pGridLayer;
};

