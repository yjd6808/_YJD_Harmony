/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:28:12 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/SGSceneBase.h>

class SGChannelSelectScene : public SGSceneBase
{
public:
    SGChannelSelectScene();
    static SGChannelSelectScene* create();

    void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
    void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;

    void update(float dt) override;
    bool init() override;
    SceneType_t getType() override { return SceneType::ChannelSelect; }
};

