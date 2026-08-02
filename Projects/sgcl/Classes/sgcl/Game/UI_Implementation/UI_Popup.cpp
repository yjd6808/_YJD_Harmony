/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "Game/UI_Implementation/UI_Popup.h"
#include "Game/Contents/PopupManager.h"

USING_NS_CC;
USING_NS_JC;

using namespace sgui;

UI_Popup* UI_Popup::Create()
{
	UI_Popup* pPopup = dbg_new UI_Popup;
	pPopup->autorelease();
	pPopup->OnInit(CDataMap<>());
	return pPopup;
}

void UI_Popup::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);

	SetModal(true);
	SetDragMoveEnabled(true);

	// 본문 + 버튼 영역
	StackPanel* pRootStack = StackPanel::Create(Orientation::Vertical);
	pRootStack->SetSpacing(16.0f);

	pTextBlock_ = TextBlock::Create();
	pTextBlock_->setName("text");
	pTextBlock_->SetTextWrapping(TextWrapping::Wrap);
	pRootStack->AddChild(pTextBlock_);

	pButtonPanel_ = StackPanel::Create(Orientation::Horizontal);
	pButtonPanel_->SetSpacing(8.0f);
	pButtonPanel_->SetHorizontalAlignment(HorizontalAlignment::Center);

	pBtnYes_ = Button::Create("확인");
	pBtnYes_->setName("yes");
	pBtnYes_->SetClickCallback([this](ButtonBase*)
	{
		const PopupCallback callback = yesCallback_;
		ClosePopup();

		if (callback)
		{
			callback();
		}
	});
	pButtonPanel_->AddChild(pBtnYes_);

	pBtnNo_ = Button::Create("취소");
	pBtnNo_->setName("no");
	pBtnNo_->SetClickCallback([this](ButtonBase*)
	{
		const PopupCallback callback = noCallback_;
		ClosePopup();

		if (callback)
		{
			callback();
		}
	});
	pButtonPanel_->AddChild(pBtnNo_);

	pBtnOk_ = Button::Create("확인");
	pBtnOk_->setName("ok");
	pBtnOk_->SetClickCallback([this](ButtonBase*)
	{
		const PopupCallback callback = okCallback_;
		ClosePopup();

		if (callback)
		{
			callback();
		}
	});
	pButtonPanel_->AddChild(pBtnOk_);

	pRootStack->AddChild(pButtonPanel_);

	pHolder_ = Border::Create();
	pHolder_->setName("holder");
	pHolder_->SetBackground(SolidColorBrush::Create(0.08f, 0.08f, 0.1f, 0.92f));
	pHolder_->SetBorderBrush(UIColorF{ 0.4f, 0.4f, 0.45f, 1.0f });
	pHolder_->SetBorderThickness(Thickness(1.0f));
	pHolder_->SetPadding(Thickness(20.0f));
	pHolder_->SetHorizontalAlignment(HorizontalAlignment::Center);
	pHolder_->SetVerticalAlignment(VerticalAlignment::Center);
	pHolder_->SetChild(pRootStack);
	AddChild(pHolder_);

	SetType(Type::eNone);
}

void UI_Popup::OnUpdate(float _dt)
{
	if (timeout_ <= 0.0f)
	{
		return;
	}

	elapsed_ += _dt;

	if (elapsed_ >= timeout_)
	{
		const PopupCallback callback = timeoutCallback_;
		ClosePopup();

		if (callback)
		{
			callback();
		}
	}
}

bool UI_Popup::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode)
{
	if (_keyCode == EventKeyboard::KeyCode::KEY_ESCAPE && IsCloseWithEscape())
	{
		ClosePopup();
		return true;
	}

	return false;
}

void UI_Popup::SetType(Type _type)
{
	type_ = _type;

	pBtnYes_->SetVisibility(type_ == Type::eYesNo ? Visibility::Visible : Visibility::Collapsed);
	pBtnNo_->SetVisibility(type_ == Type::eYesNo ? Visibility::Visible : Visibility::Collapsed);
	pBtnOk_->SetVisibility(type_ == Type::eOk ? Visibility::Visible : Visibility::Collapsed);
	pButtonPanel_->SetVisibility(type_ == Type::eNone ? Visibility::Collapsed : Visibility::Visible);
}

void UI_Popup::SetTextHAlign(HAlignment_t _hAlign)
{
	if (_hAlign == HAlignment::Center)
	{
		pTextBlock_->SetTextAlignment(sgui::TextAlignment::Center);
	}
	else if (_hAlign == HAlignment::Right)
	{
		pTextBlock_->SetTextAlignment(sgui::TextAlignment::Right);
	}
	else
	{
		pTextBlock_->SetTextAlignment(sgui::TextAlignment::Left);
	}
}

void UI_Popup::SetTextVAlign(VAlignment_t _vAlign)
{
	if (_vAlign == VAlignment::Center)
	{
		pTextBlock_->SetVerticalAlignment(sgui::VerticalAlignment::Center);
	}
	else if (_vAlign == VAlignment::Bottom)
	{
		pTextBlock_->SetVerticalAlignment(sgui::VerticalAlignment::Bottom);
	}
	else
	{
		pTextBlock_->SetVerticalAlignment(sgui::VerticalAlignment::Top);
	}
}

void UI_Popup::SetText(const std::string& _text)
{
	pTextBlock_->SetText(_text.c_str());
}

void UI_Popup::SetTimeout(float _timeout)
{
	timeout_ = _timeout;
	elapsed_ = 0.0f;
}

void UI_Popup::ClosePopup()
{
	PopupManager::Get()->Close(this);
}

void UI_Popup::Adjust()
{
	const float width = PopupManager::Get()->GetWidth();
	const float padding = PopupManager::Get()->GetPadding();

	pTextBlock_->SetWidth(width - padding * 2.0f);
	pHolder_->SetPadding(Thickness(padding * 2.0f));
	CenterOnHost();
	MarkLayoutDirty();
}

// 팝업은 PopupManager가 직접 생성하므로 REGISTER_WINDOW를 사용하지 않는다.
