#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIThemeMapper.h"
#include "sgcl/Game/UI/Theme/UIStyleResolver.h"
#include "sgcl/Game/UI/Theme/UIThemeColorTable.h"

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
        {
            key[len++] = *p++;
        }
        key[len] = 0;

        if (!pNode->isObject() || !pNode->isMember(key))
            return nullptr;
        pNode = &(*pNode)[key];

        if (*p == '.')
        {
            ++p;
        }
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

bool UIThemeMapper::ReadBool(const Json::Value& _root, const char* _path, bool _fallback)
{
    const Json::Value* pNode = FindNode(_root, _path);
    if (!pNode)
        return _fallback;
    if (pNode->isBool())
        return pNode->asBool();
    if (pNode->isNumeric())
        return pNode->asInt() != 0;
    return _fallback;
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
    out.geometry.borderGradient = ReadBool(_source, "geometry.borderGradient", out.geometry.borderGradient);

    // ----- state -----
    out.state.hoverLift        = ReadFloat(_source, "state.hoverLift",        out.state.hoverLift);
    out.state.checkedGoldMix   = ReadFloat(_source, "state.checkedGoldMix",   out.state.checkedGoldMix);
    out.state.disabledContrast = ReadFloat(_source, "state.disabledContrast", out.state.disabledContrast);

    // ----- window -----
    out.window.windowBackground  = ReadColor(_source, "window.windowBackground",  out.window.windowBackground);
    out.window.titleBarBackground= ReadColor(_source, "window.titleBarBackground",out.window.titleBarBackground);
    out.window.titleBarForeground= ReadColor(_source, "window.titleBarForeground",out.window.titleBarForeground);
    out.window.borderColor       = ReadColor(_source, "window.borderColor",       out.window.borderColor);
    out.window.borderWidth       = ReadFloat(_source, "window.borderWidth",       out.window.borderWidth);

    UIColorScheme resolved = ResolveScheme(_scheme);
    if (resolved == UIColorScheme::Light && explicitScheme != "light")
    {
        ApplyLightScheme(out);
    }

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

    _theme.window.windowBackground   = Mix(_theme.window.windowBackground,   white, 84);
    _theme.window.titleBarBackground = Mix(_theme.window.titleBarBackground, white, 80);
    _theme.window.titleBarForeground = UIColorF::FromRGBA(40, 42, 46);
    _theme.window.borderColor        = Mix(_theme.window.borderColor,        white, 40);
}

UIColorScheme UIThemeMapper::ResolveScheme(UIColorScheme _requested)
{
    if (_requested != UIColorScheme::System)
        return _requested;

    return UIColorScheme::Dark;
}

// ============================================================================
// UIThemeColorTable 구현 (선언: UIThemeColorTable.h)
// - 신규 cpp 추가 없이 빌드되도록 UIThemeMapper.cpp에 배치한다.
// ============================================================================

UIThemeColorTable::UIThemeColorTable()
{
    BuildDefaults(UIRuntimeTheme::EngineDefaults());
}

