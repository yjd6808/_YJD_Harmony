#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIRuntimeTheme.h"
#include "sgcl/Game/UI/Theme/UIStyleOverride.h"
#include "sg/Struct/SteinsGate_UI.h"

class UIStyleResolver
{
public:
    UIResolvedStyle Resolve(
        UIElementType_t _control,
        UIVisualState _state,
        const UIRuntimeTheme& _theme,
        const UIStyleOverride& _local) const;

    static UIResolvedStyle ApplyStateModifiers(
        const UIRuntimeTheme& _theme,
        UIVisualState _state);

    static void ApplyGlobalTheme(
        UIResolvedStyle& _out,
        const UIRuntimeTheme& _theme,
        UIVisualState _state);

    static void ApplySparseOverride(
        UIResolvedStyle& _out,
        const UIStyleOverride& _local);

    static void ValidateAndClamp(
        UIResolvedStyle& _out);

private:
    static void ApplySurfaceToken(
        UIResolvedStyle& _out,
        const UISurfaceTokens& _surface,
        UIVisualState _state);

    static void ApplyMetalToken(
        UIResolvedStyle& _out,
        const UIMetalTokens& _metal,
        UIVisualState _state);

    static void ApplyGlossToken(
        UIResolvedStyle& _out,
        const UIGlossTokens& _gloss,
        UIVisualState _state);
};
