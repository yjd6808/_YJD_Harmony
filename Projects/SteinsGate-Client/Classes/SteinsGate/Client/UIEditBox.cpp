/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 7:59:06 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "UIEditBox.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/UIRootGroup.h>

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UIEditBox::UIEditBox(UIRootGroup* _pMaster, UIGroup* _pParent)
: UIElement(_pMaster, _pParent)
, m_bFontAutoScaling(true)
, m_fFontSizeInitial(12.0f)
, m_fFontSize(12.0f)
, m_fPlaceholderFontSizeInitial(12.0f)
, m_fPlaceholderFontSize(12.0f)
, m_pInfo(nullptr)
, m_pEditBox{ nullptr }
, m_pListener(nullptr)
{
}

UIEditBox::UIEditBox(UIRootGroup* _pMaster, UIGroup* _pParent, UIEditBoxInfo* _pEditBoxInfo, bool _infoOwner)
: UIEditBox(_pMaster, _pParent)
{
	setInfoEditBox(_pEditBoxInfo, _infoOwner);
}

UIEditBox::~UIEditBox()
{
	JCORE_DELETE_SAFE(m_pListener);
}

UIEditBox* UIEditBox::create(UIRootGroup* _pMaster, UIGroup* _pParent)
{
	UIEditBox* pEditBox = dbg_new UIEditBox(_pMaster, _pParent);
	pEditBox->init();
	pEditBox->autorelease();
	return pEditBox;
}

UIEditBox* UIEditBox::create(UIRootGroup* _pMaster, UIGroup* _pParent, UIEditBoxInfo* _pEditBoxInfo, bool _infoOwner)
{
	UIEditBox* pEditBox = dbg_new UIEditBox(_pMaster, _pParent, _pEditBoxInfo, _infoOwner);
	pEditBox->init();
	pEditBox->autorelease();
	return pEditBox;
}

std::string UIEditBox::getText()
{
	return m_pEditBox->getText();
}

const char* UIEditBox::getTextRaw()
{
	return m_pEditBox->getText();
}

bool UIEditBox::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	if (m_pInfo == nullptr)
	{
		LogWarnMissingInfo();
		return false;
	}

	SetInitialUISize(m_pInfo->Size);

	// 에딧박스는 좌우 패딩 5씩 줘서 실제 컨텐츠 사이즈는 너비가 10 작아진다.
	// EditBoxImplCommon::setContentSize(const Size& size) 함수에서 확인가능
	//  => 패딩 0으로 없앰

	m_pEditBox = EditBox::create(uiSize_, "");
	m_pListener = dbg_new Listener(this);

	m_pEditBoxImpl = (SGEditBoxImplWin*)m_pEditBox->getImpl();
	m_hNativeHandle = m_pEditBoxImpl->getNativeHandle();
	m_pLabel = m_pEditBoxImpl->getLabel();
	m_pLabelPlaceholder = m_pEditBoxImpl->getLabelPlaceholder();
	m_pLabelPlaceholder->setAlignment(TextHAlignment(m_pInfo->TextHAlignment));
	m_pLabelPlaceholder->setDimensions(uiSize_.width, uiSize_.height);

	m_pEditBox->setFontColor(m_pInfo->FontColor);
	m_pEditBox->setFontSize(int(m_fFontSizeInitial));
	m_pEditBox->setTextHorizontalAlignment(TextHAlignment(m_pInfo->TextHAlignment));
	m_pEditBox->setPlaceHolder(m_pInfo->PlaceholderText.Source());
	m_pEditBox->setPlaceholderFontColor(m_pInfo->PlaceHolderFontColor);
	m_pEditBox->setPlaceholderFontSize(m_fPlaceholderFontSizeInitial);
	m_pEditBox->setAnchorPoint(Vec2::ZERO);
	m_pEditBox->setMaxLength(m_pInfo->MaxLength);
	m_pEditBox->setInputMode(m_pInfo->InputMode);
	m_pEditBox->setDelegate(m_pListener);
	m_pEditBox->setPassiveFocusable(true);
	this->addChild(m_pEditBox);

	return isInitialized_ = true;
}

void UIEditBox::SetInitialUISize(SGSize _size)
{
	UIElement::SetInitialUISize(_size);

	m_fFontSize = m_pInfo->FontSize * Core::ClientInfo->uiScaleYFactor_;
	m_fFontSizeInitial = m_fFontSize;

	m_fPlaceholderFontSize = m_pInfo->PlaceholderFontSize * Core::ClientInfo->uiScaleYFactor_;
	m_fPlaceholderFontSizeInitial = m_fPlaceholderFontSize;
}

