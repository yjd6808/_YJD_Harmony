# 2026-08-02 WPF Solid 테마 체계 적용 (wpf_solid_theme_update.zip) — 진행

## 작업 개요
`D:\Programming\wpf_solid_theme_update.zip`(src/sgcl/Game/UI 27개)과 `D:\Programming\WPF_Theme_Color_System_Final.md`의 WPF 단색(Solid) 색상 체계를 UltimateHarmony sgcl에 적용하고, 빌드 + Log 기반 테스트로 검증하는 작업.

## 진행 상태
- [x] ZIP 추출 → `Projects\sgcl\Classes\sgcl\Game\UI` 덮어쓰기 (27개 소스)
- [x] 신규 헤더 2개 vcxproj 등록 (`Theme\UIThemeColor.h`, `Theme\UIThemeColorTable.h`)
- [x] 빌드 오류 수정 (`Window.cpp` 생성자 `SetThemeControl` 제거 — MD 결함 #2)
- [x] 빌드 성공 (오류 0, 경고 4: C4821/C4996×2/C4005 — 기존)
- [x] 크래시 원인 규명 및 해결 (아래 이슈 참고)
- [x] 테마 초기화 복원 (`SteinsGateApp.cpp` 151-152행)
- [x] SVG 아이콘 3종 확보 (mdi/window-*.svg)
- [x] SVG 없음 폴백 + WARN 로그 추가
- [x] Log 기반 런타임 검증 (크래시 없음, entries=80, ThemeStatus Dark)

## 주요 변경
1. **UIThemeColor.h / UIThemeColorTable.h** (신규) — WPF Solid 색상 체계의 테마 컬러 매핑 테이블. 79개 테마 JSON(`UltimateHarmony_Dev\assets\res_data\theme`)을 `UIThemeColorTable`로 파생.
2. **UIThemeManager::Initialize → BakeDefaultTextureSet** — `variants=80`(20 semantics × 4 states) 베이크.
3. **UIThemeBinding** — semantic 9/13 등 텍스처 적용 경로 (setSpriteFrame 로그 확인).
4. **Window.cpp 캡션 버튼** — 아이콘 SVG 존재 시 BrushVisual 표시, 없으면 WARN + 텍스트 폴백.

## 해결한 이슈
### 1) 컴파일 오류: `SetThemeControl` 미존재 (C3861)
- ZIP의 `Window.cpp` 생성자에 `SetThemeControl(UIThemeControl::Window)` 존재했으나 Window는 Canvas 계열이라 해당 메서드 없음. **MD 문서 결함 #2와 동일** → 생성자에서 제거.

### 2) 런타임 크래시 0xC0000005 (Scene_World.cpp:108) — 기존 문제 규명
- 스택: `WorldScene::InitEventListeners` → `std::function::operator= (functional:1170/_Tidy:1009)` — EventListenerKeyboard의 onKeyPressed 할당 지점.
- 이진 탐색 결과 **테마 변경과 무관한 HEAD 기존 문제로 확정**:
  - UIThemeManager 초기화 제거 → 동일 크래시
  - git stash(UI 폴더만 HEAD로) → 동일 크래시
- **최종 원인: Debug sgcl.exe + Release libcocos2d.dll 혼합 배포**
  - `release\sgcl\libcocos2d.dll`이 14,865,920 bytes(Release)였고, 우리가 빌드한 Debug exe와 std::function 레이아웃이 달라 크래시.
  - PostBuild `xcopy /D`가 날짜 비교 때문에 Debug DLL(8/1 05:26)을 덮어쓰지 못한 것으로 추정.
  - **해결**: `ThirdParty\Cocos2d-x\Output\x64\Debug\libcocos2d.dll`(24,905,216 bytes)을 배포 폴더에 수동 복사 → 크래시 사라짐.
- 참고: cdb/windbg는 이 머신에서 0xC0000142로 실행 불가, gflags는 관리자 필요 → 미니덤프 분석 불가했음.

### 3) SVG 아이콘 파일 없음 → 폴백 + WARN
- mdi에 `window-{minimize,maximize,close}.svg` 부재 → 베이크 중 Error 로그.
- **파일 확보**: `E:\Executable Files\Shotcut\share\glaxnimate\glaxnimate\icons\icons\actions\22\` 에서 복사
  - `window-close.svg`는 실체 파일, `window-minimize/maximize.svg`는 각각 `go-down.svg`/`go-up.svg`의 심볼릭 링크 → 실제 내용으로 복사함.
- **코드 보강 (UITextureBaker.cpp)**:
  - 파일 없음/읽기 실패 시 느낌표 인라인 SVG 문자열(`kFallbackExclamationSvg`)을 동적 생성해 렌더링 (색은 semanticText 틴트).
  - `SemanticName()` 헬퍼 추가 — WARN 로그에 semantic 이름 포함:
    `[UITextureBaker] SVG 아이콘 파일 없음(semantic=WindowIconClose) - 느낌표 폴백 사용: <경로>`
  - Bake 폴백 로그에도 semantic 이름 추가.
- **Window.cpp**: 캡션 버튼 아이콘 파일 없을 때 WARN — `window=%s, button=%s, file=%s`.

## 검증 (Log 기반)
- `[BakeDefaultTextureSet] textureSet=... entries=80 generation=1` — 테마 79개 + 기본 = 80 variants 정상.
- `[UITextureBaker] SVG read/parsed/rasterized` — window-*.svg 3종 정상 로드 (22x22, scale=4.36, 96x96 래스터).
- `[UI_ThemeTest] ThemeStatus: (default) (Dark)` — UI_ThemeTest 윈도우 정상, Title 바 크롬 빌드.
- 크래시 0건, `[ Error ]` 로그 0건.
- 잔여 WARN 5건은 기존 환경 문제 (runtime_config.json 언어 옵션, map/폴더 경로) — 이번 작업과 무관.

## 환경 메모
- `Scripts\BuildProject\sgcl.bat` 경로 버그: `%~dp0..\..\Scripts\Build.ps1`이 `UltimateHarmony\..\..\Scripts\Build.ps1`로 확장됨 → 빌드는 저장소 루트에서 `Build.ps1 -ProjectName sgcl -Configuration Debug -Platform x64` 직접 호출.
- **배포 폴더 Debug/Release DLL 불일치 주의**: exe 구성에 맞는 `libcocos2d.dll`을 `release\sgcl\`에 유지할 것.
- 테마 JSON 79개는 사용자가 `UltimateHarmony_Dev\assets\res_data\theme`에 배치 (본 작업 파일 아님).

## 후속 작업
- [ ] UI_ThemeTest 화면에서 색상/스킴 전환(라이트) 시 RefreshThemeVisuals 경로 런타임 검증
- [ ] 79개 테마 JSON의 색상 파생 결과(UIThemeColorTable) 시각 확인
- [ ] 느낌표 폴백 SVG가 실제로 표시되는지 (파일 없는 상태에서) 육안 확인
