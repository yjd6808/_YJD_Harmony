#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIRuntimeTheme.h"

struct UIResolvedStyle
{
    UIColorF surfaceTop;
    UIColorF surfaceBottom;
    UIColorF surfaceInsetTop;
    UIColorF surfaceInsetBottom;

    UIColorF metalBorder;
    UIColorF metalHover;
    UIColorF metalHighlight;
    UIColorF metalBright;
    UIColorF metalShadow;
    UIColorF metalMuted;

    UIColorF glossTint;
    float glossAlpha = 0.0f;

    float geometryRadius = 8.0f;
    float geometryBorderWidth = 1.0f;
    float geometryInnerRim = 0.0f;
    float geometryDepth = 1.0f;
    float geometryShadow = 4.0f;

    UIColorF semanticText;
    UIColorF semanticDanger;
    UIColorF semanticSuccess;
    UIColorF semanticWarning;

    uint64_t ComputeHash() const;
};

enum class UIStyleToken : uint16_t
{
    SurfaceNormalTop,
    SurfaceNormalBottom,
    SurfaceHoverTop,
    SurfaceHoverBottom,
    SurfacePressedTop,
    SurfacePressedBottom,
    SurfaceDisabledTop,
    SurfaceDisabledBottom,
    SurfaceInsetTop,
    SurfaceInsetBottom,

    MetalBorder,
    MetalHover,
    MetalHighlight,
    MetalBright,
    MetalShadow,
    MetalMuted,

    GlossTint,
    GlossNormalAlpha,
    GlossHoverMultiplier,
    GlossDisabledMultiplier,
    GlossHeightRatio,
    GlossBlur,
    GlossCenterX,

    GeometryRadius,
    GeometryBorderWidth,
    GeometryInnerRim,
    GeometryDepth,
    GeometryShadow,

    SemanticText,
    SemanticDisabledText,
    SemanticDanger,
    SemanticSuccess,
    SemanticWarning,
    SemanticFocus,

    Count
};

struct UIStyleOverrideMask
{
    uint64_t lo = 0;
    uint64_t hi = 0;

    bool IsSet(size_t _bit) const
    {
        return _bit < 64 ? (lo & (1ULL << _bit)) != 0 : (hi & (1ULL << (_bit - 64))) != 0;
    }

    void Set(size_t _bit)
    {
        if (_bit < 64) lo |= (1ULL << _bit);
        else hi |= (1ULL << (_bit - 64));
    }

    void Reset(size_t _bit)
    {
        if (_bit < 64) lo &= ~(1ULL << _bit);
        else hi &= ~(1ULL << (_bit - 64));
    }

    void Clear()
    {
        lo = 0;
        hi = 0;
    }
};

struct UIStyleOverride
{
    UIResolvedStyle values;
    UIStyleOverrideMask mask;

    bool Has(UIStyleToken _token) const
    {
        return mask.IsSet((size_t)_token);
    }

    void Set(UIStyleToken _token, const UIColorF& _value)
    {
        SetColorValue(_token, _value);
        mask.Set((size_t)_token);
    }

    void Set(UIStyleToken _token, float _value)
    {
        SetFloatValue(_token, _value);
        mask.Set((size_t)_token);
    }

    void Clear(UIStyleToken _token)
    {
        mask.Reset((size_t)_token);
    }

    void ClearAll()
    {
        mask.Clear();
    }

private:
    void SetColorValue(UIStyleToken _token, const UIColorF& _value);
    void SetFloatValue(UIStyleToken _token, float _value);

    static UIColorF* ResolveColorPtr(UIResolvedStyle& _style, UIStyleToken _token);
    static float* ResolveFloatPtr(UIResolvedStyle& _style, UIStyleToken _token);
};
