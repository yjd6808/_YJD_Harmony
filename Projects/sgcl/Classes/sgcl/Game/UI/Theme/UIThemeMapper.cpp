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

const Json::Value* UIThemeMapper::FindNode(const Json::Value& _root, const char* _path)
{
    if (!_path || !_path[0])
        return nullptr;

    const Json::Value* pNode = &_root;
    char key[64];

    const char* p = _path;
    while (*p)
    {
        int len = 0;
        while (*p && *p != '.' && len < (int)sizeof(key) - 1)
            key[len++] = *p++;
        key[len] = 0;

        if (!pNode->isObject() || !pNode->isMember(key))
            return nullptr;
        pNode = &(*pNode)[key];

        if (*p == '.')
            ++p;
    }

    return pNode;
}

UIColorF UIThemeMapper::ReadColor(const Json::Value& _root, const char* _path, const UIColorF& _fallback)
{
    const Json::Value* pNode = FindNode(_root, _path);
    if (!pNode || !pNode->isString())
        return _fallback;

    const char* text = pNode->asCString();
    if (!text || text[0] != '#')
        return _fallback;

    unsigned int hex = 0;
    if (sscanf(text + 1, "%x", &hex) != 1)
        return _fallback;

    return UIColorF::FromHex(hex | 0xFF000000);
}

float UIThemeMapper::ReadFloat(const Json::Value& _root, const char* _path, float _fallback)
{
    const Json::Value* pNode = FindNode(_root, _path);
    if (!pNode || !pNode->isNumeric())
        return _fallback;
    return pNode->asFloat();
}

jc::String UIThemeMapper::ReadString(const Json::Value& _root, const char* _path, const char* _fallback)
{
    const Json::Value* pNode = FindNode(_root, _path);
    if (!pNode || !pNode->isString())
        return jc::String(_fallback);
    return jc::String(pNode->asCString());
}

UIRuntimeTheme UIThemeMapper::Map(const Json::Value& _source, UIColorScheme _scheme)
{
    UIRuntimeTheme out = UIRuntimeTheme::EngineDefaults();
    if (!_source.isObject())
        return out;

    // ----- meta -----
    out.meta.id = ReadString(_source, "meta.id", out.meta.id.Source());
    out.meta.displayName = ReadString(_source, "meta.name", out.meta.displayName.Source());
    jc::String explicitScheme = ReadString(_source, "meta.scheme", "");

    // ----- surface -----
    out.surface.normalTop     = ReadColor(_source, "surface.normalTop",     out.surface.normalTop);
    out.surface.normalBottom  = ReadColor(_source, "surface.normalBottom",  out.surface.normalBottom);
    out.surface.hoverTop      = ReadColor(_source, "surface.hoverTop",      out.surface.hoverTop);
    out.surface.hoverBottom   = ReadColor(_source, "surface.hoverBottom",   out.surface.hoverBottom);
    out.surface.pressedTop    = ReadColor(_source, "surface.pressedTop",    out.surface.pressedTop);
    out.surface.pressedBottom = ReadColor(_source, "surface.pressedBottom", out.surface.pressedBottom);
    out.surface.disabledTop   = ReadColor(_source, "surface.disabledTop",   out.surface.disabledTop);
    out.surface.disabledBottom= ReadColor(_source, "surface.disabledBottom",out.surface.disabledBottom);
    out.surface.insetTop      = ReadColor(_source, "surface.insetTop",      out.surface.insetTop);
    out.surface.insetBottom   = ReadColor(_source, "surface.insetBottom",   out.surface.insetBottom);

    // ----- metal (JSON 그룹명: gold) -----
    out.metal.border    = ReadColor(_source, "gold.border",    out.metal.border);
    out.metal.hover     = ReadColor(_source, "gold.hover",     out.metal.hover);
    out.metal.highlight = ReadColor(_source, "gold.highlight", out.metal.highlight);
    out.metal.bright    = ReadColor(_source, "gold.bright",    out.metal.bright);
    out.metal.shadow    = ReadColor(_source, "gold.shadow",    out.metal.shadow);
    out.metal.muted     = ReadColor(_source, "gold.muted",     out.metal.muted);

    // ----- gloss -----
    out.gloss.tint              = ReadColor(_source, "gloss.tint",              out.gloss.tint);
    out.gloss.normalAlpha       = ReadFloat(_source, "gloss.normalAlpha",       out.gloss.normalAlpha);
    out.gloss.hoverMultiplier   = ReadFloat(_source, "gloss.hoverMultiplier",   out.gloss.hoverMultiplier);
    out.gloss.disabledMultiplier= ReadFloat(_source, "gloss.disabledMultiplier",out.gloss.disabledMultiplier);
    out.gloss.heightRatio       = ReadFloat(_source, "gloss.heightRatio",       out.gloss.heightRatio);
    out.gloss.blurRadius        = ReadFloat(_source, "gloss.blur",              out.gloss.blurRadius);
    out.gloss.centerX           = ReadFloat(_source, "gloss.centerX",           out.gloss.centerX);

    // ----- semantic -----
    out.semantic.text         = ReadColor(_source, "semantic.text",         out.semantic.text);
    out.semantic.disabledText = ReadColor(_source, "semantic.disabledText", out.semantic.disabledText);
    out.semantic.danger       = ReadColor(_source, "semantic.danger",       out.semantic.danger);
    out.semantic.success      = ReadColor(_source, "semantic.success",      out.semantic.success);
    out.semantic.warning      = ReadColor(_source, "semantic.warning",      out.semantic.warning);
    out.semantic.focus        = ReadColor(_source, "semantic.focus",        out.semantic.focus);

    // ----- geometry -----
    out.geometry.radius      = ReadFloat(_source, "geometry.radius",      out.geometry.radius);
    out.geometry.borderWidth = ReadFloat(_source, "geometry.borderWidth", out.geometry.borderWidth);
    out.geometry.innerRim    = ReadFloat(_source, "geometry.innerRim",    out.geometry.innerRim);
    out.geometry.depth       = ReadFloat(_source, "geometry.depth",       out.geometry.depth);
    out.geometry.shadow      = ReadFloat(_source, "geometry.shadow",      out.geometry.shadow);
    out.geometry.shadowAlpha = ReadFloat(_source, "geometry.shadowAlpha", out.geometry.shadowAlpha);

    // ----- state -----
    out.state.hoverLift        = ReadFloat(_source, "state.hoverLift",        out.state.hoverLift);
    out.state.checkedGoldMix   = ReadFloat(_source, "state.checkedGoldMix",   out.state.checkedGoldMix);
    out.state.disabledContrast = ReadFloat(_source, "state.disabledContrast", out.state.disabledContrast);

    UIColorScheme resolved = ResolveScheme(_scheme);
    if (resolved == UIColorScheme::Light && explicitScheme != "light")
        ApplyLightScheme(out);

    return out;
}

