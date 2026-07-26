#include "Game/UI_Implementation/UI_Login.h"

#include "sgcl/Define/Define_Popup.h"
#include "sgcl/Net/Send/S_AUTH.h"
#include "sgcl/Scene/Scene_World.h"
#include "sgcl/Game/Contents/UIManager.h"
#include "sgcl/Net/NetCore.h"
#include "sgcl/Game/Contents/Contents.h"

#include "sgcl/Net/Component/AuthenticationComponent.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

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

void UI_Login::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);

	pLabelSource_ = FindElementByName<UILabel>("source");
	pLabelDeveloper_ = FindElementByName<UILabel>("developer");

	pGroupLoginBox_ = FindElementByName<UIGroup>("login_box");
	pSpriteBackground_ = FindElementByName<UISprite>("background");
	pToggleButtonHangameLogin_ = FindElementByName<UIToggleButton>("hangame_login");
	pToggleBtnDnfLogin_ = FindElementByName<UIToggleButton>("dnf_login");
	pBtnStart_ = FindElementByName<UIButton>("start");
	pBtnTerminate_ = FindElementByName<UIButton>("terminate");
	pEditBoxId_ = FindElementByName<UIEditBox>("id");
	if (pEditBoxId_)
	{
		pEditBoxId_->SetMaxLength(Const::StringLen::AccountId);
	}
	pEditBoxPassword_ = FindElementByName<UIEditBox>("pw");
	if (pEditBoxPassword_)
	{
		pEditBoxPassword_->SetInputFlag(cc_ui::EditBox::InputFlag::PASSWORD);
		pEditBoxPassword_->SetMaxLength(Const::StringLen::AccountPass);
	}
}

void UI_Login::OnLoaded()
{
	SetTab(tab_);

	const Size scaledContentSize = pGroupLoginBox_->getContentSize() * 1.2f;
	pGroupLoginBox_->setContentSize(scaledContentSize);
}

void UI_Login::OnUpdate(float _dt)
{
}

void UI_Login::OnMouseDownTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent)
{
}

void UI_Login::OnMouseUpTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent)
{
	const char* name = _pElement->GetName();

	if (strcmp(name, "start") == 0)
	{
		Login();
	}
	else if (strcmp(name, "terminate") == 0)
	{
		g_cWorld.Terminate();
	}
}

void UI_Login::OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state)
{
	const char* name = _pToggleBtn->GetName();

	if (strcmp(name, "hangame_login") == 0)
	{
		SetTab(Tab::eHangame);
	}
	else if (strcmp(name, "dnf_login") == 0)
	{
		SetTab(Tab::eDnf);
	}
}

bool UI_Login::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	if (_keyCode == EventKeyboard::KeyCode::KEY_ENTER)
	{
		Login();
	}

	return true;
}

bool UI_Login::OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	return true;
}

void UI_Login::OnEditBoxEditingDidEndWithAction(UIEditBox* _pEditBox, cc_ui::EditBoxDelegate::EditBoxEndAction _endAction)
{
	if (_endAction == EditBoxDelegate::EditBoxEndAction::RETURN)
	{
		Login();
	}
}

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

REGISTER_UI(ui_login, UI_Login)