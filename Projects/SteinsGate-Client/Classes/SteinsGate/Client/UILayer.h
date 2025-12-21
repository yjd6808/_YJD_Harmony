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

	static UILayer* Create();

	bool init() override;

	void OnMouseMove(SGEventMouse* _pMouseEvent);
	void OnMouseDown(SGEventMouse* _pMouseEvent);
	void OnMouseUp(SGEventMouse* _pMouseEvent);
	void OnMouseScroll(SGEventMouse* _pMouseEvent) const;

	void update(float _delta) override;
	void onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	void onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;

	UIRootGroup* FindGroup(int _groupCode);

	void ForEach(const SGActionFn<UIRootGroup*>& _actionFn);
	void AddUIGroup(int _groupCode, int _zOrder = 0);
	void AddUIGroup(UIRootGroup* _pGroup, int _zOrder = 0);
	void RemoveUIGroup(int _groupCode);
	void RemoveUIGroup(UIRootGroup* _pGroup);
	void Clear();
	void ClearUnload();

private:
	UIGroup* pPrevOverStateGroup_;
	UIGroup* pPrevPressedStateGroup_;
};
