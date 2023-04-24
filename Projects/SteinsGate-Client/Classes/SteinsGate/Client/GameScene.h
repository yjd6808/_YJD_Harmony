/*
 * 작성자 : 윤정도
 */

#pragma once

#include <SteinsGate/Common/GridLayer.h>

#include <SteinsGate/Client/SceneBase.h>
#include <SteinsGate/Client/MapLayer.h>

class SGGameScene : public SceneBase
{
public:
    SGGameScene();

    void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
    void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;

    void update(float dt) override;
    bool init() override;
    CREATE_FUNC(SGGameScene)
    ~SGGameScene() override;

    MapLayer* getMap() { return m_pMapLayer; }

    void cmdLoadChar();
    void cmdEnterMap();
    void onExit() override;
private:
    MapLayer* m_pMapLayer;
    GridLayer* m_pGridLayer;
    SceneType_t getType() override { return SceneType::Game; }
};

