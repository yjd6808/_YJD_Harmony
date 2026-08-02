/*
 * 작성자: 윤정도
 * =====================
 * sgui - 테마 컨트롤/상태별 색상 식별자 (WPF의 SystemColors/DynamicResource 컨셉)
 * 테마 JSON의 controls 섹션과 1:1로 대응하며, 코드 어디서든
 * UIThemeManager::Get()->GetColor(UIThemeColor::ButtonHoverBackground) 형태로
 * 현재 테마의 색상값을 조회할 수 있다.
 */

#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"

#include <cstring>
#include <cstdint>

// ==================== X-매크로 목록 ====================
// (EnumName, jsonKey) — jsonKey는 테마 JSON controls 섹션의 키와 일치해야 한다.

#define SGUI_THEME_CONTROL_LIST(X) \
    X(Button, "button") \
    X(ToggleButton, "toggleButton") \
    X(CheckBox, "checkBox") \
    X(Radio, "radio") \
    X(ProgressBar, "progressBar") \
    X(Slider, "slider") \
    X(ScrollBar, "scrollBar") \
    X(TextBox, "textBox") \
    X(Window, "window") \
    X(TextBlock, "textBlock")

#define SGUI_THEME_COLOR_STATE_LIST(X) \
    X(Normal, "normal") \
    X(Hover, "hover") \
    X(Pressed, "pressed") \
    X(Disabled, "disabled") \
    X(Focused, "focused") \
    X(Checked, "checked")

#define SGUI_THEME_COLOR_ROLE_LIST(X) \
    X(Background, "background") \
    X(Foreground, "foreground") \
    X(Border, "border")

// ==================== 구성 요소 enum ====================

enum class UIThemeControl : uint8_t
{
#define SGUI_ENUM_ENTRY(Name, key) Name,
    SGUI_THEME_CONTROL_LIST(SGUI_ENUM_ENTRY)
#undef SGUI_ENUM_ENTRY
    Count
};

enum class UIThemeColorState : uint8_t
{
#define SGUI_ENUM_ENTRY(Name, key) Name,
    SGUI_THEME_COLOR_STATE_LIST(SGUI_ENUM_ENTRY)
#undef SGUI_ENUM_ENTRY
    Count
};

enum class UIThemeColorRole : uint8_t
{
#define SGUI_ENUM_ENTRY(Name, key) Name,
    SGUI_THEME_COLOR_ROLE_LIST(SGUI_ENUM_ENTRY)
#undef SGUI_ENUM_ENTRY
    Count
};

constexpr int kUIThemeControlCount_v = (int)UIThemeControl::Count;
constexpr int kUIThemeColorStateCount_v = (int)UIThemeColorState::Count;
constexpr int kUIThemeColorRoleCount_v = (int)UIThemeColorRole::Count;
constexpr int kUIThemeColorCount_v = kUIThemeControlCount_v * kUIThemeColorStateCount_v * kUIThemeColorRoleCount_v;

// ==================== 평탄화된 색상 enum ====================
// 값 = (control * StateCount + state) * RoleCount + role

constexpr uint16_t UIThemeColorIndexOf(UIThemeControl _control, UIThemeColorState _state, UIThemeColorRole _role)
{
    return (uint16_t)((((uint16_t)_control * kUIThemeColorStateCount_v) + (uint16_t)_state) * kUIThemeColorRoleCount_v + (uint16_t)_role);
}

#define SGUI_THEME_COLOR_ENTRY(Control, State, Role) \
    Control##State##Role = UIThemeColorIndexOf(UIThemeControl::Control, UIThemeColorState::State, UIThemeColorRole::Role),

#define SGUI_THEME_COLOR_STATE_BLOCK(Control, State) \
    SGUI_THEME_COLOR_ENTRY(Control, State, Background) \
    SGUI_THEME_COLOR_ENTRY(Control, State, Foreground) \
    SGUI_THEME_COLOR_ENTRY(Control, State, Border)

#define SGUI_THEME_COLOR_CONTROL_BLOCK(Control, key) \
    SGUI_THEME_COLOR_STATE_BLOCK(Control, Normal) \
    SGUI_THEME_COLOR_STATE_BLOCK(Control, Hover) \
    SGUI_THEME_COLOR_STATE_BLOCK(Control, Pressed) \
    SGUI_THEME_COLOR_STATE_BLOCK(Control, Disabled) \
    SGUI_THEME_COLOR_STATE_BLOCK(Control, Focused) \
    SGUI_THEME_COLOR_STATE_BLOCK(Control, Checked)

enum class UIThemeColor : uint16_t
{
    SGUI_THEME_CONTROL_LIST(SGUI_THEME_COLOR_CONTROL_BLOCK)
    Count = kUIThemeColorCount_v
};

