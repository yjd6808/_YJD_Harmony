/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:28:12 AM
 * =====================
 *
 */

#pragma once

#include <sgcl/Core.h>
#include <sgcl/_Scene/Scene_Base.h>

class SGChannelSelectScene : public SceneBase
{
public:
	SGChannelSelectScene();
	static SGChannelSelectScene* create();

	void OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;
	void OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;

	void update(float _dt) override;

	bool init() override;
	void onEnterTransitionDidFinish() override;

	SceneType_t GetType() override
	{
		return SceneType::ChannelSelect;
	}
};
