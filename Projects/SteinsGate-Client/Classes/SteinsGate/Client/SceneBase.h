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

	virtual SceneType_t GetType() = 0;

	virtual bool OnMouseMove(SGEventMouse* _pMouseEvent) { return true; }
	virtual bool OnMouseDown(SGEventMouse* _pMouseEvent) { return true; }
	virtual bool OnMouseUp(SGEventMouse* _pMouseEvent) { return true; }
	virtual bool OnMouseScroll(SGEventMouse* _pMouseEvent) { return true; }

	virtual void OnKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) = 0;
	virtual void OnKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) = 0;

protected:
	WorldScene* pWorldScene_ = nullptr;
	UILayer* pUILayer_ = nullptr;
};
