/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:34 PM
 * =====================
 * 후;; 파일 생성을 2월 15일에 했는데 코드 작성을 4월 25일에 하네..
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UI_Login.h"

#include <SteinsGate/Client/UIDefine.h>

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;


UI_Login::UI_Login(UIGroupInfo* groupInfo)
	: UIMasterGroup(groupInfo)
	, m_pLabelSource(nullptr)
	, m_pLabelDeveloper(nullptr)
	, m_pGroupLoginBox(nullptr)
	, m_pSpriteBackground(nullptr)
	, m_pTBtnHangameLogin(nullptr)
	, m_pTBtnDnfLogin(nullptr)
	, m_pBtnStart(nullptr)
	, m_pBtnTerminate(nullptr)
	, m_pEbId(nullptr)
	, m_pEbPass(nullptr)
	, m_eTab(Tab::eDnf)
{}

void UI_Login::onInit() {

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

	m_pLabelSource = CoreUIManager_v->getLabel(UI_LOGIN_LABEL_SOURCE);
	m_pLabelDeveloper = CoreUIManager_v->getLabel(UI_LOGIN_LABEL_DEVELOPER);

	m_pGroupLoginBox = CoreUIManager_v->getGroup(UI_LOGIN_GROUP_LOGIN_BOX);
	m_pSpriteBackground = CoreUIManager_v->getSprite(UI_LOGIN_LOGIN_BOX_SPRITE_BACKGROUND);
	m_pTBtnHangameLogin = CoreUIManager_v->getToggleButton(UI_LOGIN_LOGIN_BOX_TOGGLEBUTTON_HANGAME_LOGIN);
	m_pTBtnDnfLogin = CoreUIManager_v->getToggleButton(UI_LOGIN_LOGIN_BOX_TOGGLEBUTTON_DNF_LOGIN);
	m_pBtnStart = CoreUIManager_v->getButton(UI_LOGIN_LOGIN_BOX_BUTTON_START);
	m_pBtnTerminate = CoreUIManager_v->getButton(UI_LOGIN_LOGIN_BOX_BUTTON_TERMINATE);
	m_pEbId = CoreUIManager_v->getEditBox(UI_LOGIN_LOGIN_BOX_EDITBOX_ID);
	m_pEbPass = CoreUIManager_v->getEditBox(UI_LOGIN_LOGIN_BOX_EDITBOX_PW);
	m_pEbPass->setInputFlag(SGEditBox::InputFlag::PASSWORD);
}

void UI_Login::onLoaded() {
	setTab(m_eTab);

	auto g = m_pGroupLoginBox->getContentSize() * 1.2;
	m_pGroupLoginBox->setContentSize(g);
}


void UI_Login::onUpdate(float dt) {
}

void UI_Login::onMouseDown(UIElement* element, SGEventMouse* mouseEvent) {
}

void UI_Login::onMouseUp(UIElement* element, SGEventMouse* mouseEvent) {
	switch (element->getCode()) {
	case UI_LOGIN_LOGIN_BOX_BUTTON_START:
		login();
		break;
	case UI_LOGIN_LOGIN_BOX_BUTTON_TERMINATE:
		CoreWorld_v->terminate();
		break;
	default:
		break;
	}
}

void UI_Login::onToggleStateChanged(UIToggleButton* toggleBtn, ToggleState state) {
	switch (toggleBtn->getCode()) {
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


bool UI_Login::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {

	if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
		login();
	}

	return true;
}

bool UI_Login::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	return true;
}

void UI_Login::onEditBoxEditingDidEndWithAction(UIEditBox* editBox, SGEditBoxEndAction endAction) {
	if (endAction == EditBoxDelegate::EditBoxEndAction::RETURN) {
		login();
	}
}

void UI_Login::setTab(Tab tab) {
	m_eTab = tab;

	if (m_eTab == Tab::eHangame) {
		m_pTBtnHangameLogin->setToggleState(ToggleState::eToggled);
		m_pTBtnDnfLogin->setToggleState(ToggleState::eNormal);
	} else if (m_eTab == Tab::eDnf) {
		m_pTBtnHangameLogin->setToggleState(ToggleState::eNormal);
		m_pTBtnDnfLogin->setToggleState(ToggleState::eToggled);
	}
}

void UI_Login::login() {
	if (m_pEbId->getText().length() == 0) {
		return;
	}

	_LogDebug_("아이디: %s", m_pEbId->getText().data());
	_LogDebug_("패스워드: %s", m_pEbPass->getText().data());

	_LogDebug_("아이디: %s", m_pEbId->getText().c_str());
	_LogDebug_("패스워드: %s", m_pEbPass->getText().c_str());
}



