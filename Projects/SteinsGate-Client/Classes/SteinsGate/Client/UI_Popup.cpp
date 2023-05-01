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
	, m_bClosed(true)
	, m_fnYes(DefaultCallbackMethod)
	, m_fnNo(DefaultCallbackMethod)
	, m_fnOk(DefaultCallbackMethod)
{}

void UI_Popup::onInit() {

	// #define UI_POPUP_GROUP_HOLDER
	// #define UI_POPUP_HOLDER_SPRITE_BACKGROUND
	// #define UI_POPUP_HOLDER_LABEL_TEXT
	// #define UI_POPUP_HOLDER_GROUP_BTN_HOLDER
	// #define UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_YES
	// #define UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_NO
	// #define UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_OK

	m_pGroupHolder = findGroup(UI_POPUP_GROUP_HOLDER);
	m_pSpriteBackground = findSprite(UI_POPUP_HOLDER_SPRITE_BACKGROUND);
	m_pGroupButtonHolder = findGroup(UI_POPUP_HOLDER_GROUP_BTN_HOLDER);
	m_pBtnYes = findButton(UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_YES);
	m_pBtnNo = findButton(UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_NO);
	m_pBtnOk = findButton(UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_OK);
	m_pLabelText = findLabel(UI_POPUP_HOLDER_LABEL_TEXT);

	m_pGroupButtonHolder->setResizable(false);
}

void UI_Popup::onLoaded() {
	const Size size = m_pSpriteBackground->getUISize();
	const float fBorderThickness = 3.0f;	// 팝업 배경 변경시 수정해줄 것

	m_pSpriteBackground->setCapInsets(
		{ fBorderThickness, fBorderThickness, size.width - fBorderThickness * 2, size.height - fBorderThickness * 2 }
	);
}

void UI_Popup::onMouseUp(UIElement* element, SGEventMouse* mouseEvent) {

	switch (element->getCode()) {
	case UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_YES:
		if (m_fnYes)
			m_fnYes();

		close();
		break;
	case UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_NO:
		if (m_fnNo)
			m_fnNo();

		close();
		break;
	case UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_OK:
		if (m_fnOk)
			m_fnOk();

		close();
		break;
	default:
		break;
	}

}

// 팝업이 활성화된 동안 해당 팝업보다 우선순위(_localZOrder)가 낮은 대상에게는 이벤트가 전파되지 않도록 하기 위해
bool UI_Popup::onMouseDownDetail(SGEventMouse* mouseEvent) { return false; }
bool UI_Popup::onMouseScrollDetail(SGEventMouse* mouseEvent) { return false; }

void UI_Popup::setType(Type type) {
	m_eType = type;

	if (type == Type::eOk) {
		m_pBtnNo->setVisible(false);
		m_pBtnNo->setEnabled(false);
		m_pBtnYes->setVisible(false);
		m_pBtnYes->setEnabled(false);
		m_pBtnOk->setVisible(true);
		m_pBtnOk->setEnabled(true);
	}
	else if (type == Type::eYesNo) {
		m_pBtnNo->setVisible(true);
		m_pBtnNo->setEnabled(true);
		m_pBtnYes->setVisible(true);
		m_pBtnYes->setEnabled(false);
		m_pBtnOk->setVisible(false);
		m_pBtnOk->setEnabled(false);
	}
}

void UI_Popup::setText(const std::string& text) {
	m_pLabelText->setText(text);
}

void UI_Popup::setYesCallback(const SGActionFn<>& fnYes) {
	if (fnYes != nullptr)
		m_fnYes = fnYes;
}

void UI_Popup::setNoCallback(const SGActionFn<>& fnNo) {
	if (fnNo != nullptr)
		m_fnNo = fnNo;
}

void UI_Popup::setOkCallback(const SGActionFn<>& fnOk) {
	if (fnOk != nullptr)
		m_fnOk = fnOk;
}


void UI_Popup::close() {
	CorePopupManager_v->close(this);
}

void UI_Popup::adjust() {

	// 내가 정한 팝업박스 규칙
	// 1. 팝업창의 너비는 최소한 팝업 버튼을 포함할 수 있을 만큼 넓어야한다.
	// 2. 팝업창의 너비는 패딩 길이를 포함한 너비이다.
	// 3. 팝업의 하단부의 버튼영역과 상단부의 텍스트 영역으로 구분한다.
	// 4. 팝업 상단부와 하단부 사이에 패딩 너비를 포함하여 여백의 미를 준다.

	const float fPadding = CorePopupManager_v->getPadding();
	const float fFontSize = m_pLabelText->getFontSize();
	const int iLineCount = m_pLabelText->getLineCount();
	const Size& buttonArea = m_pGroupButtonHolder->getUISize();
	const float fWidth = Math::Max(CorePopupManager_v->getWidth(), buttonArea.width + fPadding * 2);
	const float fHeight = fPadding * 3 + iLineCount * fFontSize + buttonArea.height;
	const Size& textArea = {
		fWidth - fPadding * 2,
		iLineCount * fFontSize
	};

	m_pGroupHolder->setUISize({ fWidth, fHeight });
	m_pGroupHolder->setRelativePosition(0, 0);

	m_pSpriteBackground->setUISize({ fWidth, fHeight });
	m_pSpriteBackground->setRelativePosition(0, 0);

	m_pLabelText->setUISize(textArea);
	m_pLabelText->setRelativePosition(fPadding, -fPadding, HAlignment::Left, VAlignment::Top);
	
	m_pGroupButtonHolder->setRelativePosition(0, fPadding);
}

// 버튼과 패딩
