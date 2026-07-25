# Phase 7: XML Theme Token Override

## 목표
XML UI 정의 파일에서 Theme 관련 속성을 직접 지정할 수 있도록 하여, C++ 코드 수정 없이도 컨트롤별 렌더링 모드와 스타일 오버라이드를 설정 가능하게 함.

## 작업 항목

### 1. `UIElementInfo`에 renderMode 필드 추가
- `SteinsGate_UI.h`: `UIElementInfo`에 `UIRenderMode renderMode_ = UIRenderMode::Auto` 추가
- 컨트롤들이 Info의 renderMode를 읽어서 `Load()`에서 사용

### 2. UIXmlLoader에 render_mode 파싱 추가
- `ReadCommon()` 에서 `render_mode="auto|legacy|theme"` XML 속성 파싱
- 값에 따라 `info->renderMode_` 설정

### 3. 컨트롤 Load()에서 Info renderMode 사용
- UIButton, UICheckBox, UIToggleButton, UIProgressBar
- `UIElement::Load()` 또는 각 컨트롤 `Load()`에서 `pBaseInfo_->renderMode_` 읽어서 `renderMode_` 설정

### 4. (선택) Style Override XML 속성
- `UIElementInfo`에 `jc::CDataMap<>* pStyleOverrides_` 추가
- XML에서 `style_radius="12"` 또는 `<override>` 자식 엘리먼트 파싱
- `UIStyleResolver`에 오버라이드 적용

## 영향 받는 파일
- `Projects/sg/Sources/sg/Struct/SteinsGate_UI.h`
- `Projects/sgcl/Classes/sgcl/Game/UI/UIXmlLoader.h`
- `Projects/sgcl/Classes/sgcl/Game/UI/UIXmlLoader.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/UIButton.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/UICheckBox.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/UIToggleButton.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/UIProgressBar.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/UIElement.h`
- `Projects/sgcl/Classes/sgcl/Game/UI/UIElement.cpp`

## 결과
- Build: Compile 100% 성공 (Link만 libcocos2d.lib 부재)
- XML: `render_mode="theme|legacy|auto"` 속성 지원
- 기존 XML 호환성: render_mode 속성 없는 경우 Auto(default) 유지 → 기존 동작 변경 없음
- `eRenderModeAuto`(0)일 때만 info에서 덮어쓰므로, C++에서 `SetRenderMode()`로 설정한 값이 우선권 가짐
