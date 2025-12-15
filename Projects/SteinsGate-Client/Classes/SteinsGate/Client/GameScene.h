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
	CREATE_FUNC(SGGameScene)

	SGGameScene();
	~SGGameScene() override;

	void onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	void onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;

	void update(float _dt) override;
	bool init() override;

	MapLayer* getMap()
	{
		return mapLayer_;
	}

	void cmdLoadChar();
	void cmdEnterMap();
	void onExit() override;

private:
	MapLayer* mapLayer_;
	GridLayer* gridLayer_;

	SceneType_t getType() override
	{
		return SceneType::Game;
	}
};
