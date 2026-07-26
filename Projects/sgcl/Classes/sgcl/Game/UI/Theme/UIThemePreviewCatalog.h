#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIThemeBinding.h"
#include "sgcl/Game/UI/UIButton.h"
#include "sgcl/Game/UI/UICheckBox.h"
#include "sgcl/Game/UI/UIToggleButton.h"
#include "sgcl/Game/UI/UIProgressBar.h"

class UIThemePreviewCatalog : public cc::Node
{
public:
    static UIThemePreviewCatalog* Create(float _width, float _height);

    void SetPreviewSize(float _width, float _height);
    void RefreshAll();

private:
    UIButton* pBtnNormal_;
    UIButton* pBtnHover_;
    UIButton* pBtnPressed_;
    UIButton* pBtnDisabled_;

    UICheckBox* pCheckOff_;
    UICheckBox* pCheckOn_;
    UICheckBox* pCheckDisabled_;

    UIToggleButton* pToggleOff_;
    UIToggleButton* pToggleOn_;
    UIToggleButton* pToggleDisabled_;

    UIProgressBar* pProgressNeutral_;
    UIProgressBar* pProgressSuccess_;
    UIProgressBar* pProgressWarning_;

    void CreateButtons();
    void CreateCheckBoxes();
    void CreateToggles();
    void CreateProgressBars();
};
