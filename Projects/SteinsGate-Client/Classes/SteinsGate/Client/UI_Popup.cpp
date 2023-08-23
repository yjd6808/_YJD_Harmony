/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:10:03 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UI_Popup.h"

#include <SteinsGate/Client/Define_UI.h>

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

UI_Popup::UI_Popup(UIGroupInfo* groupInfo)
	: UIMasterGroup(groupInfo)
	, m_pGroupHolder(nullptr)
	, m_pSpriteBackground(nullptr)
	, m_pGroupButtonHolder(nullptr)
	, m_pBtnYes(nullptr)
	, m_pBtnNo(nullptr)
	, m_pBtnOk(nullptr)
	, m_pLabelText(nullptr)
	, m_eType(Type::eNone)
	, m_fTimeout(-1.f)
	, m_bClosed(true)
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

	m_pLabelText->setEnableFontAutoScaling(false);
	m_pGroupButtonHolder->setResizable(false);
	m_pSpriteBackground->setDraggable(true);
	m_pSpriteBackground->setDragLinkElement(m_pGroupHolder);
	
}

void UI_Popup::onLoaded() {
	const Size size = m_pSpriteBackground->getUISize();
	const float fBorderThickness = 3.0f;	// 팝업 배경 변경시 수정해줄 것

	m_pSpriteBackground->setCapInsets(
		{ fBorderThickness, fBorderThickness, size.width - fBorderThickness * 2, size.height - fBorderThickness * 2 }
	);
	setType(m_eType);
}

void UI_Popup::onAdded() {
	m_OpenedTime.Elapsed.Second = 0;
}

void UI_Popup::onRemoved() {
	close();
}

void UI_Popup::onUpdate(float dt) {
	if (!m_AttributeFlag.Check(eTimeout)) {
		return;
	}

	m_OpenedTime.Elapsed.Second += dt;

	if (m_OpenedTime.ElapsedSeconds(m_fTimeout) && m_fnTimeout) {
		m_fnTimeout();
		close();
	}
}

void UI_Popup::onMouseUpTarget(UIElement* element, SGEventMouse* mouseEvent) {

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

bool UI_Popup::onMouseMoveInternalDetail(SGEventMouse* mouseEvent) {
	return false;
}

// 팝업이 활성화된 동안 해당 팝업보다 우선순위(_localZOrder)가 낮은 대상에게는 이벤트가 전파되지 않도록 하기 위해
bool UI_Popup::onMouseDownInternalDetail(SGEventMouse* mouseEvent) {
	return false;
}
bool UI_Popup::onMouseScrollInternalDetail(SGEventMouse* mouseEvent) {
	return false;
}
bool UI_Popup::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {

	if (m_AttributeFlag.Check(eCloseWithEsc) && keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
		close();
		return false;
	}

	return true;
}
bool UI_Popup::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	return true;
}

void UI_Popup::setType(Type type) {
	m_eType = type;

	if (type == Type::eOk) {
		m_pBtnNo->setVisible(false);
		m_pBtnNo->setEnabled(false);
		m_pBtnYes->setVisible(false);
		m_pBtnYes->setEnabled(false);
		m_pBtnOk->setVisible(true);
		m_pBtnOk->setEnabled(true);
	} else if (type == Type::eYesNo) {
		m_pBtnNo->setVisible(true);
		m_pBtnNo->setEnabled(true);
		m_pBtnYes->setVisible(true);
		m_pBtnYes->setEnabled(true);
		m_pBtnOk->setVisible(false);
		m_pBtnOk->setEnabled(false);
	} else if (type == Type::eNone) {
		m_pBtnNo->setVisible(false);
		m_pBtnNo->setEnabled(false);
		m_pBtnYes->setVisible(false);
		m_pBtnYes->setEnabled(false);
		m_pBtnOk->setVisible(false);
		m_pBtnOk->setEnabled(false);
	}
}

void UI_Popup::setText(const std::string& text) {
	m_pLabelText->setText(text);
}

void UI_Popup::setYesCallback(const PopupCallback& fnYes) {
	m_fnYes = fnYes;
}

