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
	void onUpdate(float _dt) override;
	void OnMouseDownTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent) override;
	void OnMouseUpTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent) override;
	void OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state) override;
	bool onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	bool onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	void OnEditBoxEditingDidEndWithAction(UIEditBox* _pEditBox, SGEditBoxEndAction _endAction) override;

public:
	void setTab(Tab _tab);
	void login();

private:
	UILabel* labelSource_;
	UILabel* labelDeveloper_;

	UIGroup* groupLoginBox_;
	UISprite* spriteBackground_;
	UIToggleButton* hangameLoginToggleButton_;
	UIToggleButton* dnfLoginToggleButton_;
	UIButton* startButton_;
	UIButton* terminateButton_;
	UIEditBox* idEditBox_;
	UIEditBox* passEditBox_;

	Tab tab_;
};
