/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:28:12 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/Tutturu.h>
#include <SteinsGate/Client/SceneBase.h>

class SGChannelSelectScene : public SceneBase
{
public:
	SGChannelSelectScene();
	static SGChannelSelectScene* create();

	void OnKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	void OnKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;

	void update(float _dt) override;

	bool init() override;
	void onEnterTransitionDidFinish() override;

	SceneType_t GetType() override
	{
		return SceneType::ChannelSelect;
	}
};
