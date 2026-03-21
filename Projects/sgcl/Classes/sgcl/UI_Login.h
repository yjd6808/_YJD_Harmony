/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:08 PM
 * =====================
 *
 */


#pragma once

#include "sgcl/UIRootGroup.h"

class UI_Login : public UIRootGroup
{
public:
	UI_Login(UIGroupInfo* _pGroupInfo);

	enum class Tab
	{
		eHangame,
		eDnf
	};

protected:
	void OnInit() override;
	void OnLoaded() override;
	void OnUpdate(float _dt) override;
	void OnMouseDownTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent) override;
	void OnMouseUpTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent) override;
	void OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state) override;
	bool OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;
	bool OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;
	void OnEditBoxEditingDidEndWithAction(UIEditBox* _pEditBox, cc_ui::EditBoxDelegate::EditBoxEndAction _endAction) override;

public:
	void SetTab(Tab _tab);
	void Login();

private:
	UILabel* pLabelSource_;
	UILabel* pLabelDeveloper_;

	UIGroup* pGroupLoginBox_;
	UISprite* pSpriteBackground_;
	UIToggleButton* pToggleButtonHangameLogin_;
	UIToggleButton* pToggleBtnDnfLogin_;
	UIButton* pBtnStart_;
	UIButton* pBtnTerminate_;
	UIEditBox* pEditBoxId_;
	UIEditBox* pEditBoxPassword_;

	Tab tab_;
};
