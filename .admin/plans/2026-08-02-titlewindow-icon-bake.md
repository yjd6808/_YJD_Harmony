# 2026-08-02 WindowStyle(타이틀바) + SVG 아이콘 테마 베이크

## 목표
1. **Window에 WindowStyle 통합** — WPF `WindowStyle` 컨셉: 루트는 항상 `<Window>`, `WindowStyle="Title"` 속성으로 타이틀바(좌측 title + 우측 min/max/close 버튼) 활성화. 기본 `WindowStyle="None"`(기존 Window 동작 그대로)
2. **SVG 아이콘 로드 → 텍스처화**: `assets/res_data/gfx/mdi`의 SVG를 로드해 텍스처로 만들어 min/max/close 버튼에 표시 (nanosvg)
3. **아이콘도 bake 요소**: 테마 베이크 시스템(UIAssetSemantic)에 아이콘 3종 추가 — 스킴/스타일 변경 시 재베이크되고 아이콘 색상이 테마를 따라감
4. Title 속성 디폴트 지원 (기본값 "Untitled")

## 확정 사항 (사용자 답변)
- **TitleWindow 클래스 신설 금지** → Window 하나로 통일, `WindowStyle { None(기본), Title }` 전환 (XML/코드 모두)
  - XML: `<Window WindowStyle="Title" Title="...">` (루트 태그는 항상 Window)
  - 코드: `SetWindowStyle(WindowStyle::Title)` / `SetTitle("...")`
- 버튼 아이콘: mdi SVG 텍스처 (파일 없으면 텍스트 문자 폴백)
- 아이콘은 테마의 영향을 받아야 함 (bake 요소) — 아이콘 틴트 색상 = 테마 semanticText
- 테마/스키마는 동적 조절 가능한 구조 (setter + RefreshThemeVisuals 훅)
- 적용 대상: 신규 데모만 (UI_TitleDemo)

## 구조 파악 결과
- **테마 베이크 흐름**: `UIThemeManager::ApplyTheme` → `BuildThemeVariants`(시맨틱별 스타일 resolve) → `UITextureBakeService::EnqueueRequest` → `BuildTextureSet`(SelectRecipe → UITextureBaker::Bake → Texture2D → UITextureSet) → `SwapTextureSet`(textureRevision++ → UIHost::PollThemeRevision → RefreshThemeVisuals)
- **적용 흐름**: `BrushVisual`(ThemeBrush: semantic+controlType+slot+fixedSize) → `UIThemeBinding::BindFixed(Sprite, key=For(semantic, styleHash), slot)` → `Refresh(set)` — styleHash가 스타일에 따라 바뀌므로 스킴 변경 시 다른 키로 베이크됨 ✓
- **아이콘 색상**: `UIResolvedStyle.semanticText` = `_theme.semantic.text` (스킴별로 다름) — 아이콘 틴트에 사용
- mdi 폴더: `assets/res_data/gfx/mdi` 에 SVG 수백 개 있음. min/max/close용 SVG는 현재 없음 → 이름 규칙 `window-minimize.svg`/`window-maximize.svg`/`window-close.svg` 로 로드, 없으면 텍스트 폴백
- 프로젝트에 SVG 파서 없음 → **nanosvg** ThirdParty 추가 (사용자 승인)

## 변경 설계

### 1. nanosvg (신규 ThirdParty) — 완료
- `ThirdParty/nanosvg/nanosvg.h` + `nanosvgrast.h` (GitHub raw)
- `sgcl.vcxproj` IncludePath/AdditionalIncludeDirectories 8곳에 `$(SolutionDir)ThirdParty\nanosvg` 추가

### 2. 테마 시스템 — 아이콘 bake — 완료
- `UIThemeTypes.h`: `UIAssetSemantic`에 `WindowIconMinimize`/`WindowIconMaximize`/`WindowIconClose` 추가
- `UIPixelBuffer.h`: `UIAssetRecipe`에 `jc::String svgPath` 추가
- `UIControlBakeRecipes.h/.cpp`: `WindowIconRecipe(UIAssetSemantic, int size)` — svg 파일명 설정 (window-{minimize,maximize,close}.svg, 24px)
- `UITextureBaker.cpp`: `RenderSvgIcon` — 파일 로드 → `nsvgParse`(문자열) → `nsvgRasterize`(supersample 작업 버퍼로 확대 래스터) → RGB를 틴트색(스타일.semanticText)로, 알파는 SVG 유지 → UIPixelBuffer. 실패 시 도형 폴백
- `UITextureBakeService`: `SetIconDirectory` 보유, SelectRecipe에 아이콘 케이스, svgPath 조합(`jc::Path::Combine`)
- `UIThemeManager::BuildThemeVariants`: 아이콘 3종 variants 추가 (Button 타입으로 resolve → semanticText 확보), Initialize에서 `resDataPath/gfx/mdi` 설정

### 3. Window.h/.cpp — WindowStyle + 타이틀바 크롬 (신규 작업)
- `enum class WindowStyle : uint8_t { None, Title };` (기본 None = 기존 동작)
- `SetWindowStyle(WindowStyle)` / `GetWindowStyle()` — Title 전환 시 크롬 빌드
- `SetTitle(const char*)` / `GetTitle()` — 기본값 "Untitled"
- 타이틀바 크롬 구조 (WindowStyle::Title일 때만):
  - `pChrome_`(Border: 창 배경/테두리) → `pLayoutGrid_`(Grid 2행: Auto 타이틀바 / Star 콘텐츠)
  - 타이틀바 `pTitleBar_`(Border: 배경/테두리) → `pTitleBarGrid_`(Grid 3열: Auto title / Star 스페이서 / Auto 버튼)
  - 캡션 버튼 3개(28x28): 배경 ThemeBrush(Button) + 아이콘 `BrushVisual`(ThemeBrush 아이콘 semantic, fixedSize, slot=Icon)