void UI_Popup::setNoCallback(const PopupCallback& fnNo) {
	m_fnNo = fnNo;
}

void UI_Popup::setOkCallback(const PopupCallback& fnOk) {
	m_fnOk = fnOk;
}

void UI_Popup::setCloseWithEsc(bool closeWithEsc) {
	if (closeWithEsc) {
		m_AttributeFlag.Add(eCloseWithEsc);
	} else {
		m_AttributeFlag.Unset(eCloseWithEsc);
	}
}

void UI_Popup::setTimeoutCallback(const PopupCallback& fnTimeout) {
	m_fnTimeout = fnTimeout;
}

void UI_Popup::setTimeout(float timeout) {
	if (timeout >= 0.0f) {
		m_fTimeout = timeout;
		m_AttributeFlag.Add(eTimeout);
	} else {
		m_AttributeFlag.Unset(eTimeout);
	}
}


void UI_Popup::close() {
	Core::Contents.PopupManager->close(this);
}



void UI_Popup::adjust() {

	// 내가 정한 팝업박스 규칙
	// 1. 팝업창의 너비는 최소한 팝업 버튼을 포함할 수 있을 만큼 넓어야한다.
	// 2. 팝업창의 너비는 패딩 길이를 포함한 너비이다.
	// 3. 팝업의 하단부의 버튼영역과 상단부의 텍스트 영역으로 구분한다.
	// 4. 팝업 상단부와 하단부 사이에 패딩 너비를 포함하여 여백의 미를 준다.
	const float fPadding = Core::Contents.PopupManager->getPadding();
	const Size& buttonArea = m_pGroupButtonHolder->getUISize();	
	const float fPopupAreaWidth = Math::Max(Core::Contents.PopupManager->getWidth(), buttonArea.width + fPadding * 2);
	const float fPopupTextAreaWidth = fPopupAreaWidth - fPadding * 2;

	// 라벨의 Dimension이 UI 설정파일상의 너비, 높이 정보로 처음 세팅되기 때문에
	// setText()후 getLineCount()를 수행할때 이 기본 정보를 바탕으로 라인수를 얻게된다.
	// 팝업매니저에서 설정한 팝업의 기본너비에 맞게 라벨의 너비정보도 수정해줘야
	// 텍스트 수정 후 Cocos2d-x 엔진이 너비에 맞춰서 올바르게 계산된 라인수를 얻어낼 수 있게된다.

	// 1. 먼저 텍스트영역 너비를 결정해줘서 라인카운트를 올바르게 계산할 수 있도록 만듬
	m_pLabelText->source()->setDimensions(fPopupTextAreaWidth, 0);

	// 2. 위 작업 덕분에 게임엔진의 라벨이 해당 너비를 기준으로 올바르게 라인수를 계산할 수 있게된다.
	const int iLineCount = m_pLabelText->getLineCount();		
	const float fFontSize = m_pLabelText->getInitialFontSize();
	const float fPopupAreaHeight = fPadding * 3 + iLineCount * fFontSize + buttonArea.height;

	Size textArea = {
		fPopupTextAreaWidth,
		iLineCount * fFontSize
	};

	// 논타입인 경우 버튼이 없으므로, 버튼 영역까지 높이를 확장해준다.
	if (m_eType == Type::eNone)
		textArea.height += buttonArea.height + fPadding;

	m_pGroupHolder->setUISize({ fPopupAreaWidth, fPopupAreaHeight });
	m_pGroupHolder->setRelativePosition(0, 0);

	m_pSpriteBackground->setUISize({ fPopupAreaWidth, fPopupAreaHeight });
	m_pSpriteBackground->setRelativePosition(0, 0);

	m_pLabelText->setUISize(textArea);
	m_pLabelText->setRelativePosition(fPadding, -fPadding, HAlignment::Left, VAlignment::Top);
	
	m_pGroupButtonHolder->setRelativePosition(0, fPadding, HAlignment::Center, VAlignment::Bottom);
}

// 버튼과 패딩
