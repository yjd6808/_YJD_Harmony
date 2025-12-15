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

	void onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	void onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;

	void update(float _dt) override;
	bool init() override;
	SceneType_t getType() override { return SceneType::Login; }
};
