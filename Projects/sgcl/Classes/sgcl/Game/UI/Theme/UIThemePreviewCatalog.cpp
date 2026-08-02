/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIThemePreviewCatalog.h"

USING_NS_CC;

using namespace sgui;

static const cc::size PREVIEW_BUTTON_SIZE = { 100, 32 };
static const cc::size PREVIEW_CHECK_SIZE = { 32, 32 };
static const cc::size PREVIEW_TOGGLE_SIZE = { 54, 28 };
static const cc::size PREVIEW_PROGRESS_SIZE = { 120, 16 };
static const float CATALOG_PADDING = 8.0f;
static const float ROW_SPACING = 36.0f;

UIThemePreviewCatalog* UIThemePreviewCatalog::Create(float _width, float _height)
{
	auto* pCatalog = dbg_new UIThemePreviewCatalog();
	pCatalog->autorelease();
	pCatalog->setContentSize({ _width, _height });
	pCatalog->CreateButtons();
	pCatalog->CreateCheckBoxes();
	pCatalog->CreateToggles();
	pCatalog->CreateProgressBars();
	return pCatalog;
}

void UIThemePreviewCatalog::SetPreviewSize(float _width, float _height)
{
	setContentSize({ _width, _height });
}

// 카탈로그는 Window 호스트 없이 동작하므로 각 요소를 직접 Measure/Arrange 한다.
// (_x, _y)는 y-down 로컬 좌표다.
void UIThemePreviewCatalog::PlaceElement(UIElement* _pElement, float _x, float _y, float _w, float _h)
{
	addChild(_pElement);
	_pElement->SetWidth(_w);
	_pElement->SetHeight(_h);
	_pElement->Measure(cc::size(_w, _h));
	_pElement->Arrange(sgui::Rect(_x, _y, _w, _h));
}

void UIThemePreviewCatalog::CreateButtons()
{
	const float y = CATALOG_PADDING + ROW_SPACING * 0.0f;
	const float stepX = PREVIEW_BUTTON_SIZE.width + CATALOG_PADDING;

	pBtnNormal_ = Button::Create("Normal");
	PlaceElement(pBtnNormal_, CATALOG_PADDING, y, PREVIEW_BUTTON_SIZE.width, PREVIEW_BUTTON_SIZE.height);

	pBtnHover_ = Button::Create("Hover");
	PlaceElement(pBtnHover_, CATALOG_PADDING + stepX, y, PREVIEW_BUTTON_SIZE.width, PREVIEW_BUTTON_SIZE.height);

	pBtnPressed_ = Button::Create("Pressed");
	PlaceElement(pBtnPressed_, CATALOG_PADDING + stepX * 2.0f, y, PREVIEW_BUTTON_SIZE.width, PREVIEW_BUTTON_SIZE.height);

	pBtnDisabled_ = Button::Create("Disabled");
	PlaceElement(pBtnDisabled_, CATALOG_PADDING + stepX * 3.0f, y, PREVIEW_BUTTON_SIZE.width, PREVIEW_BUTTON_SIZE.height);
	pBtnDisabled_->SetIsEnabled(false);
}

void UIThemePreviewCatalog::CreateCheckBoxes()
{
	const float y = CATALOG_PADDING + ROW_SPACING * 1.0f;
	const float stepX = PREVIEW_CHECK_SIZE.width + CATALOG_PADDING;

	pCheckOff_ = CheckBox::Create(nullptr);
	PlaceElement(pCheckOff_, CATALOG_PADDING, y, PREVIEW_CHECK_SIZE.width, PREVIEW_CHECK_SIZE.height);
	pCheckOff_->SetIsChecked(false);

	pCheckOn_ = CheckBox::Create(nullptr);
	PlaceElement(pCheckOn_, CATALOG_PADDING + stepX, y, PREVIEW_CHECK_SIZE.width, PREVIEW_CHECK_SIZE.height);
	pCheckOn_->SetIsChecked(true);

	pCheckDisabled_ = CheckBox::Create(nullptr);
	PlaceElement(pCheckDisabled_, CATALOG_PADDING + stepX * 2.0f, y, PREVIEW_CHECK_SIZE.width, PREVIEW_CHECK_SIZE.height);
	pCheckDisabled_->SetIsChecked(true);
	pCheckDisabled_->SetIsEnabled(false);
}

