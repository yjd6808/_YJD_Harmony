/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:34 PM
 * =====================
 * 후;; 파일 생성을 2월 15일에 했는데 코드 작성을 4월 25일에 하네..
 */


#include "Core.h"
#include "UI_Login.h"

#include <sgcl/Define_UI.h>
#include <sgcl/Define_Popup.h>
#include <sgcl/S_AUTH.h>
#include <sgcl/_Scene/Scene_World.h>
#include <sgcl/UIManager.h>
#include <sgcl/_Net/NetCore.h>
#include <sgcl/Contents.h>

#include <sgcl/AuthenticationComponent.h>



USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UI_Login::UI_Login(UIGroupInfo* _pGroupInfo)
: UIRootGroup(_pGroupInfo)
, pLabelSource_(nullptr)
, pLabelDeveloper_(nullptr)
, pGroupLoginBox_(nullptr)
, pSpriteBackground_(nullptr)
, pToggleButtonHangameLogin_(nullptr)
, pToggleBtnDnfLogin_(nullptr)
, pBtnStart_(nullptr)
, pBtnTerminate_(nullptr)
, pEditBoxId_(nullptr)
, pEditBoxPassword_(nullptr)
, tab_(Tab::eDnf)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::OnInit()
{
	//#define UI_LOGIN_SPRITE_AGE_18	1001
	//#define UI_LOGIN_SPRITE_LOGO	1002
	//#define UI_LOGIN_SPRITE_GANZI	1003
	//#define UI_LOGIN_LABEL_SOURCE	1004
	//#define UI_LOGIN_LABEL_DEVELOPER	1005
	//#define UI_LOGIN_GROUP_LOGIN_BOX	2000
	//#define UI_LOGIN_LOGIN_BOX_SPRITE_BACKGROUND	2001
	//#define UI_LOGIN_LOGIN_BOX_BUTTON_HANGAME_LOGIN	2002
	//#define UI_LOGIN_LOGIN_BOX_BUTTON_DNF_LOGIN	2003
	//#define UI_LOGIN_LOGIN_BOX_BUTTON_START	2004
	//#define UI_LOGIN_LOGIN_BOX_BUTTON_TERMINATE	2005
	//#define UI_LOGIN_LOGIN_BOX_EDITBOX_ID	2006
	//#define UI_LOGIN_LOGIN_BOX_EDITBOX_PW	2007

	pLabelSource_ = g_cUIMgr.GetLabel(UI_LOGIN_LABEL_SOURCE);
	pLabelDeveloper_ = g_cUIMgr.GetLabel(UI_LOGIN_LABEL_DEVELOPER);

	pGroupLoginBox_ = g_cUIMgr.GetGroup(UI_LOGIN_GROUP_LOGIN_BOX);
	pSpriteBackground_ = g_cUIMgr.GetSprite(UI_LOGIN_LOGIN_BOX_SPRITE_BACKGROUND);
	pToggleButtonHangameLogin_ = g_cUIMgr.GetToggleButton(UI_LOGIN_LOGIN_BOX_TOGGLEBUTTON_HANGAME_LOGIN);
	pToggleBtnDnfLogin_ = g_cUIMgr.GetToggleButton(UI_LOGIN_LOGIN_BOX_TOGGLEBUTTON_DNF_LOGIN);
	pBtnStart_ = g_cUIMgr.GetButton(UI_LOGIN_LOGIN_BOX_BUTTON_START);
	pBtnTerminate_ = g_cUIMgr.GetButton(UI_LOGIN_LOGIN_BOX_BUTTON_TERMINATE);
	pEditBoxId_ = g_cUIMgr.GetEditBox(UI_LOGIN_LOGIN_BOX_EDITBOX_ID);
	pEditBoxId_->SetMaxLength(Const::StringLen::AccountId);
	pEditBoxPassword_ = g_cUIMgr.GetEditBox(UI_LOGIN_LOGIN_BOX_EDITBOX_PW);
	pEditBoxPassword_->SetInputFlag(cc_ui::EditBox::InputFlag::PASSWORD);
	pEditBoxPassword_->SetMaxLength(Const::StringLen::AccountPass);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::OnLoaded()
{
	SetTab(tab_);

	const Size scaledContentSize = pGroupLoginBox_->getContentSize() * 1.2f;
	pGroupLoginBox_->setContentSize(scaledContentSize);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::OnUpdate(float _dt)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::OnMouseDownTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::OnMouseUpTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent)
{
	switch (_pElement->GetCode())
	{
	case UI_LOGIN_LOGIN_BOX_BUTTON_START:
		Login();
		break;
	case UI_LOGIN_LOGIN_BOX_BUTTON_TERMINATE:
		g_cWorld.Terminate();
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state)
{
	switch (_pToggleBtn->GetCode())
	{
	case UI_LOGIN_LOGIN_BOX_TOGGLEBUTTON_HANGAME_LOGIN:
		SetTab(Tab::eHangame);
		break;
	case UI_LOGIN_LOGIN_BOX_TOGGLEBUTTON_DNF_LOGIN:
		SetTab(Tab::eDnf);
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UI_Login::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	if (_keyCode == EventKeyboard::KeyCode::KEY_ENTER)
	{
		Login();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UI_Login::OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::OnEditBoxEditingDidEndWithAction(UIEditBox* _pEditBox, cc_ui::EditBoxDelegate::EditBoxEndAction _endAction)
{
	if (_endAction == EditBoxDelegate::EditBoxEndAction::RETURN)
	{
		Login();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::SetTab(Tab _tab)
{
	tab_ = _tab;

	if (tab_ == Tab::eHangame)
	{
		pToggleButtonHangameLogin_->SetToggleState(ToggleState::eToggled);
		pToggleBtnDnfLogin_->SetToggleState(ToggleState::eNormal);
	}
	else if (tab_ == Tab::eDnf)
	{
		pToggleButtonHangameLogin_->SetToggleState(ToggleState::eNormal);
		pToggleBtnDnfLogin_->SetToggleState(ToggleState::eToggled);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::Login()
{
	if (pEditBoxId_->GetText().length() == 0 || pEditBoxPassword_->GetText().length() == 0)
	{
		sg::Contents.PopupManager->ShowOk(SG_LOCAL_RAW("LOGIN_REQUIRED_ID_PASS"));
		return;
	}

	g_cNet.authentication_.SetAccountIdPass(pEditBoxId_->GetText().c_str(), pEditBoxPassword_->GetText().c_str());

	if (!g_cNet.ConnectAuthTcp())
	{
		sg::Contents.PopupManager->ShowOk(SG_LOCAL_RAW("CONNECT_AUTH_FAILED_UNCONNECTABLE_STATE"));
		return;
	}
	
	std::string msg = StringUtils::format(SG_LOCAL_RAW("CONNECT_SERVER"), ServerType::Name[ServerType::Auth]);
	sg::Contents.PopupManager->ShowNone(msg, DEF_POPUP_CONNECT_AUTH);
}
