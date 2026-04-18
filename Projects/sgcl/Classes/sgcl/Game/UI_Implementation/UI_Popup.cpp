/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:10:03 AM
 * =====================
 *
 */


#include "GameCoreHeader.h"
#include "Game/UI_Implementation/UI_Popup.h"

#include "sgcl/Define/Define_UI.h"

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

void UI_Popup::OnInit()
{
	// #define UI_POPUP_GROUP_HOLDER
	// #define UI_POPUP_HOLDER_SPRITE_BACKGROUND
	// #define UI_POPUP_HOLDER_LABEL_TEXT
	// #define UI_POPUP_HOLDER_GROUP_BTN_HOLDER
	// #define UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_YES
	// #define UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_NO
	// #define UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_OK

	pGroupHolder_ = FindGroup(UI_POPUP_GROUP_HOLDER);
	pSpriteBackground_ = FindSprite(UI_POPUP_HOLDER_SPRITE_BACKGROUND);
	pGroupButtonHolder_ = FindGroup(UI_POPUP_HOLDER_GROUP_BTN_HOLDER);
	pBtnYes_ = FindButton(UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_YES);
	pBtnNo_ = FindButton(UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_NO);
	pBtnOk_ = FindButton(UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_OK);
	pLabelText_ = FindLabel(UI_POPUP_HOLDER_LABEL_TEXT);

	pLabelText_->setEnableFontAutoScaling(false);
	pGroupButtonHolder_->SetResizable(false);
	pSpriteBackground_->SetDraggable(true);
	pSpriteBackground_->SetDragLinkElement(pGroupHolder_);
}

void UI_Popup::OnLoaded()
{
	const Size backgroundSize = pSpriteBackground_->GetUISize();
	const float borderThickness = 3.0f; // 팝업 배경 변경시 수정해줄 것

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
	switch (_pElement->GetCode())
	{
	case UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_YES:
		if (yesCallback_)
		{
			yesCallback_();
		}
		Close();
		break;
	case UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_NO:
		if (noCallback_)
		{
			noCallback_();
		}
		Close();
		break;
	case UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_OK:
		if (okCallback_)
		{
			okCallback_();
		}
		Close();
		break;
	default:
		break;
	}
}

bool UI_Popup::OnMouseMoveInternalDetail(cc::EventMouse* _pMouseEvent)
{
	return false;
}

// 팝업이 활성화된 동안 해당 팝업보다 우선순위(_localZOrder)가 낮은 대상에게는 이벤트가 전파되지 않도록 하기 위해
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
	// 내가 정한 팝업박스 규칙
	// 1. 팝업창의 너비는 최소한 팝업 버튼을 포함할 수 있을 만큼 넓어야한다.
	// 2. 팝업창의 너비는 패딩 길이를 포함한 너비이다.
	// 3. 팝업의 하단부의 버튼영역과 상단부의 텍스트 영역으로 구분한다.
	// 4. 팝업 상단부와 하단부 사이에 패딩 너비를 포함하여 여백의 미를 준다.
	const float padding = sg::Contents.PopupManager->GetPadding();
	const Size& buttonArea = pGroupButtonHolder_->GetUISize();
	const float popupAreaWidth = Math::Max(sg::Contents.PopupManager->GetWidth(), buttonArea.width + padding * 2);
	const float popupTextAreaWidth = popupAreaWidth - padding * 2;

	// 라벨의 Dimension이 UI 설정파일상의 너비, 높이 정보로 처음 세팅되기 때문에
	// setText()후 getLineCount()를 수행할때 이 기본 정보를 바탕으로 라인수를 얻게된다.
	// 팝업매니저에서 설정한 팝업의 기본너비에 맞게 라벨의 너비정보도 수정해줘야
	// 텍스트 수정 후 Cocos2d-x 엔진이 너비에 맞춰서 올바르게 계산된 라인수를 얻어낼 수 있게된다.

	// 1. 먼저 텍스트영역 너비를 결정해줘서 라인카운트를 올바르게 계산할 수 있도록 만듬
	pLabelText_->source()->setDimensions(popupTextAreaWidth, 0);

	// 2. 위 작업 덕분에 게임엔진의 라벨이 해당 너비를 기준으로 올바르게 라인수를 계산할 수 있게된다.
	const int lineCount = pLabelText_->getLineCount();
	const float fontSize = pLabelText_->getInitialFontSize();
	const float popupAreaHeight = padding * 3 + lineCount * fontSize + buttonArea.height;

	Size textArea = {
		popupTextAreaWidth,
		lineCount * fontSize
	};

	// 논타입인 경우 버튼이 없으므로, 버튼 영역까지 높이를 확장해준다.
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
