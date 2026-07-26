# Phase 9.5 — Theme Controls Refactor

## Goal
UIButton/UICheckBox/UIToggleButton/UIProgressBar가 SGA/sprite 속성 없이 Theme 시스템만으로 완전히 동작하는 공용 GUI 컨트롤로 전환

## Changes

### 1. UIButton
- `init()`: SGA 의존 제거 → `LoadLegacy()`에서만 SGA 검증
- `Unload()`: Theme 모드에서 `themeBinding_.Clear()` 누락 수정
- `SetUISize()`: Theme 모드에서 Scale9Sprite 크기 업데이트
- `SetVisibleState()`: Theme 모드에서 `setColor`/`setOpacity`로 상태 시각화
- `BuildThemeVisuals()`: Hover/Pressed/Disabled 색상 오버라이드 지원

### 2. UICheckBox
- `init()`: CrossSga 의존 제거 → `LoadLegacy()`에서만 검증
- `RefreshThemeVisuals()`: protected → public
- `SetEnabled()`: Theme 모드에서 legacy pSprite_ 참조 제거
- `SetUISize()`: Theme 모드에서 Scale9Sprite 업데이트

### 3. UIToggleButton
- `init()`: Sga 의존 제거 → `LoadLegacy()`에서만 검증
- `RefreshThemeVisuals()`: protected → public
- `SetVisibleState()`: Theme 모드에서 knob 위치/색상 업데이트
- `SetUISize()`: Theme 모드에서 track/knob 업데이트

### 4. UIProgressBar
- `RefreshThemeVisuals()`: protected → public
- `BuildThemeVisuals()`: pGaugeCap_ 생성
- `SetUISize()`: Gauge 크기도 업데이트
