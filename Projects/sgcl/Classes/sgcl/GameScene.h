/*
 * 작성자 : 윤정도
 */

#pragma once

#include <sgcl/SceneBase.h>
#include <sgcl/MapLayer.h>

class GridLayer;
class SGGameScene : public SceneBase
{
public:
	CREATE_FUNC(SGGameScene)

	SGGameScene();
	~SGGameScene() override;

	void OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;
	void OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;

	void update(float _dt) override;
	bool init() override;

	MapLayer* GetMap()
	{
		return mapLayer_;
	}

	void CmdLoadChar();
	void CmdEnterMap();

	void onExit() override;

private:
	MapLayer* mapLayer_;
	GridLayer* gridLayer_;

	SceneType_t GetType() override
	{
		return SceneType::Game;
	}
};
