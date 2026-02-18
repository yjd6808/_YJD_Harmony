/*
 * 작성자 : 윤정도
 */

#pragma once

#include <sgtc/_Scene/Scene_Base.h>

class GridLayer;
class GameScene : public SceneBase
{
public:
	CREATE_FUNC(GameScene)

	GameScene();
	~GameScene() override;

	void OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;
	void OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;

	void update(float _dt) override;
	bool init() override;
	void onExit() override;

private:

	SceneType_t GetType() override
	{
		return SceneType::Game;
	}
};
