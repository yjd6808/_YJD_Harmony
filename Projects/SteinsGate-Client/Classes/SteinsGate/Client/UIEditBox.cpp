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
UIEditBox::UIEditBox(UIRootGroup* _pRoot, UIGroup* _pParent)
: UIElement(_pRoot, _pParent)
, isFontAutoScaling_(true)
, fontSizeInitial_(12.0f)
, fontSize_(12.0f)
, placeholderFontSizeInitial_(12.0f)
, placeholderFontSize_(12.0f)
, pInfo_(nullptr)
, pEditBox_{ nullptr }
, pListener_(nullptr)
{
}

UIEditBox::UIEditBox(UIRootGroup* _pRoot, UIGroup* _pParent, UIEditBoxInfo* _pEditBoxInfo, bool _infoOwner)
: UIEditBox(_pRoot, _pParent)
{
	SetInfoEditBox(_pEditBoxInfo, _infoOwner);
}

UIEditBox::~UIEditBox()
{
	JCORE_DELETE_SAFE(pListener_);
}

UIEditBox* UIEditBox::Create(UIRootGroup* _pRoot, UIGroup* _pParent)
{
	UIEditBox* pEditBox = dbg_new UIEditBox(_pRoot, _pParent);
	pEditBox->init();
	pEditBox->autorelease();
	return pEditBox;
}

UIEditBox* UIEditBox::Create(UIRootGroup* _pRoot, UIGroup* _pParent, UIEditBoxInfo* _pEditBoxInfo, bool _infoOwner)
{
	UIEditBox* pEditBox = dbg_new UIEditBox(_pRoot, _pParent, _pEditBoxInfo, _infoOwner);
	pEditBox->init();
	pEditBox->autorelease();
	return pEditBox;
}

std::string UIEditBox::GetText()
{
	return pEditBox_->getText();
}

const char* UIEditBox::GetTextRaw()
{
	return pEditBox_->getText();
}

bool UIEditBox::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	if (pInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return false;
	}

	SetInitialUISize(pInfo_->Size);

	// 에딧박스는 좌우 패딩 5씩 줘서 실제 컨텐츠 사이즈는 너비가 10 작아진다.
	// EditBoxImplCommon::setContentSize(const Size& size) 함수에서 확인가능
	//  => 패딩 0으로 없앰

	pEditBox_ = EditBox::create(uiSize_, "");
	pListener_ = dbg_new Listener(this);

	pEditBoxImpl_ = (SGEditBoxImplWin*)pEditBox_->getImpl();
	nativeHandle_ = pEditBoxImpl_->getNativeHandle();
	pLabel_ = pEditBoxImpl_->getLabel();
	pLabelPlaceHolder_ = pEditBoxImpl_->getLabelPlaceholder();
	pLabelPlaceHolder_->setAlignment(TextHAlignment(pInfo_->TextHAlignment));
	pLabelPlaceHolder_->setDimensions(uiSize_.width, uiSize_.height);

	pEditBox_->setFontColor(pInfo_->FontColor);
	pEditBox_->setFontSize(int(fontSizeInitial_));
	pEditBox_->setTextHorizontalAlignment(TextHAlignment(pInfo_->TextHAlignment));
	pEditBox_->setPlaceHolder(pInfo_->PlaceholderText.Source());
	pEditBox_->setPlaceholderFontColor(pInfo_->PlaceHolderFontColor);
	pEditBox_->setPlaceholderFontSize(placeholderFontSizeInitial_);
	pEditBox_->setAnchorPoint(Vec2::ZERO);
	pEditBox_->setMaxLength(pInfo_->MaxLength);
	pEditBox_->setInputMode(pInfo_->InputMode);
	pEditBox_->setDelegate(pListener_);
	pEditBox_->setPassiveFocusable(true);
	this->addChild(pEditBox_);

	return isInitialized_ = true;
}

void UIEditBox::SetInitialUISize(SGSize _size)
{
	UIElement::SetInitialUISize(_size);

	fontSize_ = pInfo_->FontSize * Core::ClientInfo->uiScaleYFactor_;
	fontSizeInitial_ = fontSize_;

	placeholderFontSize_ = pInfo_->PlaceholderFontSize * Core::ClientInfo->uiScaleYFactor_;
	placeholderFontSizeInitial_ = placeholderFontSize_;
}