- 상태 머신: Normal / Minimized(콘텐츠 Collapsed + 높이=타이틀바+테두리) / Maximized(호스트 전체) — `savedRect_`(y-down)로 복원
- API: `Minimize` / `Maximize` / `Restore` / `ToggleMaximize` / `IsMinimized` / `IsMaximized`
- 동적 조절: `SetTitleBarHeight` / `SetTitleBarBackground` / `SetTitleBarForeground` / `SetWindowBackground` / `SetWindowBorderBrush` (기본 다크 톤), `RefreshThemeVisuals()` 오버라이드로 스킴 전환 시 색상 갱신
- 드래그: 타이틀바에 AddHandler(ueMouseDown/Move/Up, handledEventsToo=false → 버튼 클릭은 제외) + CaptureMouse
- `virtual UIElement* GetContentRoot()` — None이면 this, Title이면 콘텐츠 호스트 (XML 부착 대상)
- 창 크기 동기화: `OnRenderSizeChanged`에서 chrome 크기를 창 renderSize로 갱신

### 4. UIXmlLoader — WindowStyle/Title 속성
- 루트 속성 파싱: `WindowStyle`("None"/"Title") / `Title` / `TitleBarHeight` / `TitleBarBackground` / `TitleBarForeground` / `WindowBackground` / `WindowBorderBrush`
- 자식 부착: `pWindow->GetContentRoot()` 기준 (None 모드는 this — 기존 호환 유지)

### 5. 데모
- `UI_TitleDemo.h/.cpp` (Window 상속, REGISTER_WINDOW "ui_title_demo")
- `ui_title_demo.xml` — `<Window WindowStyle="Title" Title="..." Width=... Height=...>` + 콘텐츠
- OnInit(XML) 검증 로그: 버튼/아이콘 스프라이트 존재, 최소화/복원/최대화 전이 로그
- Scene_Login에 임시 Show → 검증 후 원복

## 작업 절차
1. [x] plan 작성 (본 문서, WindowStyle 통합으로 갱신)
2. [x] nanosvg ThirdParty 추가
3. [x] 테마 시스템 아이콘 bake (시맨틱/레시피/Baker/Manager)
4. [x] Window.h/.cpp: WindowStyle + 타이틀바 크롬 + 상태머신 + GetContentRoot
5. [x] UIXmlLoader: WindowStyle/Title 속성 + GetContentRoot 부착
6. [x] 데모 (UI_TitleDemo + XML)
7. [x] Scene_Login 임시 Show → 빌드 0 에러 (Build.ps1 sgcl Debug x64)
8. [x] 런타임 검증: ui_title_demo 로드/아이콘 바인딩/최소화·최대화 동작 로그
9. [x] Scene_Login 원복 + plan/progress/done 문서

## 추가 수정 (검증 과정)
- **크래시 수정**: `jc::File::ReadAllText`가 파일 없을 때 `RuntimeException` throw(File.cpp:217) → RenderSvgIcon에 `jc::File::Exist` 가드 추가 (파일 없으면 도형 폴백, 크래시 없음)
- **`ButtonBase::SimulateClick()` 추가** (Button.h) — 데모에서 캡션 버튼 클릭 시뮬레이션용
- **UI_TitleDemo OnLoaded**에 scheduleOnce 순차 클릭 시뮬레이션 (0.3s minimize → 0.6s restore → 0.9s maximize → 1.2s close)
- Scene_Login: `Show("ui_login")`로 원복

## 검증 결과 (런타임 로그, 2026-08-02)
- 아이콘 3종(semantic=18/19/20) bake 성공 — SVG 없음 → `[UITextureBaker] SVG 파일이 존재하지 않습니다` + 도형 폴백, textureSet entries=20
- `[Window] ui_title_demo title bar chrome built` / `[UI_TitleDemo] OnInit(XML) chrome=1 ... size=480x320` / caption buttons 3개 생성
- 클릭 시뮬레이션 전이: `minimized (height=34)` → `restored (pos=(400,200) size=(480x320))` → `maximized` → `RemoveWindow` → `~Window rc=0 running=0` (파괴 정상)
- 빌드 0 에러 0 경고 (원복 후)

## 검증 기준
- [x] `[Bake] semantic=18/19/20` 로그 — 아이콘이 테마 bake에 포함되어 텍스처 생성됨
- [x] `[UI_TitleDemo]` 최소화/최대화/복원 상태 전이 로그 + 크기/가시성 값 일치
- [x] `<Window WindowStyle="Title">` XML 로드 + 콘텐츠가 콘텐츠 영역에 배치 (타이틀바와 분리)
- [ ] 테마 갱신(스킴 전환) 시 아이콘 키(styleHash)가 바뀌어 재베이크되고 타이틀바 색상 갱신 경로 존재 (RefreshThemeVisuals 구현됨, 동적 검증은 후속)
- [x] SVG 파일 없을 때 텍스트 문자 폴백 (도형 폴백으로 bake 성공)
- [x] 빌드 0 에러 / 크래시 없음
