/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 7:59:06 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "UIEditBox.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/UIMasterGroup.h>

#include "ui/UIEditBox/UIEditBoxImpl-common.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

UIEditBox::UIEditBox(UIMasterGroup* master, UIGroup* parent, UIEditBoxInfo* editBoxInfo)
	: UIElement(master, parent, editBoxInfo)
	, m_pInfo(editBoxInfo)
	, m_pEditBox{nullptr}
	, m_pListener(nullptr) {
}

UIEditBox::~UIEditBox() {
	DeleteSafe(m_pListener);
}

UIEditBox* UIEditBox::create(UIMasterGroup* master, UIGroup* parent, UIEditBoxInfo* editBoxInfo) {
	UIEditBox* pEditBox = dbg_new UIEditBox(master, parent, editBoxInfo);
	pEditBox->init();
	pEditBox->autorelease();
	return pEditBox;
}

std::string UIEditBox::getText() {
	return m_pEditBox->getText();
}

bool UIEditBox::init() {

	// 에딧박스는 좌우 패딩 5씩 줘서 실제 컨텐츠 사이즈는 너비가 10 작아진다.
	// EditBoxImplCommon::setContentSize(const Size& size) 함수에서 확인가능
	//  => 패딩 0으로 없앰

	m_pEditBox = EditBox::create(m_pInfo->Size, "");
	m_pListener = dbg_new Listener(this);

	m_pImpl = (SGEditBoxImplWin*)m_pEditBox->getImpl();
	m_hNativeHandle = m_pImpl->getNativeHandle();
	m_pLabel = m_pImpl->getLabel();
	m_pLabelPlaceholder = m_pImpl->getLabelPlaceholder();
	m_pLabelPlaceholder->setAlignment(TextHAlignment(m_pInfo->TextHAlignment));
	m_pLabelPlaceholder->setDimensions(m_pInfo->Size.width, m_pInfo->Size.height);

	m_pEditBox->setFontColor(m_pInfo->FontColor);
	m_pEditBox->setFontSize(m_pInfo->FontSize);
	m_pEditBox->setTextHorizontalAlignment(TextHAlignment(m_pInfo->TextHAlignment));
	m_pEditBox->setTextHorizontalAlignment(TextHAlignment::RIGHT);
	m_pEditBox->setPlaceHolder(m_pInfo->PlaceholderText.Source());
	m_pEditBox->setPlaceholderFontColor(m_pInfo->PlaceHolderFontColor);
	m_pEditBox->setPlaceholderFontSize(m_pInfo->PlaceholderFontSize);
	m_pEditBox->setAnchorPoint(Vec2::ZERO);
	m_pEditBox->setMaxLength(m_pInfo->MaxLength);
	m_pEditBox->setInputMode(m_pInfo->InputMode);
	m_pEditBox->setDelegate(m_pListener);
	this->addChild(m_pEditBox);

	setInitialUISize(m_pInfo->Size);
	return m_bInitialized = true;
}

void UIEditBox::setTextEditBeginCallback(const SGActionFn<UIEditBox*>& fnTextEditBegin) const {
	m_pListener->FnEditBoxEditingDidBegin = fnTextEditBegin;
}

void UIEditBox::setTextChangedCallback(const SGActionFn<UIEditBox*, const SGString&>& fnTextChanged) const {
	m_pListener->FnEditBoxTextChanged = fnTextChanged;
}

void UIEditBox::setReturnCallback(const SGActionFn<UIEditBox*>& fnEditBoxReturn) const {
	m_pListener->FnEditBoxReturn = fnEditBoxReturn;
}

void UIEditBox::setLoseFocusCallback(const SGActionFn<UIEditBox*, SGEditBoxEndAction>& fnLoseFocus) const {
	m_pListener->FnEditBoxEditingDidEndWithAction = fnLoseFocus;
}

void UIEditBox::setUISize(const SGSize& size) {
	if (!m_bResizable)
		return;

	Size prevSize = m_UISize;
	UIElement::setContentSize(size);

	// 에딧박스의 컨텐트 사이즈를 변경하면 라벨은 변경되는데, 플레이스홀더 라벨은 사이즈 변경이 안되서 수동으로 해줘야함.
	// EditBoxImplCommon::setContentSize 참조

	m_pEditBox->setContentSize(size);
	m_pLabelPlaceholder->setDimensions(size.width, size.height);

	float fScaleX = m_UISize.width / prevSize.width;
	float fScaleY = m_UISize.height / prevSize.height;

	// TODO: 사이즈 변경에 따라 폰트 크기도 알맞게 변경 되어야한다.
}

void UIEditBox::focus() {
	m_pEditBox->openKeyboard();
}

void UIEditBox::setInputFlag(SGEditBox::InputFlag inputFlag) {
	m_pEditBox->setInputFlag(inputFlag);
}

void UIEditBox::setInputMode(SGEditBox::InputMode inputMode) {
	m_pEditBox->setInputMode(inputMode);
}


void UIEditBox::Listener::editBoxEditingDidBegin(SGEditBox* /* editBox */) {
	if (FnEditBoxEditingDidBegin)
		FnEditBoxEditingDidBegin(EditBox);

	

	EditBox->m_pMasterGroup->onEditBoxEditingDidBegin(EditBox);
}

void UIEditBox::Listener::nativeEditBoxFocused(SGEditBox* /* editBox */) {
}

void UIEditBox::Listener::editBoxTextChanged(SGEditBox* /* editBox */, const std::string& str) {
	if (FnEditBoxTextChanged)
		FnEditBoxTextChanged(EditBox, str.c_str());

	EditBox->m_pMasterGroup->onEditBoxTextChanged(EditBox, str);
}

void UIEditBox::Listener::editBoxReturn(SGEditBox* /* editBox */) {
	if (FnEditBoxReturn)
		FnEditBoxReturn(EditBox);

	EditBox->m_pMasterGroup->onEditBoxReturn(EditBox);
}

void UIEditBox::Listener::editBoxEditingDidEndWithAction(SGEditBox* /* editBox */, EditBoxEndAction editBoxEndAction) {
	if (FnEditBoxEditingDidEndWithAction)
		FnEditBoxEditingDidEndWithAction(EditBox, editBoxEndAction);

	EditBox->m_pMasterGroup->onEditBoxEditingDidEndWithAction(EditBox, editBoxEndAction);
}