#undef SGUI_THEME_COLOR_CONTROL_BLOCK
#undef SGUI_THEME_COLOR_STATE_BLOCK
#undef SGUI_THEME_COLOR_ENTRY

// ==================== 조합/분해 헬퍼 ====================

struct UIThemeColorParts
{
    UIThemeControl control = UIThemeControl::Button;
    UIThemeColorState state = UIThemeColorState::Normal;
    UIThemeColorRole role = UIThemeColorRole::Background;
};

constexpr UIThemeColor MakeUIThemeColor(UIThemeControl _control, UIThemeColorState _state, UIThemeColorRole _role)
{
    return (UIThemeColor)UIThemeColorIndexOf(_control, _state, _role);
}

inline UIThemeColorParts DecomposeUIThemeColor(UIThemeColor _color)
{
    const uint16_t index = (uint16_t)_color;
    UIThemeColorParts parts;
    parts.role = (UIThemeColorRole)(index % kUIThemeColorRoleCount_v);
    parts.state = (UIThemeColorState)((index / kUIThemeColorRoleCount_v) % kUIThemeColorStateCount_v);
    parts.control = (UIThemeControl)(index / (kUIThemeColorRoleCount_v * kUIThemeColorStateCount_v));
    return parts;
}

// UIVisualState(런타임 상태) → 테마 색상 상태 매핑
inline UIThemeColorState ToThemeColorState(UIVisualState _state)
{
    switch (_state)
    {
    case UIVisualState::Normal:        return UIThemeColorState::Normal;
    case UIVisualState::Hover:         return UIThemeColorState::Hover;
    case UIVisualState::Pressed:       return UIThemeColorState::Pressed;
    case UIVisualState::Disabled:      return UIThemeColorState::Disabled;
    case UIVisualState::Focused:       return UIThemeColorState::Focused;
    case UIVisualState::Selected:      return UIThemeColorState::Checked;
    case UIVisualState::Checked:       return UIThemeColorState::Checked;
    case UIVisualState::Indeterminate: return UIThemeColorState::Checked;
    default:                           return UIThemeColorState::Normal;
    }
}

// ==================== JSON 키 이름 헬퍼 ====================

inline const char* UIThemeControlKey(UIThemeControl _control)
{
    switch (_control)
    {
#define SGUI_KEY_ENTRY(Name, key) case UIThemeControl::Name: return key;
    SGUI_THEME_CONTROL_LIST(SGUI_KEY_ENTRY)
#undef SGUI_KEY_ENTRY
    default: return "";
    }
}

inline const char* UIThemeColorStateKey(UIThemeColorState _state)
{
    switch (_state)
    {
#define SGUI_KEY_ENTRY(Name, key) case UIThemeColorState::Name: return key;
    SGUI_THEME_COLOR_STATE_LIST(SGUI_KEY_ENTRY)
#undef SGUI_KEY_ENTRY
    default: return "";
    }
}

inline const char* UIThemeColorRoleKey(UIThemeColorRole _role)
{
    switch (_role)
    {
#define SGUI_KEY_ENTRY(Name, key) case UIThemeColorRole::Name: return key;
    SGUI_THEME_COLOR_ROLE_LIST(SGUI_KEY_ENTRY)
#undef SGUI_KEY_ENTRY
    default: return "";
    }
}

inline bool TryParseUIThemeControl(const char* _text, UIThemeControl& _out)
{
    if (_text == nullptr) return false;
#define SGUI_PARSE_ENTRY(Name, key) if (strcmp(_text, key) == 0) { _out = UIThemeControl::Name; return true; }
    SGUI_THEME_CONTROL_LIST(SGUI_PARSE_ENTRY)
#undef SGUI_PARSE_ENTRY
    return false;
}

inline bool TryParseUIThemeColorState(const char* _text, UIThemeColorState& _out)
{
    if (_text == nullptr) return false;
#define SGUI_PARSE_ENTRY(Name, key) if (strcmp(_text, key) == 0) { _out = UIThemeColorState::Name; return true; }
    SGUI_THEME_COLOR_STATE_LIST(SGUI_PARSE_ENTRY)
#undef SGUI_PARSE_ENTRY
    return false;
}

inline bool TryParseUIThemeColorRole(const char* _text, UIThemeColorRole& _out)
{
    if (_text == nullptr) return false;
#define SGUI_PARSE_ENTRY(Name, key) if (strcmp(_text, key) == 0) { _out = UIThemeColorRole::Name; return true; }
    SGUI_THEME_COLOR_ROLE_LIST(SGUI_PARSE_ENTRY)
#undef SGUI_PARSE_ENTRY
    return false;
}
