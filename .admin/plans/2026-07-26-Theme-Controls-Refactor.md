# Theme Controls Refactor - 작업 완료

**날짜**: 2026-07-26  
**목적**: UIButton/UICheckBox/UIToggleButton/UIProgressBar를 SGA/sprite 의존 없이 공용 Theme 기반 GUI 컨트롤로 전환

## 변경 파일

### UIButton
- `UIButton.h`: `ApplyThemeStateVisuals()` + `themeRoot_` 멤버 추가
- `UIButton.cpp`:
  - `init()`: SGA 패키지 체크를 nullable로 변경 (Theme 모드에서 실패하지 않음)
  - `SetVisibleState()`: Theme 모드에서 `ApplyThemeStateVisuals()` 호출
  - `SetUISize()`: Theme 모드에서 `themeRoot_` Scale9Sprite contentSize 갱신
  - `BuildThemeVisuals()`: `themeRoot_` 저장
  - `DestroyThemeVisuals()`: `themeRoot_` nullptr 초기화
  - `Unload()`: Theme 모드에서 `themeBinding_` + `themeRoot_` 정리
  - `Load()`: Theme 모드 초기 상태 시각화 적용
  - `ApplyThemeStateVisuals()`: 신규 - state별 Color/Opacity 처리

### UICheckBox
- `UICheckBox.h`: `RefreshThemeVisuals()` public으로 이동
- `UICheckBox.cpp`:
  - `init()`: SGA 패키지 체크를 nullable로 변경
  - `SetCheck()`: 중복 코드 제거 (양쪽 분기가 동일)
  - `SetEnabled()`: Theme 모드에서 Color/Opacity 처리
  - `SetUISize()`: Theme 모드에서 early return

### UIToggleButton
- `UIToggleButton.h`: `RefreshThemeVisuals()` public, `ApplyThemeStateVisuals()` + `themeTrack_`/`themeKnob_` 추가
- `UIToggleButton.cpp`:
  - `init()`: SGA 패키지 체크 nullable
  - `SetToggleState()`: Theme 모드에서 knob 위치 갱신
  - `SetVisibleState()`: Theme 모드에서 `ApplyThemeStateVisuals()` 호출
  - `BuildThemeVisuals()`: `themeTrack_`/`themeKnob_` 저장
  - `DestroyThemeVisuals()`: track/knob 참조 초기화
  - `Unload()`: track/knob 참조 초기화
  - `SetUISize()`: Theme 모드에서 track resize
  - `Load()`: Theme 모드 초기 상태 시각화 적용
  - `ApplyThemeStateVisuals()`: 신규

### UIProgressBar
- `UIProgressBar.h`: `RefreshThemeVisuals()` public으로 이동
- `UIProgressBar.cpp`:
  - `BuildThemeVisuals()`: ProgressCap (`BindFixed`) Sprite 생성, `pGaugeCap_`에 저장
  - `SetUISize()`: Theme 모드에서 gauge도 contentSize 갱신, `UpdateGaugeGeometry()` 호출
  - `UpdateGaugeGeometry()`: cap 위치/스케일 지정 로직 추가

## 빌드 결과
- `sgcl.exe` (x64 Debug): **성공** (오류 0)
- 선행 빌드: sge, sgcc, libcocos2d, sg, jnet 모두 정상
