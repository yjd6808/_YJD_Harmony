# 2026-08-02 WindowStyle(타이틀바) + SVG 아이콘 테마 베이크 — 완료

## 결과 요약
Window에 WPF `WindowStyle` 통합(Title 모드: 타이틀바 + min/max/close 캡션 버튼 + 상태 머신)과 테마 bake에 SVG 아이콘 3종(WindowIconMinimize/Maximize/Close) 추가를 완료하고, 데모(UI_TitleDemo)로 전이 동작을 런타임 검증했다.

## 완료 항목
1. **WindowStyle 통합** (Window.h/.cpp)
   - `enum class WindowStyle { None, Title }`, `SetWindowStyle/GetWindowStyle`
   - 타이틀바 크롬: `pChrome_`(Border) → `pLayoutGrid_`(2행 Auto/Star) → `pTitleBar_`(Border) → `pTitleBarGrid_`(3열 Auto/Star/Auto)
   - 캡션 버튼 3개(28x28): `BuildCaptionButton(UIAssetSemantic, name, fallbackText)` — 아이콘은 mdi SVG 존재 시 `BrushVisual(ThemeBrush(icon semantic, Button, Icon, fixed=20))` z=1, 없으면 텍스트 폴백
   - 상태 머신 `WindowState{Normal,Minimized,Maximized}` + `savedRect_`(y-down) 복원, `Minimize/Maximize/Restore/ToggleMaximize/IsMinimized/IsMaximized`
   - 타이틀바 드래그 (ueMouseDown/Move/Up + CaptureMouse, 버튼 클릭 제외)
   - 동적 조절: `SetTitleBarHeight/Background/Foreground`, `SetWindowBackground/BorderBrush`, `RefreshThemeVisuals()` (스킴 톤 갱신, custom*_ 플래그로 명시색 유지)
   - `virtual UIElement* GetContentRoot()` — None=this, Title=콘텐츠 호스트
2. **UIXmlLoader**: ParseWindowStyle("Title"/"TitleWindow"), Window 전용 속성(WindowStyle/Title/TitleBarHeight/TitleBarBackground/TitleBarForeground/WindowBackground/WindowBorderBrush), 자식 부착을 GetContentRoot()로 라우팅
3. **아이콘 bake**: `UIAssetSemantic` 3종 추가, `UIAssetRecipe.svgPath`, `WindowIconRecipe`(24px), `RenderSvgIcon`(File::Exist 가드 → ReadAllText → nsvgParse → nsvgRasterize supersample → 틴트 semanticText), `SelectRecipe` 케이스 + `SetIconDirectory("...gfx/mdi")`, `BuildThemeVariants` 아이콘 3종
4. **nanosvg** ThirdParty 추가 (vcxproj IncludePath 8곳)
5. **데모**: UI_TitleDemo + ui_title_demo.xml + vcxproj 등록, Scene_Login 임시 Show(원복 완료)

## 수정 중 해결한 이슈
- 빌드 에러 3건: Window.h 삼항 타입 불일치(C2446, static_cast), UI_TitleDemo.cpp `GameCoreHeader.h`/`USING_NS_JC` 누락, `SetIconDirectory(jc::Path::Combine(...))` `.Source()` 필요
- **런타임 크래시**: `jc::File::ReadAllText`가 파일 없을 때 `RuntimeException` throw(File.cpp:217) → 아이콘 bake에서 미처리 예외로 초기화 실패. RenderSvgIcon에 `jc::File::Exist` 가드 추가로 해결 (파일 없으면 도형 폴백)
- `ButtonBase::SimulateClick()` 추가 (데모 클릭 시뮬레이션용)

## 검증 (런타임 로그)
- 아이콘 3종 bake 성공 (SVG 파일 없음 → `[UITextureBaker] SVG 파일이 존재하지 않습니다` + 도형 폴백, entries=20)
- `[Window] ui_title_demo title bar chrome built` / `[UI_TitleDemo] OnInit(XML) chrome=1 size=480x320` / caption buttons 3개
- 클릭 시뮬레이션 전이 로그: `minimized (height=34)` → `restored (pos=(400,200) size=(480x320))` → `maximized` → `RemoveWindow` → `~Window rc=0 running=0` (누수 없음)
- 빌드 0 에러 0 경고, 크래시 없음

## 후속 작업 (미검증 항목)
- 테마 갱신(스킴 전환) 시 아이콘 재베이크 + 타이틀바 색상 갱신 동적 검증 (RefreshThemeVisuals 경로는 구현됨)
- mdi `window-{minimize,maximize,close}.svg` 실제 파일 제공 시 SVG 렌더링 경로(티트/래스터) 검증

## 관련 파일
- `Projects\sgcl\Classes\sgcl\Game\UI\Controls\Window.h/.cpp`
- `Projects\sgcl\Classes\sgcl\Game\UI\Xml\UIXmlLoader.cpp`
- `Projects\sgcl\Classes\sgcl\Game\UI\Theme\UIThemeTypes.h`, `UITextureBakeService.cpp`, `UIThemeManager.cpp`
- `Projects\sgcl\Classes\sgcl\Game\UI\Theme\Baker\UITextureBaker.cpp`, `UIControlBakeRecipes.h/.cpp`
- `Projects\sgcl\Classes\sgcl\Game\UI\Controls\Button.h` (SimulateClick)
- `Projects\sgcl\Classes\sgcl\Game\UI_Implementation\UI_TitleDemo.h/.cpp`
- `UltimateHarmony_Dev\assets\res_data\layout\ui_title_demo.xml`
- `Projects\sgcl\Classes\sgcl\Scene\Scene_Login.cpp` (원복)
- `ThirdParty\nanosvg\nanosvg.h`, `nanosvgrast.h`
