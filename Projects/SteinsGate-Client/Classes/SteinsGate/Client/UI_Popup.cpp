/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:10:03 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UI_Popup.h"

#include <SteinsGate/Client/UIDefine.h>

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;


void DefaultCallbackMethod() {}

UI_Popup::UI_Popup(UIGroupInfo* groupInfo)
	: UIMasterGroup(groupInfo)
	, m_pGroupHolder(nullptr)
	, m_pSpriteBackground(nullptr)
	, m_pBtnYes(nullptr)
	, m_pBtnNo(nullptr)
	, m_pBtnOk(nullptr)
	, m_pLabelText(nullptr)
	, m_eType(Type::eOk)
	, m_fnYes(DefaultCallbackMethod)
	, m_fnNo(DefaultCallbackMethod)
	, m_fnOk(DefaultCallbackMethod)
{}

void UI_Popup::onInit() {

	// #define POPUP_GROUP_HOLDER
	// #define POPUP_HOLDER_SPRITE_BACKGROUND
	// #define POPUP_HOLDER_BUTTON_YES
	// #define POPUP_HOLDER_BUTTON_NO
	// #define POPUP_HOLDER_BUTTON_OK
	// #define POPUP_HOLDER_LABEL_TEXT

	m_pGroupHolder = CoreUIManager_v->getGroup(POPUP_GROUP_HOLDER);
	m_pSpriteBackground = CoreUIManager_v->getSprite(POPUP_HOLDER_SPRITE_BACKGROUND);
	m_pBtnYes = CoreUIManager_v->getButton(POPUP_HOLDER_BUTTON_YES);
	m_pBtnNo = CoreUIManager_v->getButton(POPUP_HOLDER_BUTTON_NO);
	m_pBtnOk = CoreUIManager_v->getButton(POPUP_HOLDER_BUTTON_OK);
	m_pLabelText = CoreUIManager_v->getLabel(POPUP_HOLDER_LABEL_TEXT);
}

void UI_Popup::onLoaded() {

}

void UI_Popup::setType(Type type) {
	m_eType = type;

	if (type == Type::eOk) {
		m_pBtnNo->setVisible(false);
		m_pBtnYes->setVisible(false);
		m_pBtnOk->setVisible(true);
	}
	else if (type == Type::eYesNo) {
		m_pBtnNo->setVisible(true);
		m_pBtnYes->setVisible(true);
		m_pBtnOk->setVisible(false);
	}
}

void UI_Popup::setPopupSize(const SGSize& size) {
	m_pGroupHolder->setContentSize(size);
}

void UI_Popup::setText(const std::string& text) {
	m_pLabelText->setText(text, _contentSize.width - m_fPadding * 2, 0.0f);

}

SGRect UI_Popup::getTextArea() {
	return {};
}

SGRect UI_Popup::getButtonArea() {
	return {};
}


void UI_Popup::adjust() {
}