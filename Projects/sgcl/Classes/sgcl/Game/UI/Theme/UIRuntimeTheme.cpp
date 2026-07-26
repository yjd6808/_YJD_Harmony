#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIRuntimeTheme.h"

UIRuntimeTheme UIRuntimeTheme::EngineDefaults()
{
    UIRuntimeTheme theme;

    theme.meta.id = "engine-default";
    theme.meta.displayName = "Engine Default";

    theme.surface.normalTop = UIColorF::FromRGBA(58, 62, 70);
    theme.surface.normalBottom = UIColorF::FromRGBA(42, 46, 54);
    theme.surface.hoverTop = UIColorF::FromRGBA(72, 78, 90);
    theme.surface.hoverBottom = UIColorF::FromRGBA(52, 57, 66);
    theme.surface.pressedTop = UIColorF::FromRGBA(30, 33, 40);
    theme.surface.pressedBottom = UIColorF::FromRGBA(38, 42, 50);
    theme.surface.disabledTop = UIColorF::FromRGBA(48, 48, 48);
    theme.surface.disabledBottom = UIColorF::FromRGBA(40, 40, 40);
    theme.surface.insetTop = UIColorF::FromRGBA(20, 20, 25, 40);
    theme.surface.insetBottom = UIColorF::FromRGBA(50, 55, 65, 60);

    theme.metal.border = UIColorF::FromRGBA(180, 148, 80);
    theme.metal.hover = UIColorF::FromRGBA(200, 170, 100);
    theme.metal.highlight = UIColorF::FromRGBA(230, 210, 160);
    theme.metal.bright = UIColorF::FromRGBA(210, 190, 140);
    theme.metal.shadow = UIColorF::FromRGBA(100, 80, 50);
    theme.metal.muted = UIColorF::FromRGBA(80, 75, 65);

    theme.gloss.tint = UIColorF::FromRGBA(255, 255, 255);
    theme.gloss.normalAlpha = 0.35f;
    theme.gloss.hoverMultiplier = 1.25f;
    theme.gloss.disabledMultiplier = 0.35f;
    theme.gloss.heightRatio = 0.55f;
    theme.gloss.blurRadius = 3.0f;
    theme.gloss.centerX = 0.5f;

    theme.semantic.text = UIColorF::FromRGBA(220, 220, 220);
    theme.semantic.disabledText = UIColorF::FromRGBA(120, 120, 120);
    theme.semantic.danger = UIColorF::FromRGBA(220, 60, 60);
    theme.semantic.success = UIColorF::FromRGBA(60, 200, 100);
    theme.semantic.warning = UIColorF::FromRGBA(220, 180, 40);
    theme.semantic.focus = UIColorF::FromRGBA(100, 160, 240);

    theme.geometry.radius = 8.0f;
    theme.geometry.borderWidth = 1.0f;
    theme.geometry.innerRim = 0.0f;
    theme.geometry.depth = 1.0f;
    theme.geometry.shadow = 4.0f;

    theme.state.hoverLift = 2.0f;
    theme.state.checkedGoldMix = 0.15f;
    theme.state.disabledContrast = 0.5f;

    return theme;
}
