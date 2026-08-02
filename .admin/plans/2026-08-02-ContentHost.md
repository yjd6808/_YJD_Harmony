# Plan: XML 명시적 ContentHost 패널 도입

- 날짜: 2026-08-02
- 작업명: ContentHost
- 상태: 완료 (2026-08-02)

## 배경

- Window 콘텐츠 호스트가 코드에서 `Grid::Create()`로 하드코딩됨 (Window.cpp:320).
- 데모 XML(ui_title_demo/ui_generic/ui_test)은 `Canvas.Left/Top` 절대좌표를 Window 직속 자식에 사용 → Grid 호스트가 좌표를 무시 → 모든 요소가 (0,0) 셀에 겹침.
- WPF 원칙: Canvas.Left/Top은 직속 부모가 Canvas일 때만 동작.

## 요구사항 (사용자 지시)

1. Window에 디폴트 패널 생성 로직 제거 (Window.cpp:320-322 삭제).
2. XML에서 명시적으로 ContentHost 패널을 선언 (`ContentHost="true"` 속성).
3. ContentHost가 0개 또는 2개 이상이면 Error 로그.
4. 사용 중인 XML 6개 모두 수정.

## 설계

### 코드

- **UIElement.h**: `bool isContentHost_` + `SetContentHost(bool)` / `IsContentHost()` (canvasLeft_ 패턴)
- **UIXmlLoader.cpp**:
  - `ApplyAttribute`: `ContentHost` 속성 파싱 → `SetContentHost(ParseBool(_value))`
  - `PopulateWindowFromDocument`: Window 직속 자식을 즉시 부착하지 않고 수집 → ContentHost 검증(0개/2개+ → `_LogError_`) → `pWindow->SetContentHost(host)` → 호스트를 제외한 나머지를 `AttachChild`로 부착
  - 중첩(직속 아님) ContentHost 발견 시에도 Error 로그
- **Window.h/cpp**:
  - `pContentHost_` 타입 `Grid*` → `UIElement*`
  - `SetContentHost(UIElement*)`: Title 스타일이면 `pLayoutGrid_` row 1에 부착, 그 외엔 `AddChild` (창 전체 채움)
  - `GetContentRoot()`: `pContentHost_` 있으면 반환, 없으면 this (Error 폴백)
  - `ArrangeOverride` 오버라이드: None 스타일에서 호스트가 창 전체 영역을 채우도록 (Window : Canvas — Canvas는 desired 크기로만 배치하므로 호스트 전용 처리 필요)
  - 디폴트 Grid 생성 제거 (Window.cpp:320-322)

### XML (6개)

| 파일 | 스타일 | 수정 |
|---|---|---|
| ui_title_demo.xml | Title | 직속 자식을 `<Canvas ContentHost="true">`로 래핑 |
| ui_generic.xml | None | 직속 자식을 `<Canvas ContentHost="true">`로 래핑 |
| ui_test.xml | None | 직속 자식을 `<Grid ContentHost="true">`로 래핑 + Canvas 좌표 4개를 Margin으로 변환 |
| ui_channel.xml | None | 기존 외부 `<Grid>`에 `ContentHost="true"` 추가 |
| ui_login.xml | None | 직속 자식을 `<Grid ContentHost="true">`로 래핑 |
| ui_inventory.xml | None | 직속 자식을 `<Grid ContentHost="true">`로 래핑 |

## 검증

- 빌드 (sgcl Debug/x64) + 실행
- 로그에서 ContentHost Error 부재 확인
- ui_title_demo 덤프: lbl_desc=(16,16), btn_minimize=(16,120), btn_restore=(140,120) 등 XML 좌표와 일치
- 화면 캡처로 겹침 해소 확인
- ui_login/ui_channel 등 None 윈도우 정상 동작 확인

## 검증 결과 (2026-08-02 완료)

- **빌드**: sgcl Debug/x64 성공, ContentHost/UIXmlLoader Error 로그 0건
- **ui_title_demo** (480x320 Title, 실행 로그 2026-08-02-153640.log):
  - `[Window] ui_title_demo content host=` 로그로 호스트 부착 확인
  - 덤프(노드 좌표는 Cocos bottom-up) 환산 결과 모두 XML 좌표와 일치:
    - lbl_desc top-down (16,16) [node pos y=250 = 286-16-20]
    - btn_minimize (16,120), btn_restore (140,120), btn_toggle_max (264,120), btn_close (16,166)
    - lbl_state (16,220)
  - 콘텐츠 호스트가 타이틀바 아래 478x286 영역에 배치됨 (layout grid row 1)
  - 기존 문제였던 (0,252~266) 겹침 완전 해소
- **나머지 5개 XML**: XML 파싱 + ContentHost=1개 검증 통과 (모드=1 데모 실행은 ui_title_demo만 로드; ui_generic/test/channel/login/inventory는 서버 연동 흐름에서만 열림)
- **엔진 계측 롤백**: CCNode.cpp nodedbg 로그 계측 제거 + 엔진 Debug 재빌드 + DLL 재배포 완료. nodedbg.log 미생성 확인. CC_LITTLE_ENDIAN 수정(ccMacros.h)은 유지.
- 남은 잡음: UITextureBaker SVG 3건 Error는 기존 아이콘 파일 누락(window-minimize/maximize/close.svg) — 본 작업과 무관

## 메모

- WindowStyle 속성은 Window 자식 파싱 전에 적용되므로 chrome(pLayoutGrid_)이 ContentHost 부착 시점에 이미 존재 (Title 케이스)
- Bindings 2패스 적용은 자식 수집과 무관하게 유지
- DestroyTitleBarChrome에서 pContentHost_ = nullptr 유지 (기존 동작)
