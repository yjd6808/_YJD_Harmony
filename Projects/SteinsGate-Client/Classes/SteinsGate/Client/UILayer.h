/*
 * 작성자: 윤정도
 * 생성일: 2/14/2023 10:12:09 PM
 * =====================
 * UI 마스타 그룹 외에 절대 addChild 금지
 */

#pragma once

#include <SteinsGate/Client/Tutturu.h>
#include <SteinsGate/Client/UIGroup.h>

class UILayer : public SGLayer
{
public:
	UILayer();

	static UILayer* create();

	bool init() override;
	void onMouseMove(SGEventMouse* _pMouseEvent);
	void onMouseDown(SGEventMouse* _pMouseEvent);
	void onMouseUp(SGEventMouse* _pMouseEvent);
	void onMouseScroll(SGEventMouse* _pMouseEvent) const;

	void update(float _delta) override;
	void onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	void onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;

	UIRootGroup* findGroup(int _groupCode);

	void forEach(const SGActionFn<UIRootGroup*>& _actionFn);
	void addUIGroup(int _groupCode, int _zOrder = 0);
	void addUIGroup(UIRootGroup* _pGroup, int _zOrder = 0);
	void removeUIGroup(int _groupCode);
	void removeUIGroup(UIRootGroup* _pGroup);
	void clear();
	void clearUnload();

private:
	UIGroup* pPrevOverStateGroup_;
	UIGroup* pPrevPressedStateGroup_;
};
