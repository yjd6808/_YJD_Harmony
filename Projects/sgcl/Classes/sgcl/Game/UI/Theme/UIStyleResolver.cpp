#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIStyleResolver.h"

UIResolvedStyle UIStyleResolver::Resolve(
    UIElementType_t /*_control*/,
    UIVisualState _state,
    const UIRuntimeTheme& _theme,
    const UIStyleOverride& _local) const
{
    UIResolvedStyle result = ApplyStateModifiers(_theme, _state);
    ApplySparseOverride(result, _local);
    ValidateAndClamp(result);
    return result;
}

UIResolvedStyle UIStyleResolver::ApplyStateModifiers(
    const UIRuntimeTheme& _theme,
    UIVisualState _state)
{
    UIResolvedStyle out;
    ApplySurfaceToken(out, _theme.surface, _state);
    ApplyMetalToken(out, _theme.metal, _state);
    ApplyGlossToken(out, _theme.gloss, _state);

    out.semanticText = _theme.semantic.text;
    out.semanticDanger = _theme.semantic.danger;
    out.semanticSuccess = _theme.semantic.success;
    out.semanticWarning = _theme.semantic.warning;

    out.geometryRadius = _theme.geometry.radius;
    out.geometryBorderWidth = _theme.geometry.borderWidth;
    out.geometryInnerRim = _theme.geometry.innerRim;
    out.geometryDepth = _theme.geometry.depth;
    out.geometryShadow = _theme.geometry.shadow;

    return out;
}

void UIStyleResolver::ApplyGlobalTheme(
    UIResolvedStyle& _out,
    const UIRuntimeTheme& _theme,
    UIVisualState _state)
{
    ApplySurfaceToken(_out, _theme.surface, _state);
    ApplyMetalToken(_out, _theme.metal, _state);
    ApplyGlossToken(_out, _theme.gloss, _state);

    _out.semanticText = _theme.semantic.text;
    _out.semanticDanger = _theme.semantic.danger;
    _out.semanticSuccess = _theme.semantic.success;
    _out.semanticWarning = _theme.semantic.warning;

    _out.geometryRadius = _theme.geometry.radius;
    _out.geometryBorderWidth = _theme.geometry.borderWidth;
    _out.geometryInnerRim = _theme.geometry.innerRim;
    _out.geometryDepth = _theme.geometry.depth;
    _out.geometryShadow = _theme.geometry.shadow;
}

void UIStyleResolver::ApplySparseOverride(
    UIResolvedStyle& _out,
    const UIStyleOverride& _local)
{
    if (_local.Has(UIStyleToken::SurfaceNormalTop))
        _out.surfaceTop = _local.values.surfaceTop;
    if (_local.Has(UIStyleToken::SurfaceNormalBottom))
        _out.surfaceBottom = _local.values.surfaceBottom;
    if (_local.Has(UIStyleToken::MetalBorder))
        _out.metalBorder = _local.values.metalBorder;
    if (_local.Has(UIStyleToken::GlossTint))
        _out.glossTint = _local.values.glossTint;
    if (_local.Has(UIStyleToken::GlossNormalAlpha))
        _out.glossAlpha = _local.values.glossAlpha;
    if (_local.Has(UIStyleToken::GeometryRadius))
        _out.geometryRadius = _local.values.geometryRadius;
    if (_local.Has(UIStyleToken::GeometryBorderWidth))
        _out.geometryBorderWidth = _local.values.geometryBorderWidth;
    if (_local.Has(UIStyleToken::GeometryInnerRim))
        _out.geometryInnerRim = _local.values.geometryInnerRim;
    if (_local.Has(UIStyleToken::GeometryDepth))
        _out.geometryDepth = _local.values.geometryDepth;
    if (_local.Has(UIStyleToken::GeometryShadow))
        _out.geometryShadow = _local.values.geometryShadow;
    if (_local.Has(UIStyleToken::SemanticText))
        _out.semanticText = _local.values.semanticText;
    if (_local.Has(UIStyleToken::SemanticDanger))
        _out.semanticDanger = _local.values.semanticDanger;
    if (_local.Has(UIStyleToken::SemanticSuccess))
        _out.semanticSuccess = _local.values.semanticSuccess;
    if (_local.Has(UIStyleToken::SemanticWarning))
        _out.semanticWarning = _local.values.semanticWarning;
}

void UIStyleResolver::ValidateAndClamp(UIResolvedStyle& _out)
{
    if (_out.geometryRadius < 0.0f)
        _out.geometryRadius = 0.0f;
    if (_out.geometryBorderWidth < 0.0f)
        _out.geometryBorderWidth = 0.0f;
    if (_out.geometryShadow < 0.0f)
        _out.geometryShadow = 0.0f;
    if (_out.glossAlpha < 0.0f)
        _out.glossAlpha = 0.0f;
    if (_out.glossAlpha > 1.0f)
        _out.glossAlpha = 1.0f;
}

void UIStyleResolver::ApplySurfaceToken(
    UIResolvedStyle& _out,
    const UISurfaceTokens& _surface,
    UIVisualState _state)
{
    switch (_state)
    {
    case UIVisualState::Normal:
        _out.surfaceTop = _surface.normalTop;
        _out.surfaceBottom = _surface.normalBottom;
        break;
    case UIVisualState::Hover:
        _out.surfaceTop = _surface.hoverTop;
        _out.surfaceBottom = _surface.hoverBottom;
        break;
    case UIVisualState::Pressed:
        _out.surfaceTop = _surface.pressedTop;
        _out.surfaceBottom = _surface.pressedBottom;
        break;
    case UIVisualState::Disabled:
        _out.surfaceTop = _surface.disabledTop;
        _out.surfaceBottom = _surface.disabledBottom;
        break;
    default:
        _out.surfaceTop = _surface.normalTop;
        _out.surfaceBottom = _surface.normalBottom;
        break;
    }

    _out.surfaceInsetTop = _surface.insetTop;
    _out.surfaceInsetBottom = _surface.insetBottom;
}

void UIStyleResolver::ApplyMetalToken(
    UIResolvedStyle& _out,
    const UIMetalTokens& _metal,
    UIVisualState /*_state*/)
{
    _out.metalBorder = _metal.border;
    _out.metalHover = _metal.hover;
    _out.metalHighlight = _metal.highlight;
    _out.metalBright = _metal.bright;
    _out.metalShadow = _metal.shadow;
    _out.metalMuted = _metal.muted;
}

void UIStyleResolver::ApplyGlossToken(
    UIResolvedStyle& _out,
    const UIGlossTokens& _gloss,
    UIVisualState _state)
{
    _out.glossTint = _gloss.tint;

    switch (_state)
    {
    case UIVisualState::Normal:
        _out.glossAlpha = _gloss.normalAlpha;
        break;
    case UIVisualState::Hover:
        _out.glossAlpha = _gloss.normalAlpha * _gloss.hoverMultiplier;
        break;
    case UIVisualState::Pressed:
        _out.glossAlpha = 0.0f;
        break;
    case UIVisualState::Disabled:
        _out.glossAlpha = _gloss.normalAlpha * _gloss.disabledMultiplier;
        break;
    default:
        _out.glossAlpha = _gloss.normalAlpha;
        break;
    }
}