void UIThemePreviewCatalog::CreateToggles()
{
	const float y = CATALOG_PADDING + ROW_SPACING * 2.0f;
	const float stepX = PREVIEW_TOGGLE_SIZE.width + CATALOG_PADDING;

	pToggleOff_ = ToggleButton::Create(nullptr);
	pToggleOff_->BuildSwitchVisuals();
	PlaceElement(pToggleOff_, CATALOG_PADDING, y, PREVIEW_TOGGLE_SIZE.width, PREVIEW_TOGGLE_SIZE.height);

	pToggleOn_ = ToggleButton::Create(nullptr);
	pToggleOn_->BuildSwitchVisuals();
	PlaceElement(pToggleOn_, CATALOG_PADDING + stepX, y, PREVIEW_TOGGLE_SIZE.width, PREVIEW_TOGGLE_SIZE.height);
	pToggleOn_->SetIsChecked(true);

	pToggleDisabled_ = ToggleButton::Create(nullptr);
	pToggleDisabled_->BuildSwitchVisuals();
	PlaceElement(pToggleDisabled_, CATALOG_PADDING + stepX * 2.0f, y, PREVIEW_TOGGLE_SIZE.width, PREVIEW_TOGGLE_SIZE.height);
	pToggleDisabled_->SetIsEnabled(false);
}

void UIThemePreviewCatalog::CreateProgressBars()
{
	const float y = CATALOG_PADDING + ROW_SPACING * 3.0f;
	const float stepX = PREVIEW_PROGRESS_SIZE.width + CATALOG_PADDING;

	pProgressNeutral_ = ProgressBar::Create();
	PlaceElement(pProgressNeutral_, CATALOG_PADDING, y, PREVIEW_PROGRESS_SIZE.width, PREVIEW_PROGRESS_SIZE.height);
	pProgressNeutral_->SetPercent(50.0f);

	pProgressSuccess_ = ProgressBar::Create();
	PlaceElement(pProgressSuccess_, CATALOG_PADDING + stepX, y, PREVIEW_PROGRESS_SIZE.width, PREVIEW_PROGRESS_SIZE.height);
	pProgressSuccess_->SetPercent(80.0f);

	pProgressWarning_ = ProgressBar::Create();
	PlaceElement(pProgressWarning_, CATALOG_PADDING + stepX * 2.0f, y, PREVIEW_PROGRESS_SIZE.width, PREVIEW_PROGRESS_SIZE.height);
	pProgressWarning_->SetPercent(30.0f);
}

void UIThemePreviewCatalog::RefreshAll()
{
	if (pBtnNormal_)
	{
		pBtnNormal_->RefreshThemeVisuals();
	}
	if (pBtnHover_)
	{
		pBtnHover_->RefreshThemeVisuals();
	}
	if (pBtnPressed_)
	{
		pBtnPressed_->RefreshThemeVisuals();
	}
	if (pBtnDisabled_)
	{
		pBtnDisabled_->RefreshThemeVisuals();
	}
	if (pCheckOff_)
	{
		pCheckOff_->SetIsChecked(false);
	}
	if (pCheckOn_)
	{
		pCheckOn_->SetIsChecked(true);
	}
	if (pToggleOff_)
	{
		pToggleOff_->SetIsChecked(false);
	}
	if (pToggleOn_)
	{
		pToggleOn_->SetIsChecked(true);
	}
	if (pProgressNeutral_)
	{
		pProgressNeutral_->SetPercent(50.0f);
	}
	if (pProgressSuccess_)
	{
		pProgressSuccess_->SetPercent(80.0f);
	}
	if (pProgressWarning_)
	{
		pProgressWarning_->SetPercent(30.0f);
	}
}
