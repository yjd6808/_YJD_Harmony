/*
 * 작성자 : 윤정도
 */

#pragma once

#include <TF/SourceHeader.h>
#include <TF/Scenes/LoginLayer.h>
#include <TF/Scenes/GridLayer.h>
#include <TF/Scenes/SynchronizedScene.h>

class LoginScene : public SynchronizedScene
{
public:
    static Scene* createScene();

    bool init() override;
    CREATE_FUNC(LoginScene);

    bool SynchronizedOnReceived(JNetwork::ICommand* cmd) override;

private:
    LoginLayer* m_pLoginLayer;
};

