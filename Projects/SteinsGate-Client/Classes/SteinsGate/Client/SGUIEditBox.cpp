/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 7:59:06 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGUIEditBox.h"
#include "GameCoreHeader.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

SGUIEditBox::SGUIEditBox(SGUIGroup* parent, SGUIEditBoxInfo* editBoxInfo)
	: SGUIElement(parent)
	, m_pInfo(editBoxInfo)
	, m_pEditBox{ nullptr }
{}

SGUIEditBox::~SGUIEditBox() {
	DeleteSafe(m_pListener);
}

SGUIEditBox* SGUIEditBox::create(SGUIGroup* parent, SGUIEditBoxInfo* editBoxInfo) {
	SGUIEditBox* pEditBox = dbg_new SGUIEditBox(parent, editBoxInfo);
	pEditBox->init();
	pEditBox->autorelease();
	return pEditBox;
}

bool SGUIEditBox::init() {

	// 에딧박스는 좌우 패딩 5씩 줘서 실제 컨텐츠 사이즈는 너비가 10 작아진다.
	// EditBoxImplCommon::setContentSize(const Size& size) 함수에서 확인가능
	m_pEditBox = EditBox::create(m_pInfo->Size, "");
	m_pListener = dbg_new Listener(this);

	m_pEditBox->setFontColor(m_pInfo->FontColor);
	m_pEditBox->setFontSize(m_pInfo->FontSize);
	m_pEditBox->setTextHorizontalAlignment(TextHAlignment(m_pInfo->TextHAlignment));
	m_pEditBox->setPlaceHolder(m_pInfo->PlaceholderText.Source());
	m_pEditBox->setPlaceholderFontColor(m_pInfo->PlaceHolderFontColor);
	m_pEditBox->setPlaceholderFontSize(m_pInfo->PlaceholderFontSize);
	m_pEditBox->setAnchorPoint(Vec2::ZERO);
	m_pEditBox->setMaxLength(m_pInfo->MaxLength);
	m_pEditBox->setInputMode(m_pInfo->InputMode);
	m_pEditBox->setDelegate(m_pListener);
	this->addChild(m_pEditBox);


	return true;
}

int SGUIEditBox::getCode() {
	return m_pInfo->Code;
}

void SGUIEditBox::setTextEditBeginCallback(const SGActionFn<SGUIEditBox*>& fnTextEditBegin) {
	m_pListener->FnEditBoxEditingDidBegin = fnTextEditBegin;
}

void SGUIEditBox::setTextChangedCallback(const SGActionFn<SGUIEditBox*, const SGString&>& fnTextChanged) {
	m_pListener->FnEditBoxTextChanged = fnTextChanged;
}

void SGUIEditBox::setReturnCallback(const SGActionFn<SGUIEditBox*>& fnEditBoxReturn) {
	m_pListener->FnEditBoxReturn = fnEditBoxReturn;
}

void SGUIEditBox::setLoseFocusCallback(const SGActionFn<SGUIEditBox*, SGEditBoxEndAction>& fnLoseFocus) {
	m_pListener->FnEditBoxEditingDidEndWithAction = fnLoseFocus;
}

void SGUIEditBox::focus() {
	m_pEditBox->openKeyboard();
}


void SGUIEditBox::Listener::editBoxEditingDidBegin(SGEditBox* /* editBox */) {
	if (FnEditBoxEditingDidBegin)
		FnEditBoxEditingDidBegin(EditBox);
}

void SGUIEditBox::Listener::editBoxTextChanged(SGEditBox* /* editBox */, const std::string& str) {
	if (FnEditBoxTextChanged)
		FnEditBoxTextChanged(EditBox, str.c_str());
}

void SGUIEditBox::Listener::editBoxReturn(SGEditBox* /* editBox */) {
	if (FnEditBoxReturn)
		FnEditBoxReturn(EditBox);
}

void SGUIEditBox::Listener::editBoxEditingDidEndWithAction(SGEditBox* /* editBox */, EditBoxEndAction editBoxEndAction) {
	if (FnEditBoxEditingDidEndWithAction)
		FnEditBoxEditingDidEndWithAction(EditBox, editBoxEndAction);
}
