/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:28:12 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Research/SGSceneBase.h>

class SGServerSelectScene : public SGSceneBase
{
public:
    SGServerSelectScene();
    static SGServerSelectScene* create();

    void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
    void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;

    void update(float dt) override;
    bool init() override;
};