void UIThemeColorTable::BuildDefaults(const UIRuntimeTheme& _theme)
{
    const UISurfaceTokens& surface = _theme.surface;
    const UIMetalTokens& metal = _theme.metal;
    const UISemanticTokens& semantic = _theme.semantic;
    const UIWindowTokens& window = _theme.window;

    // 상태별 공통 파생 규칙
    UIColorF background[kUIThemeColorStateCount_v];
    background[(int)UIThemeColorState::Normal] = surface.normalTop;
    background[(int)UIThemeColorState::Hover] = surface.hoverTop;
    background[(int)UIThemeColorState::Pressed] = surface.pressedTop;
    background[(int)UIThemeColorState::Disabled] = surface.disabledTop;
    background[(int)UIThemeColorState::Focused] = surface.normalTop;
    background[(int)UIThemeColorState::Checked] = surface.pressedTop;

    UIColorF foreground[kUIThemeColorStateCount_v];
    for (int state = 0; state < kUIThemeColorStateCount_v; ++state)
        foreground[state] = semantic.text;
    foreground[(int)UIThemeColorState::Disabled] = semantic.disabledText;

    UIColorF border[kUIThemeColorStateCount_v];
    border[(int)UIThemeColorState::Normal] = metal.border;
    border[(int)UIThemeColorState::Hover] = metal.hover;
    border[(int)UIThemeColorState::Pressed] = metal.shadow;
    border[(int)UIThemeColorState::Disabled] = metal.muted;
    border[(int)UIThemeColorState::Focused] = semantic.focus;
    border[(int)UIThemeColorState::Checked] = metal.highlight;

    for (int control = 0; control < kUIThemeControlCount_v; ++control)
    {
        for (int state = 0; state < kUIThemeColorStateCount_v; ++state)
        {
            Set((UIThemeControl)control, (UIThemeColorState)state, UIThemeColorRole::Background, background[state]);
            Set((UIThemeControl)control, (UIThemeColorState)state, UIThemeColorRole::Foreground, foreground[state]);
            Set((UIThemeControl)control, (UIThemeColorState)state, UIThemeColorRole::Border, border[state]);
        }
        borderThickness_[control] = _theme.geometry.borderWidth;
    }

    // 컨트롤별 특수 규칙
    // 트랙류(입력/게이지 배경)는 inset 표면을 사용한다.
    const UIThemeControl insetControls[] = {
        UIThemeControl::TextBox, UIThemeControl::ProgressBar,
        UIThemeControl::Slider, UIThemeControl::ScrollBar
    };
    for (UIThemeControl control : insetControls)
    {
        Set(control, UIThemeColorState::Normal, UIThemeColorRole::Background, surface.insetTop);
        Set(control, UIThemeColorState::Hover, UIThemeColorRole::Background, surface.insetTop);
        Set(control, UIThemeColorState::Pressed, UIThemeColorRole::Background, surface.insetBottom);
        Set(control, UIThemeColorState::Focused, UIThemeColorRole::Background, surface.insetTop);
        Set(control, UIThemeColorState::Checked, UIThemeColorRole::Background, surface.insetTop);
    }

    // ProgressBar/Slider/ScrollBar 의 Foreground 는 게이지/썸 색상으로 사용된다.
    const UIThemeControl gaugeControls[] = {
        UIThemeControl::ProgressBar, UIThemeControl::Slider, UIThemeControl::ScrollBar
    };
    for (UIThemeControl control : gaugeControls)
    {
        Set(control, UIThemeColorState::Normal, UIThemeColorRole::Foreground, metal.border);
        Set(control, UIThemeColorState::Hover, UIThemeColorRole::Foreground, metal.hover);
        Set(control, UIThemeColorState::Pressed, UIThemeColorRole::Foreground, metal.bright);
        Set(control, UIThemeColorState::Disabled, UIThemeColorRole::Foreground, metal.muted);
        Set(control, UIThemeColorState::Focused, UIThemeColorRole::Foreground, metal.hover);
        Set(control, UIThemeColorState::Checked, UIThemeColorRole::Foreground, metal.highlight);
    }

    // Window 는 window 토큰을 그대로 사용한다.
    for (int state = 0; state < kUIThemeColorStateCount_v; ++state)
    {
        Set(UIThemeControl::Window, (UIThemeColorState)state, UIThemeColorRole::Background, window.windowBackground);
        Set(UIThemeControl::Window, (UIThemeColorState)state, UIThemeColorRole::Foreground, window.titleBarForeground);
        Set(UIThemeControl::Window, (UIThemeColorState)state, UIThemeColorRole::Border, window.borderColor);
    }
    borderThickness_[(int)UIThemeControl::Window] = window.borderWidth;

    // TextBlock 은 글자색만 의미가 있다. (배경/테두리는 투명)
    for (int state = 0; state < kUIThemeColorStateCount_v; ++state)
    {
        Set(UIThemeControl::TextBlock, (UIThemeColorState)state, UIThemeColorRole::Background, UIColorF{ 0.0f, 0.0f, 0.0f, 0.0f });
        Set(UIThemeControl::TextBlock, (UIThemeColorState)state, UIThemeColorRole::Border, UIColorF{ 0.0f, 0.0f, 0.0f, 0.0f });
    }
    borderThickness_[(int)UIThemeControl::TextBlock] = 0.0f;
}

void UIThemeColorTable::LoadJson(const Json::Value& _root)
{
    if (!_root.isObject() || !_root.isMember("controls"))
        return;

    const Json::Value& controls = _root["controls"];
    if (!controls.isObject())
        return;

    for (int control = 0; control < kUIThemeControlCount_v; ++control)
    {
        const char* controlKey = UIThemeControlKey((UIThemeControl)control);
        if (!controls.isMember(controlKey))
            continue;

        const Json::Value& node = controls[controlKey];
        if (node.isObject())
            LoadControlJson((UIThemeControl)control, node);
    }
}

