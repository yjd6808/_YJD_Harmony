#include "GameCoreHeader.h"
#include "Game/UI_Implementation/UI_Popup.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

UI_Popup::UI_Popup(UIGroupInfo* _pGroupInfo)
: UIRootGroup(_pGroupInfo)
, pGroupHolder_(nullptr)
, pSpriteBackground_(nullptr)
, pGroupButtonHolder_(nullptr)
, pBtnYes_(nullptr)
, pBtnNo_(nullptr)
, pBtnOk_(nullptr)
, pLabelText_(nullptr)
, type_(Type::eNone)
, timeout_(-1.0f)
, closed_(true)
{
}

void UI_Popup::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);

	pGroupHolder_ = FindElementByName<UIGroup>("groupHolder");
	pSpriteBackground_ = FindElementByName<UISprite>("spriteBackground");
	pGroupButtonHolder_ = FindElementByName<UIGroup>("groupBtnHolder");
	pBtnYes_ = FindElementByName<UIButton>("btnYes");
	pBtnNo_ = FindElementByName<UIButton>("btnNo");
	pBtnOk_ = FindElementByName<UIButton>("btnOk");
	pLabelText_ = FindElementByName<UILabel>("labelText");

	pLabelText_->setEnableFontAutoScaling(false);
	pGroupButtonHolder_->SetResizable(false);
	pSpriteBackground_->SetDraggable(true);
	pSpriteBackground_->SetDragLinkElement(pGroupHolder_);
}

void UI_Popup::OnLoaded()
{
	const Size backgroundSize = pSpriteBackground_->GetUISize();
	const float borderThickness = 3.0f;

	pSpriteBackground_->SetCapInsets(
		{
			borderThickness, borderThickness, backgroundSize.width - borderThickness * 2,
			backgroundSize.height - borderThickness * 2
		}
	);
	SetType(type_);
}

void UI_Popup::OnAdded()
{
	openedTime_.Elapsed.Second = 0;
}

void UI_Popup::OnRemoved()
{
	Close();
}

void UI_Popup::OnUpdate(float _dt)
{
	if (!attributeFlag_.Check(eTimeout))
	{
		return;
	}

	openedTime_.Elapsed.Second += _dt;

	if (openedTime_.ElapsedSeconds(timeout_))
	{
		if (timeoutCallback_)
		{
			timeoutCallback_();
		}
		Close();
	}
}

void UI_Popup::OnMouseUpTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent)
{
	const char* name = _pElement->GetName();

	if (strcmp(name, "btnYes") == 0)
	{
		if (yesCallback_)
		{
			yesCallback_();
		}
		Close();
	}
	else if (strcmp(name, "btnNo") == 0)
	{
		if (noCallback_)
		{
			noCallback_();
		}
		Close();
	}
	else if (strcmp(name, "btnOk") == 0)
	{
		if (okCallback_)
		{
			okCallback_();
		}
		Close();
	}
}

bool UI_Popup::OnMouseMoveInternalDetail(cc::EventMouse* _pMouseEvent)
{
	return false;
}

bool UI_Popup::OnMouseDownInternalDetail(cc::EventMouse* _pMouseEvent)
{
	return false;
}

bool UI_Popup::OnMouseScrollInternalDetail(cc::EventMouse* _pMouseEvent)
{
	return false;
}

bool UI_Popup::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	if (attributeFlag_.Check(eCloseWithEsc) && _keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		Close();
		return false;
	}

	return true;
}

bool UI_Popup::OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	return true;
}

void UI_Popup::SetType(Type _type)
{
	type_ = _type;

	if (_type == Type::eOk)
	{
		pBtnNo_->setVisible(false);
		pBtnNo_->SetEnabled(false);
		pBtnYes_->setVisible(false);
		pBtnYes_->SetEnabled(false);
		pBtnOk_->setVisible(true);
		pBtnOk_->SetEnabled(true);
	}
	else if (_type == Type::eYesNo)
	{
		pBtnNo_->setVisible(true);
		pBtnNo_->SetEnabled(true);
		pBtnYes_->setVisible(true);
		pBtnYes_->SetEnabled(true);
		pBtnOk_->setVisible(false);
		pBtnOk_->SetEnabled(false);
	}
	else if (_type == Type::eNone)
	{
		pBtnNo_->setVisible(false);
		pBtnNo_->SetEnabled(false);
		pBtnYes_->setVisible(false);
		pBtnYes_->SetEnabled(false);
		pBtnOk_->setVisible(false);
		pBtnOk_->SetEnabled(false);
	}
}

void UI_Popup::SetText(const std::string& _text)
{
	pLabelText_->setText(_text);
}

void UI_Popup::SetYesCallback(const PopupCallback& _fnYes)
{
	yesCallback_ = _fnYes;
}

void UI_Popup::SetNoCallback(const PopupCallback& _fnNo)
{
	noCallback_ = _fnNo;
}

void UI_Popup::SetOkCallback(const PopupCallback& _fnOk)
{
	okCallback_ = _fnOk;
}

void UI_Popup::SetCloseWithEsc(bool _closeWithEsc)
{
	if (_closeWithEsc)
	{
		attributeFlag_.Add(eCloseWithEsc);
	}
	else
	{
		attributeFlag_.Unset(eCloseWithEsc);
	}
}

void UI_Popup::SetTimeoutCallback(const PopupCallback& _fnTimeout)
{
	timeoutCallback_ = _fnTimeout;
}

void UI_Popup::SetTimeout(float _timeout)
{
	if (_timeout >= 0.0f)
	{
		timeout_ = _timeout;
		attributeFlag_.Add(eTimeout);
	}
	else
	{
		attributeFlag_.Unset(eTimeout);
	}
}

void UI_Popup::Close()
{
	sg::Contents.PopupManager->Close(this);
}

void UI_Popup::Adjust()
{
	const float padding = sg::Contents.PopupManager->GetPadding();
	const Size& buttonArea = pGroupButtonHolder_->GetUISize();
	const float popupAreaWidth = Math::Max(sg::Contents.PopupManager->GetWidth(), buttonArea.width + padding * 2);
	const float popupTextAreaWidth = popupAreaWidth - padding * 2;

	pLabelText_->source()->setDimensions(popupTextAreaWidth, 0);

	const int lineCount = pLabelText_->getLineCount();
	const float fontSize = pLabelText_->getInitialFontSize();
	const float popupAreaHeight = padding * 3 + lineCount * fontSize + buttonArea.height;

	Size textArea = {
		popupTextAreaWidth,
		lineCount * fontSize
	};

	if (type_ == Type::eNone)
	{
		textArea.height += buttonArea.height + padding;
	}

	pGroupHolder_->SetUISize({ popupAreaWidth, popupAreaHeight });
	pGroupHolder_->SetRelativePosition(0, 0);

	pSpriteBackground_->SetUISize({ popupAreaWidth, popupAreaHeight });
	pSpriteBackground_->SetRelativePosition(0, 0);

	pLabelText_->SetUISize(textArea);
	pLabelText_->SetRelativePosition(padding, -padding, HAlignment::Left, VAlignment::Top);

	pGroupButtonHolder_->SetRelativePosition(0, padding, HAlignment::Center, VAlignment::Bottom);
}