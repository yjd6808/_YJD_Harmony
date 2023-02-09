/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:27:43 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Research/SGSceneBase.h>

class SGLoginScene : public SGSceneBase
{
public:
    SGLoginScene();
    static SGLoginScene* create();

    void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
    void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;

    void update(float dt) override;
    bool init() override;
};

