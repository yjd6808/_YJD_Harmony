#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIThemeMapper.h"
#include "sgcl/Game/UI/Theme/UIStyleResolver.h"

UIColorF UIThemeMapper::Mix(const UIColorF& _a, const UIColorF& _b, int _percent)
{
    float t = jc::Math::Clamp(_percent / 100.0f, 0.0f, 1.0f);
    return Mix(_a, _b, t);
}

UIColorF UIThemeMapper::Mix(const UIColorF& _a, const UIColorF& _b, float _t)
{
    return {
        _a.r + (_b.r - _a.r) * _t,
        _a.g + (_b.g - _a.g) * _t,
        _a.b + (_b.b - _a.b) * _t,
        _a.a + (_b.a - _a.a) * _t
    };
}

UIColorF UIThemeMapper::ReadColor(const Json::Value& /*_root*/, const char* /*_path*/, const UIColorF& _fallback)
{
    return _fallback;
}

UIRuntimeTheme UIThemeMapper::Map(const Json::Value& _source, UIColorScheme _scheme)
{
    UIRuntimeTheme out = UIRuntimeTheme::EngineDefaults();

    bool isDark = (_scheme == UIColorScheme::Dark);

    (void)_source;

    int base = isDark ? 30 : 240;
    int inkVal = isDark ? 220 : 30;
    UIColorF neutral = UIColorF::FromRGBA((uint8_t)base, (uint8_t)base, (uint8_t)base);
    UIColorF ink = UIColorF::FromRGBA((uint8_t)inkVal, (uint8_t)inkVal, (uint8_t)inkVal);
    UIColorF primary = UIColorF::FromRGBA(isDark ? 180 : 60, isDark ? 148 : 100, isDark ? 80 : 180);

    out.surface.normalTop = Mix(neutral, ink, isDark ? 18 : 3);
    out.surface.normalBottom = Mix(neutral, ink, isDark ? 38 : 10);
    out.surface.hoverTop = Mix(out.surface.normalTop, primary, 10);
    out.surface.hoverBottom = Mix(out.surface.normalBottom, primary, 7);
    out.metal.border = primary;
    out.metal.hover = Mix(primary, ink, isDark ? 18 : 8);
    out.semantic.text = ink;

    return out;
}

UIColorScheme UIThemeMapper::ResolveScheme(UIColorScheme _requested)
{
    if (_requested != UIColorScheme::System)
        return _requested;

    return UIColorScheme::Dark;
}
