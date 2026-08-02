#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIRuntimeTheme.h"

UIRuntimeTheme UIRuntimeTheme::EngineDefaults()
{
    UIRuntimeTheme theme;

    theme.meta.id = "engine-default";
    theme.meta.displayName = "Engine Default";

    // 그라데이션 테마: 상/하 표면 다른색으로 입체감 (과거 modern-dark-gold 룩). hover/pressed는 인지 가능한 델타.
    theme.surface.normalTop = UIColorF::FromRGBA(45, 48, 53);
    theme.surface.normalBottom = UIColorF::FromRGBA(17, 19, 24);
    theme.surface.hoverTop = UIColorF::FromRGBA(58, 61, 68);
    theme.surface.hoverBottom = UIColorF::FromRGBA(25, 28, 33);
    // pressed는 윈도우 배경(26,28,33)보다 확실히 어둡게 유지해야 클릭 피드백이 보인다.
    theme.surface.pressedTop = UIColorF::FromRGBA(16, 18, 23);
    theme.surface.pressedBottom = UIColorF::FromRGBA(38, 42, 48);
    theme.surface.disabledTop = UIColorF::FromRGBA(58, 60, 64);
    theme.surface.disabledBottom = UIColorF::FromRGBA(41, 43, 47);
    theme.surface.insetTop = UIColorF::FromRGBA(12, 14, 18);
    theme.surface.insetBottom = UIColorF::FromRGBA(24, 27, 32);

    theme.metal.border = UIColorF::FromRGBA(169, 138, 74);
    theme.metal.hover = UIColorF::FromRGBA(208, 175, 99);
    theme.metal.highlight = UIColorF::FromRGBA(216, 192, 120);
    theme.metal.bright = UIColorF::FromRGBA(240, 217, 149);
    theme.metal.shadow = UIColorF::FromRGBA(103, 81, 38);
    theme.metal.muted = UIColorF::FromRGBA(116, 103, 76);

    theme.gloss.tint = UIColorF::FromRGBA(255, 248, 226);
    theme.gloss.normalAlpha = 0.188f;
    theme.gloss.hoverMultiplier = 1.32f;
    theme.gloss.disabledMultiplier = 0.30f;
    theme.gloss.heightRatio = 0.45f;
    theme.gloss.blurRadius = 1.15f;
    theme.gloss.centerX = 0.42f;

    theme.semantic.text = UIColorF::FromRGBA(242, 240, 234);
    theme.semantic.disabledText = UIColorF::FromRGBA(133, 131, 125);
    theme.semantic.danger = UIColorF::FromRGBA(183, 103, 103);
    theme.semantic.success = UIColorF::FromRGBA(106, 169, 134);
    theme.semantic.warning = UIColorF::FromRGBA(198, 155, 80);
    theme.semantic.focus = UIColorF::FromRGBA(216, 192, 120);

    theme.geometry.radius = 4.0f;
    theme.geometry.borderWidth = 1.1f;
    theme.geometry.innerRim = 0.75f;
    theme.geometry.depth = 0.4f;
    theme.geometry.shadow = 1.0f;
    theme.geometry.borderGradient = false;

    theme.state.hoverLift = 2.0f;
    theme.state.checkedGoldMix = 0.15f;
    theme.state.disabledContrast = 0.5f;

    // 윈도우 크롬 (타이틀바/창 배경 + 테두리) — 테마 토큰으로 theme에서 제어한다.
    theme.window.windowBackground = UIColorF::FromRGBA(26, 28, 33);
    theme.window.titleBarBackground = UIColorF::FromRGBA(31, 33, 38);
    theme.window.titleBarForeground = UIColorF::FromRGBA(235, 237, 242);
    theme.window.borderColor = UIColorF::FromRGBA(88, 91, 99);
    theme.window.borderWidth = 1.0f;

    return theme;
}
