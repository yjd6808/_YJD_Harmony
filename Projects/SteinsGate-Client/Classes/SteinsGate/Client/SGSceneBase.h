/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:24:22 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

#include <SteinsGate/Client/SGStruct.h>

class SGSceneBase : public SGScene
{
public:
	SGSceneBase();
	~SGSceneBase() override;

	bool init() override;
	void onEnter() override;
	void onExit() override;
	void onEnterTransitionDidFinish() override;
	void onExitTransitionDidStart() override;

	virtual SceneType_t getType() = 0;

	virtual void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) = 0;
	virtual void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) = 0;
};