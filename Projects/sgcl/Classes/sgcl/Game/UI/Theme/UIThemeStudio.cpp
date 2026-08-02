#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIThemeStudio.h"
#include "sgcl/Game/UI/Theme/UITextureBakeService.h"
#include "sgcl/Game/UI/Theme/UITextureCache.h"

USING_NS_CC;

static const float STUDIO_WIDTH = 640.0f;
static const float STUDIO_HEIGHT = 480.0f;
static const float SECTION_LABEL_HEIGHT = 24.0f;
static const float ROW_HEIGHT = 28.0f;
static const float CATALOG_HEIGHT = 160.0f;
static const float CONTROL_BAR_HEIGHT = 36.0f;
static const float STATUS_BAR_HEIGHT = 24.0f;
static const float SCROLL_OFFSET = 8.0f;

UIThemeStudio* UIThemeStudio::Create()
{
    auto* pStudio = dbg_new UIThemeStudio();
    pStudio->setContentSize({ STUDIO_WIDTH, STUDIO_HEIGHT });
    pStudio->BuildLayout();
    return pStudio;
}

void UIThemeStudio::BuildLayout()
{
    pBackground_ = LayerColor::create(Color4B(30, 30, 35, 230));
    pBackground_->setContentSize(getContentSize());
    pBackground_->setAnchorPoint({ 0, 0 });
    addChild(pBackground_);

    auto* pTitle = Label::createWithSystemFont("Theme Studio", "Arial", 16);
    if (pTitle)
    {
        pTitle->setAnchorPoint({ 0.5f, 1 });
        pTitle->setPosition(STUDIO_WIDTH * 0.5f, STUDIO_HEIGHT - 4);
        pTitle->setTextColor(Color4B::WHITE);
        addChild(pTitle);
    }

    BuildInspectorSections();
    BuildCatalog();
    BuildControlBar();
    BuildStatusBar();

    setVisible(false);
}

void UIThemeStudio::BuildInspectorSections()
{
    float scrollStartY = STUDIO_HEIGHT - CATALOG_HEIGHT - CONTROL_BAR_HEIGHT - SCROLL_OFFSET;
    float currentY = scrollStartY - SCROLL_OFFSET;

    int sectionCount = UIThemePropertyTable::GetSectionCount();
    sectionLabelCount_ = 0;

    for (int si = 0; si < sectionCount; ++si)
    {
        const auto* pSection = UIThemePropertyTable::GetSectionByIndex(si);
        if (!pSection) continue;

        if (sectionLabelCount_ < 8)
        {
            auto* pSectionLabel = Label::createWithSystemFont(pSection->name, "Arial", 12);
            if (pSectionLabel)
            {
                pSectionLabel->setAnchorPoint({ 0, 0 });
                pSectionLabel->setPosition(4, currentY);
                pSectionLabel->setTextColor(Color4B(180, 180, 200, 255));
                addChild(pSectionLabel);
                pSectionLabels_[sectionLabelCount_++] = pSectionLabel;
            }
            currentY -= SECTION_LABEL_HEIGHT;
        }

        for (int di = 0; di < pSection->count; ++di)
        {
            const auto* pDesc = UIThemePropertyTable::GetDescriptorByIndex(pSection->startIndex + di);
            if (!pDesc) continue;

            UIThemeInspectorRow* pRow = UIThemeInspectorRow::Create(*pDesc, UIColorF(), 0.0f);
            pRow->setAnchorPoint({ 0, 0 });
            pRow->setPosition(8, currentY);
            addChild(pRow);
            inspectorRows_.PushBack(pRow);

            pRow->SetOnValueChanged([this](UIStyleToken _token, float _value) { OnSliderChanged(_token, _value); });
            pRow->SetOnColorChanged([this](UIStyleToken _token, const UIColorF& _value) { OnColorChanged(_token, _value); });
            pRow->SetOnResetClicked([this](UIStyleToken _token) { OnResetToken(_token); });

            currentY -= ROW_HEIGHT;
        }
        currentY -= 4;
    }
}

void UIThemeStudio::BuildCatalog()
{
    pCatalog_ = UIThemePreviewCatalog::Create(STUDIO_WIDTH - 16, CATALOG_HEIGHT);
    if (pCatalog_)
    {
        pCatalog_->setAnchorPoint({ 0, 0 });
        pCatalog_->setPosition(8, CONTROL_BAR_HEIGHT + STATUS_BAR_HEIGHT);
        addChild(pCatalog_);
    }
}

