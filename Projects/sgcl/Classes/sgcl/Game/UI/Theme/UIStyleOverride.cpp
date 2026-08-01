#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIStyleOverride.h"

static uint64_t HashCombine(uint64_t _seed, uint64_t _value)
{
    return _seed ^ (_value * 0x9E3779B97F4A7C15ull + 0xBF58476D1CE4E5B9ull);
}

uint64_t UIResolvedStyle::ComputeHash() const
{
    auto hashColor = [](uint64_t _seed, const UIColorF& _c) -> uint64_t {
        uint32_t r = (uint32_t)(_c.r * 255.0f);
        uint32_t g = (uint32_t)(_c.g * 255.0f);
        uint32_t b = (uint32_t)(_c.b * 255.0f);
        uint32_t a = (uint32_t)(_c.a * 255.0f);
        uint64_t v = ((uint64_t)r << 48) | ((uint64_t)g << 32) | ((uint64_t)b << 16) | (uint64_t)a;
        return HashCombine(_seed, v);
    };

    auto hashFloat = [](uint64_t _seed, float _f) -> uint64_t {
        union { float f; uint32_t u; } cvt;
        cvt.f = _f;
        return HashCombine(_seed, cvt.u);
    };

    uint64_t h = 0;
    h = hashColor(h, surfaceTop);
    h = hashColor(h, surfaceBottom);
    h = hashColor(h, surfaceInsetTop);
    h = hashColor(h, surfaceInsetBottom);
    h = hashColor(h, metalBorder);
    h = hashColor(h, metalHover);
    h = hashColor(h, metalHighlight);
    h = hashColor(h, metalBright);
    h = hashColor(h, metalShadow);
    h = hashColor(h, metalMuted);
    h = hashColor(h, glossTint);
    h = hashFloat(h, glossAlpha);
    h = hashFloat(h, glossCenterX);
    h = hashFloat(h, glossHeightRatio);
    h = hashFloat(h, glossBlurRadius);
    h = hashFloat(h, geometryRadius);
    h = hashFloat(h, geometryBorderWidth);
    h = hashFloat(h, geometryInnerRim);
    h = hashFloat(h, geometryDepth);
    h = hashFloat(h, geometryShadow);
    h = hashColor(h, semanticText);
    h = hashColor(h, semanticDanger);
    h = hashColor(h, semanticSuccess);
    h = hashColor(h, semanticWarning);
    return h;
}

UIColorF* UIStyleOverride::ResolveColorPtr(UIResolvedStyle& _style, UIStyleToken _token)
{
    switch (_token)
    {
    case UIStyleToken::SurfaceNormalTop:    return &_style.surfaceTop;
    case UIStyleToken::SurfaceNormalBottom: return &_style.surfaceBottom;
    case UIStyleToken::SurfaceInsetTop:     return &_style.surfaceInsetTop;
    case UIStyleToken::SurfaceInsetBottom:  return &_style.surfaceInsetBottom;
    case UIStyleToken::MetalBorder:         return &_style.metalBorder;
    case UIStyleToken::MetalHover:          return &_style.metalHover;
    case UIStyleToken::MetalHighlight:      return &_style.metalHighlight;
    case UIStyleToken::MetalBright:         return &_style.metalBright;
    case UIStyleToken::MetalShadow:         return &_style.metalShadow;
    case UIStyleToken::MetalMuted:          return &_style.metalMuted;
    case UIStyleToken::GlossTint:           return &_style.glossTint;
    case UIStyleToken::SemanticText:        return &_style.semanticText;
    case UIStyleToken::SemanticDanger:      return &_style.semanticDanger;
    case UIStyleToken::SemanticSuccess:     return &_style.semanticSuccess;
    case UIStyleToken::SemanticWarning:     return &_style.semanticWarning;
    default: return nullptr;
    }
}

float* UIStyleOverride::ResolveFloatPtr(UIResolvedStyle& _style, UIStyleToken _token)
{
    switch (_token)
    {
    case UIStyleToken::GlossNormalAlpha:        return &_style.glossAlpha;
    case UIStyleToken::GlossCenterX:            return &_style.glossCenterX;
    case UIStyleToken::GlossHeightRatio:        return &_style.glossHeightRatio;
    case UIStyleToken::GlossBlur:               return &_style.glossBlurRadius;
    case UIStyleToken::GeometryRadius:           return &_style.geometryRadius;
    case UIStyleToken::GeometryBorderWidth:      return &_style.geometryBorderWidth;
    case UIStyleToken::GeometryInnerRim:         return &_style.geometryInnerRim;
    case UIStyleToken::GeometryDepth:            return &_style.geometryDepth;
    case UIStyleToken::GeometryShadow:           return &_style.geometryShadow;
    default: return nullptr;
    }
}

void UIStyleOverride::SetColorValue(UIStyleToken _token, const UIColorF& _value)
{
    UIColorF* ptr = ResolveColorPtr(values, _token);
    if (ptr) *ptr = _value;
}

void UIStyleOverride::SetFloatValue(UIStyleToken _token, float _value)
{
    float* ptr = ResolveFloatPtr(values, _token);
    if (ptr) *ptr = _value;
}
