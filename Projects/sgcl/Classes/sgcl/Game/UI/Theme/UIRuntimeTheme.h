#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"

struct UIMetaTokens
{
    jc::String id;
    jc::String displayName;
    uint64_t canonicalHash = 0;
};

struct UISurfaceTokens
{
    UIColorF normalTop;
    UIColorF normalBottom;
    UIColorF hoverTop;
    UIColorF hoverBottom;
    UIColorF pressedTop;
    UIColorF pressedBottom;
    UIColorF disabledTop;
    UIColorF disabledBottom;
    UIColorF insetTop;
    UIColorF insetBottom;
};

struct UIMetalTokens
{
    UIColorF border;
    UIColorF hover;
    UIColorF highlight;
    UIColorF bright;
    UIColorF shadow;
    UIColorF muted;
};

struct UIGlossTokens
{
    UIColorF tint;
    float normalAlpha = 0.35f;
    float hoverMultiplier = 1.25f;
    float disabledMultiplier = 0.35f;
    float heightRatio = 0.55f;
    float blurRadius = 3.0f;
    float centerX = 0.5f;
};

struct UISemanticTokens
{
    UIColorF text;
    UIColorF disabledText;
    UIColorF danger;
    UIColorF success;
    UIColorF warning;
    UIColorF focus;
};

struct UIGeometryTokens
{
    float radius = 8.0f;
    float borderWidth = 1.0f;
    float innerRim = 0.0f;
    float depth = 1.0f;
    float shadow = 4.0f;
    float shadowAlpha = 0.48f;

    // 테두리 금속 그라데이션(highlight→border→shadow 대각선) on/off.
    // true면 ResolveMetalColor 그라데이션, false면 단색 metalBorder.
    bool borderGradient = false;

    void Scale(float _factor)
    {
        radius *= _factor;
        borderWidth *= _factor;
        innerRim *= _factor;
        depth *= _factor;
        shadow *= _factor;
    }
};

struct UIStateTokens
{
    float hoverLift = 2.0f;
    float checkedGoldMix = 0.15f;
    float disabledContrast = 0.5f;
};

struct UIWindowTokens
{
    UIColorF windowBackground;
    UIColorF titleBarBackground;
    UIColorF titleBarForeground;
    UIColorF borderColor;
    float borderWidth = 1.0f;

    void Scale(float _factor)
    {
        borderWidth *= _factor;
    }
};

struct UIRuntimeTheme
{
    UIMetaTokens meta;
    UISurfaceTokens surface;
    UIMetalTokens metal;
    UIGlossTokens gloss;
    UISemanticTokens semantic;
    UIGeometryTokens geometry;
    UIStateTokens state;
    UIWindowTokens window;

    static UIRuntimeTheme EngineDefaults();
};
