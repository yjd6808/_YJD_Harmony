# SGCL × Aurum Runtime Texture GUI 구현 계획

## 목표
Aurum Material Theme 생성 방식을 SGCL 자체 UI 라이브러리에 통합. GUI 외형을 PNG/SGA 프레임 번호 대신 Theme Token으로 정의하고, C++ CPU Texture Baker가 픽셀을 생성하며, 결과를 TextureSet으로 관리.

## Phase 1: 기존 버그 수정
1. **ParseSpriteIndices 배열 침범 수정** (`UIXmlLoader.cpp`)
   - `sscanf` 대신 `strtol` 기반 안전 파서로 교체
   - `_count` 파라미터를 준수하여 배열 오버플로우 방지

2. **RestoreState 인자 무시 수정** (`UIButton.cpp`, `UIToggleButton.cpp`, `UIElement.cpp`)
   - `_state` 인자를 실제로 사용하도록 수정
   - Disabled가 아닐 경우 전달받은 state로 복구

3. **GetAbsoluteScale 루프 변수 오류 수정** (`UIElement.cpp`)
   - `pParent_->_scaleX/Y` → `pCur->_scaleX/Y` 로 수정

4. **UISprite 초기 크기 검사 버그 수정** (`UISprite.cpp`)
   - `int(initialSize.width) == 0 || int(initialSize.width) == 0` → height도 검사

5. **UIProgressBar 자동 Action 제거** (`UIProgressBar.cpp`)
   - `ProgressTo::create(10.0f, 100)` 제거
   - 초기 Percent 0으로 유지

6. **UIProgressBar SetUISize 스케일 계산 버그 수정** (`UIProgressBar.cpp`)
   - `getScaleX/Y()` 대신 새 uiSize_ 기준으로 올바른 스케일 계산

## Phase 2: Theme Data Model
1. Theme 디렉토리 생성 `Game/UI/Theme/`
2. `UIThemeTypes.h` - 기본 타입 (UIColorF, UIStyleToken, UIThemeRevision 등)
3. `UIStyleOverride.h/.cpp` - Override Mask와 Sparse Override
4. `UIStyleResolver.h/.cpp` - Style Resolution (Engine Default → Global Theme → Control Default → XML Override)
5. `UIRuntimeTheme.h/.cpp` - Runtime Theme 구조체
6. `UIThemeMapper.h/.cpp` - Electron Theme JSON → Runtime Theme 매핑
7. `UIThemeJsonLoader.h/.cpp` - Theme JSON 로더
8. `UIThemeManager.h/.cpp` - 최상위 Theme Manager

## Phase 3: CPU Baker Prototype
1. `UIPixelBuffer.h/.cpp` - 픽셀 버퍼
2. `UIColorMath.h/.cpp` - 색상 수학
3. `UIMaskBuffer.h/.cpp` - 알파 마스크 버퍼
4. `UIGaussianBlur.h/.cpp` - 가우시안 블러
5. `UIGradientRasterizer.h/.cpp` - 그라디언트 래스터라이저
6. `UIShapeRasterizer.h/.cpp` - 둥근 사각형 SDF
7. `UITextureBaker.h/.cpp` - 메인 Baker
8. `UIControlBakeRecipes.h/.cpp` - 컨트롤별 레시피

## Phase 4: Texture Upload & UIButton 연결
1. `UITextureEntry.h` - 텍스처 엔트리
2. `UITextureSet.h/.cpp` - 텍스처 셋
3. `UITextureCache.h/.cpp` - 메모리 캐시
4. Main Thread Uploader
5. Scale9Sprite Binding
6. UIButton LoadTheme/LoadLegacy 분리
7. Theme Revision Event

## Phase 5: Cache & Async Pipeline
1. Worker Queue
2. Generation Cancellation
3. Debounce (150ms)
4. UITX Disk Cache
5. Atomic Set Swap
6. Deferred Release

## Phase 6: Composite Controls Theme Integration (완료)
1. **UICheckBox.cpp** - LoadTheme/LoadLegacy/BuildThemeVisuals/DestroyThemeVisuals 구현
2. **UIToggleButton.cpp** - LoadTheme/LoadLegacy/BuildThemeVisuals/DestroyThemeVisuals 구현
3. **UIProgressBar.cpp** - LoadTheme/LoadLegacy/BuildThemeVisuals/DestroyThemeVisuals 구현
4. Build 오류 수정 (85개 → 0개, link만 libcocos2d.lib 미존재로 실패)

## Build 검증
- sge: Build 성공
- sgcl: **Compile 100% 성공** (0개 error), Link만 libcocos2d.lib 부재로 실패
- libcocos2d.lib는 Full Solution에서 cocos2d-x 프로젝트 빌드 필요

## 파일 수정 목록 (Phase 1)
- `Projects/sgcl/Classes/sgcl/Game/UI/UIXmlLoader.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/UIButton.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/UIToggleButton.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/UIElement.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/UISprite.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/UIProgressBar.cpp`

## Phase 2-6 신규 파일 (Theme/Baker/Serialization)
(Phase 2-6에서 생성된 모든 신규 파일 - Theme 디렉토리 30+ 파일)
