/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:24:22 AM
 * =====================
 *
 */


#pragma once

#include <sgcl/Core.h>
#include "WorldScene.h"

JC_SENUM_BEGIN(SceneType)
Login,
ChannelSelect,
Game,
None,
Max = None
JC_SENUM_END(SceneType)

class WorldScene;
class UILayer;

class SceneBase : public cc::Scene
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

	virtual bool OnMouseMove(cc::EventMouse* _pMouseEvent) { return true; }
	virtual bool OnMouseDown(cc::EventMouse* _pMouseEvent) { return true; }
	virtual bool OnMouseUp(cc::EventMouse* _pMouseEvent) { return true; }
	virtual bool OnMouseScroll(cc::EventMouse* _pMouseEvent) { return true; }

	virtual void OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) = 0;
	virtual void OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) = 0;

protected:
	WorldScene* pWorldScene_ = nullptr;
	UILayer* pUILayer_ = nullptr;
};