void UIThemeStudio::BuildControlBar()
{
    const float btnY = STATUS_BAR_HEIGHT + 4;
    const float btnW = 80.0f;
    const float btnH = CONTROL_BAR_HEIGHT - 8;

    auto makeButton = [&](const char* _label, float _x)
    {
        auto* pLabel = Label::createWithSystemFont(_label, "Arial", 12);
        if (pLabel)
        {
            pLabel->setAnchorPoint({ 0, 0 });
            pLabel->setPosition(_x, btnY);
            pLabel->setTextColor(Color4B::WHITE);
            pLabel->setContentSize({ btnW, btnH });
            addChild(pLabel);
        }
    };

    makeButton("Undo", 8);
    makeButton("Redo", 92);
    makeButton("Reset All", 240);
    makeButton("Apply", STUDIO_WIDTH - btnW * 2 - 12);
    makeButton("Cancel", STUDIO_WIDTH - btnW - 8);
}

void UIThemeStudio::BuildStatusBar()
{
    pBuildStatusLabel_ = Label::createWithSystemFont("Idle", "Arial", 11);
    if (pBuildStatusLabel_)
    {
        pBuildStatusLabel_->setAnchorPoint({ 0, 0 });
        pBuildStatusLabel_->setPosition(8, 4);
        pBuildStatusLabel_->setTextColor(Color4B(150, 150, 160, 255));
        addChild(pBuildStatusLabel_);
    }

    pCacheStatsLabel_ = Label::createWithSystemFont("", "Arial", 11);
    if (pCacheStatsLabel_)
    {
        pCacheStatsLabel_->setAnchorPoint({ 1, 0 });
        pCacheStatsLabel_->setPosition(STUDIO_WIDTH - 8, 4);
        pCacheStatsLabel_->setTextColor(Color4B(150, 150, 160, 255));
        addChild(pCacheStatsLabel_);
    }
}

void UIThemeStudio::Show()
{
    isVisible_ = true;
    setVisible(true);
    RefreshAll();
}

void UIThemeStudio::Hide()
{
    isVisible_ = false;
    setVisible(false);
}

void UIThemeStudio::Toggle()
{
    if (isVisible_)
    {
        Hide();
    }
    else
    {
        Show();
    }
}

void UIThemeStudio::BeginEditIfNeeded()
{
    if (!isEditing_)
    {
        userOverrides_.ClearAll();
        isEditing_ = true;
    }
}

void UIThemeStudio::RefreshAll()
{
    UIThemeManager* pThemeMgr = UIThemeManager::Get();
    if (!pThemeMgr) return;

    UIResolvedStyle baseStyle = pThemeMgr->ResolveWithEditOverrides(
        UIElementType::Button, UIVisualState::Normal);

    int rowIdx = 0;
    for (int si = 0; si < UIThemePropertyTable::GetSectionCount(); ++si)
    {
        const auto* pSection = UIThemePropertyTable::GetSectionByIndex(si);
        if (!pSection) continue;

        for (int di = 0; di < pSection->count; ++di)
        {
            const auto* pDesc = UIThemePropertyTable::GetDescriptorByIndex(pSection->startIndex + di);
            if (!pDesc) continue;
            if (rowIdx >= inspectorRows_.Size()) continue;

            UIThemeInspectorRow* pRow = inspectorRows_[rowIdx++];
            bool isOverridden = userOverrides_.Has(pDesc->token);

            switch (pDesc->editor)
            {
            case UIPropertyEditorType::Color:
                {
                    UIColorF* pColor = UIStyleOverride::ResolveColorPtr(baseStyle, pDesc->token);
                    if (pColor)
                    {
                        pRow->SetColorValue(*pColor);
                    }
                }
                break;

            case UIPropertyEditorType::Slider:
            case UIPropertyEditorType::Dropdown:
            case UIPropertyEditorType::Toggle:
                {
                    float* pFloat = UIStyleOverride::ResolveFloatPtr(baseStyle, pDesc->token);
                    if (pFloat)
                    {
                        pRow->SetFloatValue(*pFloat);
                    }
                }
                break;
            }

            pRow->SetOverridden(isOverridden);
        }
    }

    UpdateCacheStats();
    if (pCatalog_)
    {
        pCatalog_->RefreshAll();
    }
}

