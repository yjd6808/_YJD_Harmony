/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:24:22 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

#include <SteinsGate/Client/SGStruct.h>

class SGWorldScene;
class SGUILayer;
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

	virtual bool onMouseMove(SGEventMouse* mouseEvent) { return true; }
	virtual bool onMouseDown(SGEventMouse* mouseEvent) { return true; }
	virtual bool onMouseUp(SGEventMouse* mouseEvent) { return true; }
	virtual bool onMouseScroll(SGEventMouse* mouseEvent) { return true; }

	virtual void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) = 0;
	virtual void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) = 0;
protected:
	SGWorldScene* m_pWorldScene;
	SGUILayer* m_pUILayer;
};