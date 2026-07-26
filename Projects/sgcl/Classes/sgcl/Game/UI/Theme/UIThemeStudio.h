#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIThemePropertyDescriptor.h"
#include "sgcl/Game/UI/Theme/UIThemeEditHistory.h"
#include "sgcl/Game/UI/Theme/UIThemeInspectorRow.h"
#include "sgcl/Game/UI/Theme/UIThemePreviewCatalog.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"
#include "sgcl/Game/UI/Theme/UIStyleOverride.h"

class UIThemeStudio : public cc::Node
{
public:
    static UIThemeStudio* Create();

    void Show();
    void Hide();
    void Toggle();
    bool IsVisible() const { return isVisible_; }

    void RefreshAll();
    void Update(float _dt);

private:
    bool isVisible_ = false;
    bool isEditing_ = false;
    UIThemeEditHistory editHistory_;
    UIThemePreviewCatalog* pCatalog_ = nullptr;
    cc::LayerColor* pBackground_ = nullptr;

    UIStyleOverride userOverrides_;
    jc::Vector<UIThemeInspectorRow*> inspectorRows_;
    cc::Label* pBuildStatusLabel_ = nullptr;
    cc::Label* pCacheStatsLabel_ = nullptr;
    cc::Label* pSectionLabels_[8];
    int sectionLabelCount_ = 0;

    void BuildLayout();
    void BuildInspectorSections();
    void BuildControlBar();
    void BuildStatusBar();
    void BuildCatalog();

    void OnSliderChanged(UIStyleToken _token, float _value);
    void OnColorChanged(UIStyleToken _token, const UIColorF& _value);
    void OnResetToken(UIStyleToken _token);

    void OnApply();
    void OnCancel();
    void OnResetTheme();
    void OnUndo();
    void OnRedo();

    void RequestPreview();
    void UpdateBuildStatus(const char* _status);
    void UpdateCacheStats();
    void BeginEditIfNeeded();
};
