/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:34 PM
 * =====================
 * 후;; 파일 생성을 2월 15일에 했는데 코드 작성을 4월 25일에 하네..
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UI_Login.h"

#include <SteinsGate/Client/Define_UI.h>
#include <SteinsGate/Client/Define_Popup.h>
#include <SteinsGate/Client/S_AUTH.h>

#include "AuthenticationComponent.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UI_Login::UI_Login(UIGroupInfo* _pGroupInfo)
: UIRootGroup(_pGroupInfo)
, labelSource_(nullptr)
, labelDeveloper_(nullptr)
, groupLoginBox_(nullptr)
, spriteBackground_(nullptr)
, hangameLoginToggleButton_(nullptr)
, dnfLoginToggleButton_(nullptr)
, startButton_(nullptr)
, terminateButton_(nullptr)
, idEditBox_(nullptr)
, passEditBox_(nullptr)
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

	labelSource_ = Core::Contents.UIManager->getLabel(UI_LOGIN_LABEL_SOURCE);
	labelDeveloper_ = Core::Contents.UIManager->getLabel(UI_LOGIN_LABEL_DEVELOPER);

	groupLoginBox_ = Core::Contents.UIManager->getGroup(UI_LOGIN_GROUP_LOGIN_BOX);
	spriteBackground_ = Core::Contents.UIManager->getSprite(UI_LOGIN_LOGIN_BOX_SPRITE_BACKGROUND);
	hangameLoginToggleButton_ = Core::Contents.UIManager->
	                                           getToggleButton(UI_LOGIN_LOGIN_BOX_TOGGLEBUTTON_HANGAME_LOGIN);
	dnfLoginToggleButton_ = Core::Contents.UIManager->getToggleButton(UI_LOGIN_LOGIN_BOX_TOGGLEBUTTON_DNF_LOGIN);
	startButton_ = Core::Contents.UIManager->getButton(UI_LOGIN_LOGIN_BOX_BUTTON_START);
	terminateButton_ = Core::Contents.UIManager->getButton(UI_LOGIN_LOGIN_BOX_BUTTON_TERMINATE);
	idEditBox_ = Core::Contents.UIManager->getEditBox(UI_LOGIN_LOGIN_BOX_EDITBOX_ID);
	idEditBox_->setMaxLength(Const::StringLen::AccountId);
	passEditBox_ = Core::Contents.UIManager->getEditBox(UI_LOGIN_LOGIN_BOX_EDITBOX_PW);
	passEditBox_->setInputFlag(SGEditBox::InputFlag::PASSWORD);
	passEditBox_->setMaxLength(Const::StringLen::AccountPass);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::OnLoaded()
{
	setTab(tab_);

	const Size scaledContentSize = groupLoginBox_->getContentSize() * 1.2f;
	groupLoginBox_->setContentSize(scaledContentSize);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::onUpdate(float _dt)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::OnMouseDownTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::OnMouseUpTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent)
{
	switch (_pElement->GetCode())
	{
	case UI_LOGIN_LOGIN_BOX_BUTTON_START:
		login();
		break;
	case UI_LOGIN_LOGIN_BOX_BUTTON_TERMINATE:
		Core::Contents.World->terminate();
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
		setTab(Tab::eHangame);
		break;
	case UI_LOGIN_LOGIN_BOX_TOGGLEBUTTON_DNF_LOGIN:
		setTab(Tab::eDnf);
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UI_Login::onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent)
{
	if (_keyCode == EventKeyboard::KeyCode::KEY_ENTER)
	{
		login();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UI_Login::onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::OnEditBoxEditingDidEndWithAction(UIEditBox* _pEditBox, SGEditBoxEndAction _endAction)
{
	if (_endAction == EditBoxDelegate::EditBoxEndAction::RETURN)
	{
		login();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::setTab(Tab _tab)
{
	tab_ = _tab;

	if (tab_ == Tab::eHangame)
	{
		hangameLoginToggleButton_->setToggleState(ToggleState::eToggled);
		dnfLoginToggleButton_->setToggleState(ToggleState::eNormal);
	}
	else if (tab_ == Tab::eDnf)
	{
		hangameLoginToggleButton_->setToggleState(ToggleState::eNormal);
		dnfLoginToggleButton_->setToggleState(ToggleState::eToggled);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UI_Login::login()
{
	if (idEditBox_->getText().length() == 0 || passEditBox_->getText().length() == 0)
	{
		Core::Contents.PopupManager->showOk(SG_TEXT_RAW("LOGIN_REQUIRED_ID_PASS"));
		return;
	}

	AuthenticationComponent* pAuthenticationComponent = Core::Net->getAuthenticationComponent();
	pAuthenticationComponent->setAccountIdPass(idEditBox_->getText().c_str(), passEditBox_->getText().c_str());

	if (!Core::Net->connectAuthTcp())
	{
		Core::Contents.PopupManager->showOk(SG_TEXT_RAW("CONNECT_AUTH_FAILED_UNCONNECTABLE_STATE"));
		return;
	}

	Core::Contents.PopupManager->showNone(
		SG_TEXT_RAW_FMT_STD("CONNECT_SERVER", ServerType::Name[ServerType::Auth]), DEF_POPUP_CONNECT_AUTH);
}
