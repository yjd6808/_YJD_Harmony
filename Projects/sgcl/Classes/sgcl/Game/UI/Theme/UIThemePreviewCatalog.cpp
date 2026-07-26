#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIThemePreviewCatalog.h"

USING_NS_CC;

static const cc::size PREVIEW_BUTTON_SIZE = { 100, 32 };
static const cc::size PREVIEW_CHECK_SIZE = { 32, 32 };
static const cc::size PREVIEW_TOGGLE_SIZE = { 54, 28 };
static const cc::size PREVIEW_PROGRESS_SIZE = { 120, 16 };
static const float CATALOG_PADDING = 8.0f;
static const float ROW_SPACING = 36.0f;

UIThemePreviewCatalog* UIThemePreviewCatalog::Create(float _width, float _height)
{
    auto* pCatalog = dbg_new UIThemePreviewCatalog();
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

void UIThemePreviewCatalog::CreateButtons()
{
    const float startY = getContentSize().height - CATALOG_PADDING - ROW_SPACING;

    pBtnNormal_ = UIButton::Create(nullptr, nullptr);
    pBtnNormal_->setContentSize(PREVIEW_BUTTON_SIZE);
    pBtnNormal_->setAnchorPoint({ 0, 1 });
    pBtnNormal_->setPosition(CATALOG_PADDING, startY);
    addChild(pBtnNormal_);

    pBtnHover_ = UIButton::Create(nullptr, nullptr);
    pBtnHover_->setContentSize(PREVIEW_BUTTON_SIZE);
    pBtnHover_->setAnchorPoint({ 0, 1 });
    pBtnHover_->setPosition(CATALOG_PADDING + PREVIEW_BUTTON_SIZE.width + CATALOG_PADDING, startY);
    addChild(pBtnHover_);

    pBtnPressed_ = UIButton::Create(nullptr, nullptr);
    pBtnPressed_->setContentSize(PREVIEW_BUTTON_SIZE);
    pBtnPressed_->setAnchorPoint({ 0, 1 });
    pBtnPressed_->setPosition(CATALOG_PADDING + (PREVIEW_BUTTON_SIZE.width + CATALOG_PADDING) * 2, startY);
    addChild(pBtnPressed_);

    pBtnDisabled_ = UIButton::Create(nullptr, nullptr);
    pBtnDisabled_->setContentSize(PREVIEW_BUTTON_SIZE);
    pBtnDisabled_->setAnchorPoint({ 0, 1 });
    pBtnDisabled_->setPosition(CATALOG_PADDING + (PREVIEW_BUTTON_SIZE.width + CATALOG_PADDING) * 3, startY);
    pBtnDisabled_->SetEnabled(false);
    addChild(pBtnDisabled_);
}

void UIThemePreviewCatalog::CreateCheckBoxes()
{
    const float startY = getContentSize().height - CATALOG_PADDING - ROW_SPACING * 2;

    pCheckOff_ = UICheckBox::Create(nullptr, nullptr);
    pCheckOff_->setContentSize(PREVIEW_CHECK_SIZE);
    pCheckOff_->setAnchorPoint({ 0, 1 });
    pCheckOff_->setPosition(CATALOG_PADDING, startY);
    pCheckOff_->SetCheck(false);
    addChild(pCheckOff_);

    pCheckOn_ = UICheckBox::Create(nullptr, nullptr);
    pCheckOn_->setContentSize(PREVIEW_CHECK_SIZE);
    pCheckOn_->setAnchorPoint({ 0, 1 });
    pCheckOn_->setPosition(CATALOG_PADDING + PREVIEW_CHECK_SIZE.width + CATALOG_PADDING, startY);
    pCheckOn_->SetCheck(true);
    addChild(pCheckOn_);

    pCheckDisabled_ = UICheckBox::Create(nullptr, nullptr);
    pCheckDisabled_->setContentSize(PREVIEW_CHECK_SIZE);
    pCheckDisabled_->setAnchorPoint({ 0, 1 });
    pCheckDisabled_->setPosition(CATALOG_PADDING + (PREVIEW_CHECK_SIZE.width + CATALOG_PADDING) * 2, startY);
    pCheckDisabled_->SetCheck(true);
    pCheckDisabled_->SetEnabled(false);
    addChild(pCheckDisabled_);
}

void UIThemePreviewCatalog::CreateToggles()
{
    const float startY = getContentSize().height - CATALOG_PADDING - ROW_SPACING * 3;

    pToggleOff_ = UIToggleButton::Create(nullptr, nullptr, nullptr, false);
    pToggleOff_->setContentSize(PREVIEW_TOGGLE_SIZE);
    pToggleOff_->setAnchorPoint({ 0, 1 });
    pToggleOff_->setPosition(CATALOG_PADDING, startY);
    addChild(pToggleOff_);

    pToggleOn_ = UIToggleButton::Create(nullptr, nullptr, nullptr, false);
    pToggleOn_->setContentSize(PREVIEW_TOGGLE_SIZE);
    pToggleOn_->setAnchorPoint({ 0, 1 });
    pToggleOn_->setPosition(CATALOG_PADDING + PREVIEW_TOGGLE_SIZE.width + CATALOG_PADDING, startY);
    pToggleOn_->SetToggleState(ToggleState::eToggled);
    addChild(pToggleOn_);

    pToggleDisabled_ = UIToggleButton::Create(nullptr, nullptr, nullptr, false);
    pToggleDisabled_->setContentSize(PREVIEW_TOGGLE_SIZE);
    pToggleDisabled_->setAnchorPoint({ 0, 1 });
    pToggleDisabled_->setPosition(CATALOG_PADDING + (PREVIEW_TOGGLE_SIZE.width + CATALOG_PADDING) * 2, startY);
    pToggleDisabled_->SetEnabled(false);
    addChild(pToggleDisabled_);
}

void UIThemePreviewCatalog::CreateProgressBars()
{
    const float startY = getContentSize().height - CATALOG_PADDING - ROW_SPACING * 4;

    pProgressNeutral_ = UIProgressBar::Create(nullptr, nullptr, nullptr, false);
    pProgressNeutral_->setContentSize(PREVIEW_PROGRESS_SIZE);
    pProgressNeutral_->setAnchorPoint({ 0, 1 });
    pProgressNeutral_->setPosition(CATALOG_PADDING, startY);
    pProgressNeutral_->SetPercent(50.0f);
    addChild(pProgressNeutral_);

    pProgressSuccess_ = UIProgressBar::Create(nullptr, nullptr, nullptr, false);
    pProgressSuccess_->setContentSize(PREVIEW_PROGRESS_SIZE);
    pProgressSuccess_->setAnchorPoint({ 0, 1 });
    pProgressSuccess_->setPosition(CATALOG_PADDING + PREVIEW_PROGRESS_SIZE.width + CATALOG_PADDING, startY);
    pProgressSuccess_->SetPercent(80.0f);
    addChild(pProgressSuccess_);

    pProgressWarning_ = UIProgressBar::Create(nullptr, nullptr, nullptr, false);
    pProgressWarning_->setContentSize(PREVIEW_PROGRESS_SIZE);
    pProgressWarning_->setAnchorPoint({ 0, 1 });
    pProgressWarning_->setPosition(CATALOG_PADDING + (PREVIEW_PROGRESS_SIZE.width + CATALOG_PADDING) * 2, startY);
    pProgressWarning_->SetPercent(30.0f);
    addChild(pProgressWarning_);
}

void UIThemePreviewCatalog::RefreshAll()
{
    if (pBtnNormal_) pBtnNormal_->RefreshThemeVisuals();
    if (pBtnHover_) pBtnHover_->RefreshThemeVisuals();
    if (pBtnPressed_) pBtnPressed_->RefreshThemeVisuals();
    if (pBtnDisabled_) pBtnDisabled_->RefreshThemeVisuals();
    if (pCheckOff_) pCheckOff_->SetCheck(false);
    if (pCheckOn_) pCheckOn_->SetCheck(true);
    if (pToggleOff_) pToggleOff_->SetToggleState(ToggleState::eNormal);
    if (pToggleOn_) pToggleOn_->SetToggleState(ToggleState::eToggled);
    if (pProgressNeutral_) pProgressNeutral_->SetPercent(50.0f);
    if (pProgressSuccess_) pProgressSuccess_->SetPercent(80.0f);
    if (pProgressWarning_) pProgressWarning_->SetPercent(30.0f);
}
