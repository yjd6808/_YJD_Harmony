/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:08 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIRootGroup.h>

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
	void OnMouseDownTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent) override;
	void OnMouseUpTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent) override;
	void OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state) override;
	bool OnKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	bool OnKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	void OnEditBoxEditingDidEndWithAction(UIEditBox* _pEditBox, SGEditBoxEndAction _endAction) override;

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
