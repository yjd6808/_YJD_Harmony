#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIThemePropertyDescriptor.h"
#include "sgcl/Game/UI/Theme/UIThemeEditHistory.h"
#include "sgcl/Game/UI/Theme/UIStyleOverride.h"

class UIThemeInspectorRow : public cc::Node
{
public:
    using ValueChangedCallback = jc::Action<UIStyleToken, float>;
    using ColorChangedCallback = jc::Action<UIStyleToken, const UIColorF&>;
    using ResetClickedCallback = jc::Action<UIStyleToken>;

    static UIThemeInspectorRow* Create(
        const UIThemePropertyDescriptor& _desc,
        const UIColorF& _initialColor,
        float _initialValue);

    void SetColorValue(const UIColorF& _color);
    void SetFloatValue(float _value);
    void SetOverridden(bool _overridden);
    void SetEnabled(bool _enabled);

    UIStyleToken GetToken() const { return descriptor_.token; }
    bool IsOverridden() const { return isOverridden_; }

    void SetOnValueChanged(ValueChangedCallback _fn) { onValueChanged_ = _fn; }
    void SetOnColorChanged(ColorChangedCallback _fn) { onColorChanged_ = _fn; }
    void SetOnResetClicked(ResetClickedCallback _fn) { onResetClicked_ = _fn; }

    void SetHighlightColor(const UIColorF& _color);

private:
    UIThemePropertyDescriptor descriptor_;
    bool isOverridden_ = false;

    cc::Label* pLabel_;
    cc::Label* pValueLabel_;
    cc::Node* pEditor_;

    ValueChangedCallback onValueChanged_;
    ColorChangedCallback onColorChanged_;
    ResetClickedCallback onResetClicked_;
};
