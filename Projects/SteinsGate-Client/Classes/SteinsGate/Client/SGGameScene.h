/*
 * 작성자 : 윤정도
 */

#pragma once

#include <SteinsGate/Common/GridLayer.h>

#include <SteinsGate/Client/SGSceneBase.h>
#include <SteinsGate/Client/SGMapLayer.h>

class SGGameScene : public SGSceneBase
{
public:
    SGGameScene();

    void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
    void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;

    void update(float dt) override;
    bool init() override;
    CREATE_FUNC(SGGameScene)
    ~SGGameScene() override;

    SGMapLayer* getMap() { return m_pMapLayer; }

    void cmdLoadChar();
    void cmdEnterMap();
    void onExit() override;
private:
    SGMapLayer* m_pMapLayer;
    GridLayer* m_pGridLayer;
    SceneType_t getType() override { return SceneType::Game; }
};