void UIEditBox::SetMaxLength(int _maxLength)
{
	pEditBox_->setMaxLength(_maxLength);
}

void UIEditBox::SetTextEditBeginCallback(const SGActionFn<UIEditBox*>& _fnTextEditBegin) const
{
	pListener_->fnEditBoxEditingDidBegin_ = _fnTextEditBegin;
}

void UIEditBox::SetTextChangedCallback(const SGActionFn<UIEditBox*, const SGString&>& _fnTextChanged) const
{
	pListener_->fnEditBoxTextChanged_ = _fnTextChanged;
}

void UIEditBox::SetReturnCallback(const SGActionFn<UIEditBox*>& _fnEditBoxReturn) const
{
	pListener_->fnEditBoxReturn_ = _fnEditBoxReturn;
}

void UIEditBox::SetLoseFocusCallback(const SGActionFn<UIEditBox*, SGEditBoxEndAction>& _fnLoseFocus) const
{
	pListener_->fnEditBoxEditingDidEndWithAction_ = _fnLoseFocus;
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
	pEditBox_->setContentSize(_size);
	pLabelPlaceHolder_->setDimensions(_size.width, _size.height);

	if (isFontAutoScaling_)
	{
		const float scaleY = GetUIScaleY();

		fontSize_ = fontSizeInitial_ * scaleY;
		placeholderFontSize_ = placeholderFontSizeInitial_ * scaleY;

		pLabel_->setSystemFontSize(fontSize_);
		pLabelPlaceHolder_->setSystemFontSize(placeholderFontSize_);
	}
}

void UIEditBox::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->type_ != UIElementType::EditBox)
	{
		LogWarnInvalidInfo(_pInfo->type_);
		return;
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(pInfo_);
	}

	pBaseInfo_ = _pInfo;
	pInfo_ = static_cast<UIEditBoxInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UIEditBox::SetInfoEditBox(UIEditBoxInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

void UIEditBox::Focus()
{
	pEditBox_->openKeyboard();
}

void UIEditBox::SetInputFlag(SGEditBox::InputFlag _inputFlag)
{
	pEditBox_->setInputFlag(_inputFlag);
}

void UIEditBox::SetInputMode(SGEditBox::InputMode _inputMode)
{
	pEditBox_->setInputMode(_inputMode);
}

bool UIEditBox::OnMouseUpContainedInternalDetail(SGEventMouse* /*_pMouseEvent*/)
{
	pEditBox_->openKeyboard();
	return false;
}

void UIEditBox::Listener::editBoxEditingDidBegin(SGEditBox* /* editBox */)
{
	if (fnEditBoxEditingDidBegin_)
	{
		fnEditBoxEditingDidBegin_(pEditBox_);
	}

	pEditBox_->pRootGroup_->OnEditBoxEditingDidBegin(pEditBox_);
}

void UIEditBox::Listener::nativeEditBoxFocused(SGEditBox* /* editBox */)
{
}

void UIEditBox::Listener::editBoxTextChanged(SGEditBox* /* editBox */, const std::string& _text)
{
	if (fnEditBoxTextChanged_)
	{
		fnEditBoxTextChanged_(pEditBox_, _text.c_str());
	}

	pEditBox_->pRootGroup_->OnEditBoxTextChanged(pEditBox_, _text);
}

void UIEditBox::Listener::editBoxReturn(SGEditBox* /* editBox */)
{
	if (fnEditBoxReturn_)
	{
		fnEditBoxReturn_(pEditBox_);
	}

	pEditBox_->pRootGroup_->OnEditBoxReturn(pEditBox_);
}

void UIEditBox::Listener::editBoxEditingDidEndWithAction(SGEditBox* /* editBox */, EditBoxEndAction _editBoxEndAction)
{
	if (fnEditBoxEditingDidEndWithAction_)
	{
		fnEditBoxEditingDidEndWithAction_(pEditBox_, _editBoxEndAction);
	}

	pEditBox_->pRootGroup_->OnEditBoxEditingDidEndWithAction(pEditBox_, _editBoxEndAction);
}