void UIThemeStudio::OnSliderChanged(UIStyleToken _token, float _value)
{
    BeginEditIfNeeded();
    userOverrides_.Set(_token, _value);
    RequestPreview();
}

void UIThemeStudio::OnColorChanged(UIStyleToken _token, const UIColorF& _value)
{
    BeginEditIfNeeded();
    userOverrides_.Set(_token, _value);
    RequestPreview();
}

void UIThemeStudio::OnResetToken(UIStyleToken _token)
{
    if (!userOverrides_.Has(_token)) return;
    userOverrides_.Clear(_token);
    RefreshAll();
    RequestPreview();
}

void UIThemeStudio::OnApply()
{
    UIThemeManager* pThemeMgr = UIThemeManager::Get();
    if (!pThemeMgr) return;

    UpdateBuildStatus("Applying...");

    UIRuntimeTheme base = pThemeMgr->GetActiveTheme();
    pThemeMgr->CommitDraft(base, userOverrides_);

    UIThemeBakeRequest request;
    request.generation = pThemeMgr->GetRevision().sourceRevision + 1;
    request.theme = pThemeMgr->GetActiveTheme();
    request.preview = false;
    request.persistCache = true;
    pThemeMgr->RequestBake(jc::Move(request));

    isEditing_ = false;
    editHistory_.Clear();
    UpdateBuildStatus("Applied.");
    Hide();
}

void UIThemeStudio::OnCancel()
{
    UIThemeManager* pThemeMgr = UIThemeManager::Get();
    if (pThemeMgr)
    {
        pThemeMgr->CancelPreview();
    }

    isEditing_ = false;
    userOverrides_.ClearAll();
    editHistory_.Clear();
    Hide();
}

void UIThemeStudio::OnResetTheme()
{
    BeginEditIfNeeded();
    userOverrides_.ClearAll();
    RefreshAll();
    RequestPreview();
}

void UIThemeStudio::OnUndo()
{
    if (!editHistory_.CanUndo()) return;
    const UIThemeSnapshot& snap = editHistory_.Undo();
    userOverrides_.values = snap.values;
    userOverrides_.mask = snap.mask;
    RefreshAll();
    RequestPreview();
}

void UIThemeStudio::OnRedo()
{
    if (!editHistory_.CanRedo()) return;
    const UIThemeSnapshot& snap = editHistory_.Redo();
    userOverrides_.values = snap.values;
    userOverrides_.mask = snap.mask;
    RefreshAll();
    RequestPreview();
}

void UIThemeStudio::Update(float _dt)
{
    if (!isVisible_) return;
    UpdateCacheStats();
}

void UIThemeStudio::RequestPreview()
{
    UIThemeManager* pThemeMgr = UIThemeManager::Get();
    if (!pThemeMgr) return;

    UpdateBuildStatus("Baking preview...");

    UIThemeBakeRequest request;
    request.generation = pThemeMgr->GetRevision().sourceRevision;
    request.theme = pThemeMgr->GetActiveTheme();
    request.preview = true;
    request.persistCache = false;
    pThemeMgr->RequestBake(jc::Move(request));
}

void UIThemeStudio::UpdateBuildStatus(const char* _status)
{
    if (pBuildStatusLabel_)
    {
        pBuildStatusLabel_->setString(_status);
    }
}

void UIThemeStudio::UpdateCacheStats()
{
    if (!pCacheStatsLabel_) return;

    UIThemeManager* pThemeMgr = UIThemeManager::Get();
    if (!pThemeMgr) return;

    size_t memBytes = pThemeMgr->GetCache().GetCurrentMemoryBytes();
    char buf[64];
    if (memBytes > 1024 * 1024)
    {
        snprintf(buf, sizeof(buf), "Cache: %.1f MB", memBytes / (1024.0f * 1024.0f));
    }
    else if (memBytes > 1024)
    {
        snprintf(buf, sizeof(buf), "Cache: %.1f KB", memBytes / 1024.0f);
    }
    else
    {
        snprintf(buf, sizeof(buf), "Cache: %zu B", memBytes);
    }
    pCacheStatsLabel_->setString(buf);
}
