/*
 * 작성자: 윤정도
 * 생성일: 2/14/2023 10:12:09 PM
 * =====================
 * UI 마스타 그룹 외에 절대 addChild 금지
 */

#pragma once

#include <sgcl/Core.h>
#include <sgcl/UIGroup.h>

class UILayer : public c2d::Layer
{
public:
	UILayer();

	static UILayer* Create();

	bool init() override;

	void OnMouseMove(c2d::EventMouse* _pMouseEvent);
	void OnMouseDown(c2d::EventMouse* _pMouseEvent);
	void OnMouseUp(c2d::EventMouse* _pMouseEvent);
	void OnMouseScroll(c2d::EventMouse* _pMouseEvent) const;

	void update(float _delta) override;
	void onKeyPressed(c2d::EventKeyboard::KeyCode _keyCode, c2d::Event* _pEvent) override;
	void onKeyReleased(c2d::EventKeyboard::KeyCode _keyCode, c2d::Event* _pEvent) override;

	UIRootGroup* FindGroup(int _groupCode);

	void ForEach(const jc::Action<UIRootGroup*>& _actionFn);
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
