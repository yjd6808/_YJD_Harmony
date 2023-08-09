/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:27:43 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>
#include <SteinsGate/Client/SceneBase.h>

class SGLoginScene : public SceneBase
{
public:
    SGLoginScene();
    static SGLoginScene* create();

    void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
    void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;

    void update(float dt) override;
    bool init() override;
    SceneType_t getType() override { return SceneType::Login; }
};

