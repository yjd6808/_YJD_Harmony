/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:24:22 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

#include <SteinsGate/Research/SGStruct.h>

class SGSceneBase : public SGScene
{
public:
	SGSceneBase(SceneType_t type);
	~SGSceneBase() override;

	virtual void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) = 0;
	virtual void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) = 0;

private:
	SceneType_t m_eType;
};