void UIEditBox::setMaxLength(int _maxLength)
{
	m_pEditBox->setMaxLength(_maxLength);
}

void UIEditBox::setTextEditBeginCallback(const SGActionFn<UIEditBox*>& _fnTextEditBegin) const
{
	m_pListener->FnEditBoxEditingDidBegin = _fnTextEditBegin;
}

void UIEditBox::setTextChangedCallback(const SGActionFn<UIEditBox*, const SGString&>& _fnTextChanged) const
{
	m_pListener->FnEditBoxTextChanged = _fnTextChanged;
}

void UIEditBox::setReturnCallback(const SGActionFn<UIEditBox*>& _fnEditBoxReturn) const
{
	m_pListener->FnEditBoxReturn = _fnEditBoxReturn;
}

void UIEditBox::setLoseFocusCallback(const SGActionFn<UIEditBox*, SGEditBoxEndAction>& _fnLoseFocus) const
{
	m_pListener->FnEditBoxEditingDidEndWithAction = _fnLoseFocus;
}

void UIEditBox::SetUISize(const SGSize& _size)
{
	if (!isResizable_)
	{
		return;
	}

	uiSize_ = _size;

	// 에딧박스의 컨텐트 사이즈를 변경하면 라벨은 변경되는데, 플레이스홀더 라벨은 사이즈 변경이 안되서 수동으로 해줘야함.
	// 에딧박스의 컨텐트 사이즈를 변경하더라도 라벨의 폰트 크기가 변경되지도 않는다.
	// EditBoxImplCommon::setContentSize 참조
	m_pEditBox->setContentSize(_size);
	m_pLabelPlaceholder->setDimensions(_size.width, _size.height);

	if (m_bFontAutoScaling)
	{
		const float scaleY = GetUIScaleY();

		m_fFontSize = m_fFontSizeInitial * scaleY;
		m_fPlaceholderFontSize = m_fPlaceholderFontSizeInitial * scaleY;

		m_pLabel->setSystemFontSize(m_fFontSize);
		m_pLabelPlaceholder->setSystemFontSize(m_fPlaceholderFontSize);
	}
}

void UIEditBox::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->Type != UIElementType::EditBox)
	{
		LogWarnInvalidInfo(_pInfo->Type);
		return;
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(m_pInfo);
	}

	pBaseInfo_ = _pInfo;
	m_pInfo = static_cast<UIEditBoxInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UIEditBox::setInfoEditBox(UIEditBoxInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

void UIEditBox::Focus()
{
	m_pEditBox->openKeyboard();
}

void UIEditBox::setInputFlag(SGEditBox::InputFlag _inputFlag)
{
	m_pEditBox->setInputFlag(_inputFlag);
}

void UIEditBox::setInputMode(SGEditBox::InputMode _inputMode)
{
	m_pEditBox->setInputMode(_inputMode);
}

bool UIEditBox::OnMouseUpContainedInternalDetail(SGEventMouse* /*_pMouseEvent*/)
{
	m_pEditBox->openKeyboard();
	return false;
}

void UIEditBox::Listener::editBoxEditingDidBegin(SGEditBox* /* editBox */)
{
	if (FnEditBoxEditingDidBegin)
	{
		FnEditBoxEditingDidBegin(EditBox);
	}

	EditBox->pRootGroup_->OnEditBoxEditingDidBegin(EditBox);
}

void UIEditBox::Listener::nativeEditBoxFocused(SGEditBox* /* editBox */)
{
}

void UIEditBox::Listener::editBoxTextChanged(SGEditBox* /* editBox */, const std::string& _text)
{
	if (FnEditBoxTextChanged)
	{
		FnEditBoxTextChanged(EditBox, _text.c_str());
	}

	EditBox->pRootGroup_->OnEditBoxTextChanged(EditBox, _text);
}

void UIEditBox::Listener::editBoxReturn(SGEditBox* /* editBox */)
{
	if (FnEditBoxReturn)
	{
		FnEditBoxReturn(EditBox);
	}

	EditBox->pRootGroup_->OnEditBoxReturn(EditBox);
}

void UIEditBox::Listener::editBoxEditingDidEndWithAction(SGEditBox* /* editBox */, EditBoxEndAction _editBoxEndAction)
{
	if (FnEditBoxEditingDidEndWithAction)
	{
		FnEditBoxEditingDidEndWithAction(EditBox, _editBoxEndAction);
	}

	EditBox->pRootGroup_->OnEditBoxEditingDidEndWithAction(EditBox, _editBoxEndAction);
}
