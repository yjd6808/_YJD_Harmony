/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:24:22 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

#include <SteinsGate/Client/Struct.h>

class WorldScene;
class UILayer;

class SceneBase : public SGScene
{
public:
	SceneBase();
	~SceneBase() override;

	bool init() override;
	void onEnter() override;
	void onExit() override;
	void onEnterTransitionDidFinish() override;
	void onExitTransitionDidStart() override;
	void removeAllChildren() override;

	virtual SceneType_t getType() = 0;

	virtual bool onMouseMove(SGEventMouse* _pMouseEvent) { return true; }
	virtual bool onMouseDown(SGEventMouse* _pMouseEvent) { return true; }
	virtual bool onMouseUp(SGEventMouse* _pMouseEvent) { return true; }
	virtual bool onMouseScroll(SGEventMouse* _pMouseEvent) { return true; }

	virtual void onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) = 0;
	virtual void onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) = 0;

protected:
	WorldScene* m_pWorldScene;
	UILayer* m_pUILayer;
};
