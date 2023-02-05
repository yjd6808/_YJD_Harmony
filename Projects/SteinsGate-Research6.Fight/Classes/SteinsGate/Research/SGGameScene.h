/*
 * 작성자 : 윤정도
 */

#pragma once

#include <SteinsGate/Common/Engine/GridLayer.h>

#include <SteinsGate/Research/SGSceneBase.h>
#include <SteinsGate/Research/SGMapLayer.h>

class SGGameScene : public SGSceneBase
{
public:
    SGGameScene();

    void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event);
    void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event);

    void update(float dt) override;
    bool init() override;
    CREATE_FUNC(SGGameScene)

    void cmdLoadChar();
    void cmdEnterMap();
private:
    SGMapLayer* m_pMapLayer;
    GridLayer* m_pGridLayer;
};

