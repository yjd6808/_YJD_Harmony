/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:08 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIMasterGroup.h>



class UI_Login : public UIMasterGroup
{
public:
	UI_Login(UIGroupInfo* groupInfo);

	enum class Tab
	{
		eHangame,
		eDnf
	};

protected:
	void onInit() override;
	void onLoaded() override;
	void onUpdate(float dt) override;
	void onMouseDown(UIElement* element, SGEventMouse* mouseEvent) override;
	void onMouseUp(UIElement* element, SGEventMouse* mouseEvent) override;
	void onToggleStateChanged(UIToggleButton* toggleBtn, ToggleState state) override;
	bool onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
	bool onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
	void onEditBoxEditingDidEndWithAction(UIEditBox* editBox, SGEditBoxEndAction endAction) override;
public:
	void setTab(Tab tab);
	void login();
private:

	// #define UI_LOGIN_SPRITE_AGE_18	
	// #define UI_LOGIN_SPRITE_LOGO	
	// #define UI_LOGIN_SPRITE_GANZI	
	// #define UI_LOGIN_LABEL_SOURCE	
	// #define UI_LOGIN_LABEL_DEVELOPER	
	// #define UI_LOGIN_GROUP_LOGIN_BOX	
	// #define UI_LOGIN_LOGIN_BOX_SPRITE_BACKGROUND	
	// #define UI_LOGIN_LOGIN_BOX_BUTTON_HANGAME_LOGIN	
	// #define UI_LOGIN_LOGIN_BOX_BUTTON_DNF_LOGIN	
	// #define UI_LOGIN_LOGIN_BOX_BUTTON_START	
	// #define UI_LOGIN_LOGIN_BOX_BUTTON_TERMINATE	
	// #define UI_LOGIN_LOGIN_BOX_EDITBOX_ID	
	// #define UI_LOGIN_LOGIN_BOX_EDITBOX_PW	

	UILabel* m_pLabelSource;
	UILabel* m_pLabelDeveloper;

	UIGroup* m_pGroupLoginBox;
	UISprite* m_pSpriteBackground;
	UIToggleButton* m_pTBtnHangameLogin;
	UIToggleButton* m_pTBtnDnfLogin;
	UIButton* m_pBtnStart;
	UIButton* m_pBtnTerminate;
	UIEditBox* m_pEbId;
	UIEditBox* m_pEbPass;

	// =========================================

	Tab m_eTab;
};


