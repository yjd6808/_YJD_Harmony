# History: XML 명시적 ContentHost 패널 도입

- 날짜: 2026-08-02
- 작업명: ContentHost
- 관련 계획: .admin/plans/2026-08-02-ContentHost.md

## 배경

Window 콘텐츠 호스트가 코드에서 `Grid::Create()`로 하드코딩됨 (Window.cpp:320). 데모 XML들이 `Canvas.Left/Top` 절대좌표를 Window 직속 자식에 사용했는데 Grid 호스트가 이를 무시해 모든 요소가 (0, 252~266)에 겹침. WPF 원칙: Canvas.Left/Top은 직속 부모가 Canvas일 때만 동작.

## 완료 내용

### 코드 (sgcl)

- **UIElement.h**: `bool isContentHost_` + `SetContentHost(bool)` / `IsContentHost()` 추가
- **UIXmlLoader.cpp**:
  - `ApplyAttribute`: `ContentHost` 속성 파싱
  - `PopulateWindowFromDocument`: 자식 수집 → ContentHost 검증 (0개 → Error "선언되지 않았습니다", 2개+ → Error "1개만 허용됩니다", 창 이름 포함) → `SetContentHost(host)` → 나머지 `AttachChild` → `ValidateNoNestedContentHost` 재귀 검증 (중첩 발견 시 Error)
- **Window.h**: `pContentHost_` 타입 `UIElement*`로 변경, `SetContentHost(UIElement*)`, `ArrangeOverride` 선언, `GetContentRoot()` 단순화 (`pContentHost_ ? pContentHost_ : this`)
- **Window.cpp**:
  - 디폴트 Grid 생성 로직 제거 (BuildTitleBarChrome에서 삭제)
  - `SetContentHost`: Title → `SetGridRow(1)` + `pLayoutGrid_->AddChild`, None → `AddChild`
  - `ArrangeOverride`: None 스타일에서 호스트가 창 전체 영역을 채우고 나머지는 Canvas 규칙 배치

### XML (6개, UltimateHarmony_Dev\assets\res_data\layout\)

| 파일 | 수정 |
|---|---|
| ui_title_demo.xml | 직속 자식 `<Canvas ContentHost="true">` 래핑 |
| ui_generic.xml | 직속 자식 `<Canvas ContentHost="true">` 래핑 |
| ui_test.xml | `<Grid ContentHost="true">` 래핑 + Canvas 좌표 4개(pbar_percent/chk_bind/chk_bool/expr_visible)를 Margin으로 변환 |
| ui_channel.xml | 기존 외부 `<Grid>`에 `ContentHost="true"` 추가 |
| ui_login.xml | `<Grid ContentHost="true">` 래핑 |
| ui_inventory.xml | `<Grid ContentHost="true">` 래핑 |

### 엔진 (Cocos2d-x)

- **계측 제거**: CCNode.cpp `insertChild`/`sortAllChildren`의 nodedbg.log 기록 코드 + `<cstdarg>/<cstdio>/CCConsole.h` include 제거. 엔진 Debug 재빌드 + DLL 재배포. nodedbg.log 미생성 확인.
- **유지**: ccMacros.h CC_LITTLE_ENDIAN x64 수정 (영구 수정, z-order 정렬)

## 검증

- sgcl Debug/x64 빌드 성공, 실행 로그(2026-08-02-153640.log)에서 ContentHost/UIXmlLoader Error 0건
- ui_title_demo 덤프 환산 (Cocos bottom-up → top-down): lbl_desc=(16,16), btn_minimize=(16,120), btn_restore=(140,120), btn_toggle_max=(264,120), btn_close=(16,166), lbl_state=(16,220) — XML 좌표와 모두 일치
- 콘텐츠 호스트 478x286 (타이틀바 아래) 정상 배치, 겹침 해소
- ui_generic/test/channel/login/inventory는 서버 연동 흐름에서만 로드되어 이번 실행에선 확인 못함 (XML 검증은 통과)

## 남은 잡음 (무관)

- UITextureBaker SVG 3건 Error: `assets/res_data/gfx/mdi/window-minimize.svg` 등 아이콘 파일 누락

## 참고 (다음 세션용)

- 빌드: `Scripts\Build.ps1 -ProjectName sgcl -Configuration Debug -Platform x64 -FileLog -NoConsoleLogging` — **반드시 `C:\Users\jdyun\Desktop\uh-dev-ui-refactoring` 디렉토리에서 실행** (git rev-parse로 SolutionDir 탐색, 다른 cwd에선 EXIT=1)
- 엔진 빌드: MSBuild 직접 호출 (Build-Cocos.bat은 cmd에서 where.exe 미설정으로 실패)
- 실행: release\sgcl에서 `"assets=... mode=1 auth_ep=127.0.0.1:10110"` 단일 따옴표 인자, 14초 후 종료
- 덤프 좌표는 Cocos bottom-up 노드 좌표 — top-down 환산: `y_topdown = parentHeight - nodeY - height`
