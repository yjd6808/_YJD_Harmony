/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:24:22 AM
 * =====================
 *
 */


#pragma once

#include <sgcl/Core.h>

#include <sgcl/Struct.h>

class WorldScene;
class UILayer;

class SceneBase : public c2d::Scene
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

	virtual bool OnMouseMove(c2d::EventMouse* _pMouseEvent) { return true; }
	virtual bool OnMouseDown(c2d::EventMouse* _pMouseEvent) { return true; }
	virtual bool OnMouseUp(c2d::EventMouse* _pMouseEvent) { return true; }
	virtual bool OnMouseScroll(c2d::EventMouse* _pMouseEvent) { return true; }

	virtual void OnKeyPressed(c2d::EventKeyboard::KeyCode _keyCode, c2d::Event* _pEvent) = 0;
	virtual void OnKeyReleased(c2d::EventKeyboard::KeyCode _keyCode, c2d::Event* _pEvent) = 0;

protected:
	WorldScene* pWorldScene_ = nullptr;
	UILayer* pUILayer_ = nullptr;
};
