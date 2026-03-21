/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:27:43 AM
 * =====================
 *
 */


#pragma once

#include "sgcl/_Scene/Scene_Base.h"

class SGLoginScene : public SceneBase
{
public:
	SGLoginScene();
	static SGLoginScene* Create();

	void OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;
	void OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;

	void update(float _dt) override;
	bool init() override;
	SceneType_t GetType() override { return SceneType::Login; }
};
