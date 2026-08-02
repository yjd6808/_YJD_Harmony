/*
 * 작성자: 윤정도
 * =====================
 * sgui - 현재 테마의 컨트롤/상태별 색상 테이블
 * - BuildDefaults(): UIRuntimeTheme 토큰(surface/semantic/gold/window)에서 기본값을 파생한다.
 * - LoadJson(): 테마 JSON의 "controls" 섹션으로 개별 색상을 오버라이드한다.
 *   지원 형식 1 (중첩): "button": { "hover": { "background": "#3A3D44", "foreground": "#FFFFFF" } }
 *   지원 형식 2 (평탄): "button": { "hover-background": "#3A3D44", "hover-foreground": "#FFFFFF" }
 * - 색상 문자열은 #RRGGBB 또는 #RRGGBBAA를 지원한다.
 *
 * 구현부는 UIThemeMapper.cpp에 있다. (신규 cpp 추가 없이 빌드되도록)
 */

#pragma once

#include "sgcl/Game/UI/Theme/UIThemeColor.h"
#include "sgcl/Game/UI/Theme/UIRuntimeTheme.h"

#include <json.h>

class UIThemeColorTable
{
public:
    UIThemeColorTable();

    // UIRuntimeTheme 토큰에서 전체 기본값을 파생한다. (테마 전환 시 항상 먼저 호출)
    void BuildDefaults(const UIRuntimeTheme& _theme);

    // 테마 JSON 루트에서 "controls" 섹션을 읽어 오버라이드한다.
    void LoadJson(const Json::Value& _root);

    const UIColorF& Get(UIThemeColor _color) const;
    const UIColorF& Get(UIThemeControl _control, UIThemeColorState _state, UIThemeColorRole _role) const;
    const UIColorF& Get(UIThemeControl _control, UIVisualState _state, UIThemeColorRole _role) const;

    void Set(UIThemeColor _color, const UIColorF& _value);
    void Set(UIThemeControl _control, UIThemeColorState _state, UIThemeColorRole _role, const UIColorF& _value);

    float GetBorderThickness(UIThemeControl _control) const;
    void SetBorderThickness(UIThemeControl _control, float _thickness);

private:
    // "#RRGGBB" 또는 "#RRGGBBAA" 문자열 노드를 색상으로 읽는다.
    static bool TryReadColor(const Json::Value& _node, UIColorF& _out);

    void LoadControlJson(UIThemeControl _control, const Json::Value& _node);
    void LoadStateJson(UIThemeControl _control, UIThemeColorState _state, const Json::Value& _node);

    UIColorF colors_[kUIThemeColorCount_v];
    float borderThickness_[kUIThemeControlCount_v];
};