void UIThemeColorTable::LoadControlJson(UIThemeControl _control, const Json::Value& _node)
{
    if (_node.isMember("borderThickness") && _node["borderThickness"].isNumeric())
        borderThickness_[(int)_control] = _node["borderThickness"].asFloat();

    for (int state = 0; state < kUIThemeColorStateCount_v; ++state)
    {
        const char* stateKey = UIThemeColorStateKey((UIThemeColorState)state);

        // 형식 1: 중첩 오브젝트 "hover": { "background": "#..." }
        if (_node.isMember(stateKey) && _node[stateKey].isObject())
            LoadStateJson(_control, (UIThemeColorState)state, _node[stateKey]);

        // 형식 2: 평탄 키 "hover-background": "#..."
        for (int role = 0; role < kUIThemeColorRoleCount_v; ++role)
        {
            char flatKey[64];
            snprintf(flatKey, sizeof(flatKey), "%s-%s", stateKey, UIThemeColorRoleKey((UIThemeColorRole)role));

            UIColorF color;
            if (_node.isMember(flatKey) && TryReadColor(_node[flatKey], color))
                Set(_control, (UIThemeColorState)state, (UIThemeColorRole)role, color);
        }
    }
}

void UIThemeColorTable::LoadStateJson(UIThemeControl _control, UIThemeColorState _state, const Json::Value& _node)
{
    for (int role = 0; role < kUIThemeColorRoleCount_v; ++role)
    {
        const char* roleKey = UIThemeColorRoleKey((UIThemeColorRole)role);

        UIColorF color;
        if (_node.isMember(roleKey) && TryReadColor(_node[roleKey], color))
            Set(_control, _state, (UIThemeColorRole)role, color);
    }
}

bool UIThemeColorTable::TryReadColor(const Json::Value& _node, UIColorF& _out)
{
    if (!_node.isString())
        return false;

    const std::string text = _node.asString();
    if (text.empty() || text[0] != '#')
        return false;

    const size_t hexLength = text.size() - 1;
    unsigned int rgb = 0;
    unsigned int alpha = 0xFF;

    if (hexLength == 6)
    {
        if (sscanf(text.c_str() + 1, "%06x", &rgb) != 1)
            return false;
    }
    else if (hexLength == 8)
    {
        unsigned int rgba = 0;
        if (sscanf(text.c_str() + 1, "%08x", &rgba) != 1)
            return false;
        rgb = (rgba >> 8) & 0xFFFFFF;
        alpha = rgba & 0xFF;
    }
    else
    {
        return false;
    }

    _out = UIColorF::FromRGBA(
        (rgb >> 16) & 0xFF,
        (rgb >> 8) & 0xFF,
        rgb & 0xFF,
        alpha);
    return true;
}

const UIColorF& UIThemeColorTable::Get(UIThemeColor _color) const
{
    const int index = (int)_color;
    if (index < 0 || index >= kUIThemeColorCount_v)
    {
        static const UIColorF fallback_s{ 1.0f, 1.0f, 1.0f, 1.0f };
        return fallback_s;
    }
    return colors_[index];
}

const UIColorF& UIThemeColorTable::Get(UIThemeControl _control, UIThemeColorState _state, UIThemeColorRole _role) const
{
    return Get(MakeUIThemeColor(_control, _state, _role));
}

const UIColorF& UIThemeColorTable::Get(UIThemeControl _control, UIVisualState _state, UIThemeColorRole _role) const
{
    return Get(MakeUIThemeColor(_control, ToThemeColorState(_state), _role));
}

void UIThemeColorTable::Set(UIThemeColor _color, const UIColorF& _value)
{
    const int index = (int)_color;
    if (index < 0 || index >= kUIThemeColorCount_v)
        return;
    colors_[index] = _value;
}

void UIThemeColorTable::Set(UIThemeControl _control, UIThemeColorState _state, UIThemeColorRole _role, const UIColorF& _value)
{
    Set(MakeUIThemeColor(_control, _state, _role), _value);
}

float UIThemeColorTable::GetBorderThickness(UIThemeControl _control) const
{
    const int index = (int)_control;
    if (index < 0 || index >= kUIThemeControlCount_v)
        return 0.0f;
    return borderThickness_[index];
}

void UIThemeColorTable::SetBorderThickness(UIThemeControl _control, float _thickness)
{
    const int index = (int)_control;
    if (index < 0 || index >= kUIThemeControlCount_v)
        return;
    borderThickness_[index] = _thickness;
}