void UIThemeMapper::ApplyLightScheme(UIRuntimeTheme& _theme)
{
    // Dark JSON 기준 값을 Light 스킴용 밝은 팔레트로 변환한다.
    UIColorF white = UIColorF::FromRGBA(255, 255, 255);

    _theme.surface.normalTop     = Mix(_theme.surface.normalTop,     white, 68);
    _theme.surface.normalBottom  = Mix(_theme.surface.normalBottom,  white, 82);
    _theme.surface.hoverTop      = Mix(_theme.surface.hoverTop,      white, 72);
    _theme.surface.hoverBottom   = Mix(_theme.surface.hoverBottom,   white, 84);
    _theme.surface.pressedTop    = Mix(_theme.surface.pressedTop,    white, 60);
    _theme.surface.pressedBottom = Mix(_theme.surface.pressedBottom, white, 76);
    _theme.surface.disabledTop   = Mix(_theme.surface.disabledTop,   white, 78);
    _theme.surface.disabledBottom= Mix(_theme.surface.disabledBottom,white, 86);
    _theme.surface.insetTop      = Mix(_theme.surface.insetTop,      white, 58);
    _theme.surface.insetBottom   = Mix(_theme.surface.insetBottom,   white, 74);

    _theme.metal.border    = Mix(_theme.metal.border,    white, 30);
    _theme.metal.hover     = Mix(_theme.metal.hover,     white, 36);
    _theme.metal.highlight = Mix(_theme.metal.highlight, white, 40);
    _theme.metal.bright    = Mix(_theme.metal.bright,    white, 44);
    _theme.metal.shadow    = Mix(_theme.metal.shadow,    white, 22);
    _theme.metal.muted     = Mix(_theme.metal.muted,     white, 34);

    _theme.semantic.text         = UIColorF::FromRGBA(40, 42, 46);
    _theme.semantic.disabledText = UIColorF::FromRGBA(130, 132, 136);
}

UIColorScheme UIThemeMapper::ResolveScheme(UIColorScheme _requested)
{
    if (_requested != UIColorScheme::System)
        return _requested;

    return UIColorScheme::Dark;
}
