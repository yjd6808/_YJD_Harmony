# UltimateHarmony dev-ui-refactoring 진행 상황 (progress)

> 이 문서는 작업을 phase별로 추적하기 위한 진행 로그입니다.
> 각 phase 완료 시 체크박스를 갱신하고, "다음 phase"를 참고해 이어서 진행합니다.

## 브랜치 / 기준
- 브랜치: `uh-dev-ui-refactoring`
- 기준 커밋: `16837196` (fable 베이스)
- 작업 루트: `C:\Users\jdyun\Desktop\uh-dev-ui-refactoring`
- dev 에셋: `C:\Users\jdyun\Desktop\UltimateHarmony_Dev\assets`
- 계획 문서: `.admin/plans/2026-08-02-apply-fable-sgui.md`
- 최종 완료 문서: `.admin/histories/2026-08-02-ui-findelementbyname-restore.md`

---

## Phase 구성 및 상태

### Phase 0 — 기반 작업 (완료)
- [x] fable 소스 오버레이 (추가 57 / 변경 26 / 삭제 34, MD5 검증)
- [x] vcxproj 동기화 (제거 파일 반영, GameScene/Image 제외)
- [x] tinyxml2 v9 헤더 우선 포함 + `Project/tinyxml2.cpp` 편입 제거 (심볼 중복 해결)
- [x] sgcl.vcxproj ClInclude .cpp 중복 167개 제거

### Phase 1 — sgui(WPF) 코어 + UIManager (완료)
- [x] `UI/Host/UIManager.h/.cpp`: 팩토리 Show 우선 → XML Show 폴백, ShowWindow 생명주기, g_cUIMgr 매크로
- [x] `UI/Host/UIHost.h/.cpp`: Window 컨테이너, PollThemeRevision, OnUpdate
- [x] `UI/Host/WindowRegister.h`: REGISTER_WINDOW 팩토리
- [x] `UI/Core`, `UI/Controls`, `UI/Panels`: sgui 요소/컨트롤/패널 (Window/Grid/StackPanel/Canvas/DockPanel/Border/TextBlock/Button/CheckBox/ToggleButton/ProgressBar/ScrollBar/Slider/TextBox/Image)
- [x] `Window::SetXmlLoaded/IsXmlLoaded/bXmlLoaded_`
- [x] fable 중복 이관 + ViewManager 제거 (g_cViewMgr → g_cUIMgr 전환 완료 — grep으로 g_cViewMgr 잔존 없음 확인)
- [x] SGA(UI 전용) 제거 (ImageBrush/NinePatchBrush/ResourceTable/UIResourceIndex/CreateUITexture 제거 — grep으로 잔존 없음 확인, Brush.h는 SolidColor/LinearGradient/ThemeBrush만)

### Phase 2 — XML 바인딩/리졸브 엔진 (완료)
- [x] `UI/Xml/UIValue.h/.cpp`: UIValue(variant) + UIDataList + ParseDataList (0b/0x/10진, 멀티라인 <Value>, Array/Map, 순서 보존)
- [x] `UI/Xml/UIResolve.h/.cpp`: 표현식 파서(재귀하강) — `{...}`/백틱/`@`데이터/`$`상대/요소 참조, +-*/%() 비교 논리 max/min, 문자열 보간 폴백, 타입 캐스팅, 중첩 접근 `{@Items[{@Index}]}`, 요소 프로퍼티 읽기(Name/Width/Height/Margin/Text/IsChecked/Percent/Value 등)
- [x] `UI/Xml/UIXmlLoader.h/.cpp`: WPF XML→sgui 2패스 빌드 (1패스 정적 속성, 2패스 `{}`/백틱 바인딩 지연 적용), DataList 파싱, LoadWindowFromFile/Into/IntoString
- [x] dev layout XML 5종 WPF 변환: ui_generic / ui_login / ui_channel / ui_inventory / ui_test

### Phase 3 — UI_* FindElementByName 복원 (완료)
- [x] `UIManager::Show()` XML 우선 + 코드 빌드 폴백
- [x] UI_Generic: OnInit XML 분기 (FindElementByName + 이벤트 연결) + 코드 폴백
- [x] UI_Login: OnInit XML 분기 + 코드 폴백, SetTab nullptr 안전
- [x] UI_ChannelSelect: InitFromXml/BuildCodeFallback 분기, BuildServerButtons/BuildChannelButtons 헬퍼
- [x] UI_Inventory: XML 분기 + BuildEquipSlots/BuildInvenSlots 헬퍼, pInvenSlotGrid_ → Panel* 완화
- [x] UI_Test: OnInit XML 분기 + XML 버튼 콜백 연결
- [x] 빌드 검증: sgcl Debug x64 0 에러
- [x] 런타임 검증: `[UI_Generic] OnInit(XML) ...` 로그 (XML 병합 + FindElementByName 동작 확인)

### Phase 4 — 테마 융화 검토 (완료)
- [x] Brush.h에서 ImageBrush/NinePatchBrush 제거 → ThemeBrush/SolidColorBrush/LinearGradientBrush 유지
- [x] BrushVisual::ApplyThemeBinding (ThemeBrush → 텍스처 세마틱 바인딩)
- [x] UIThemeManager::ApplyTheme + UIHost::PollThemeRevision 동작 확인
- [x] UI_Generic::ApplyThemeFile / UIColorScheme(Dark/Light/System) 실시간 전환 확인

### Phase 5 — 최종 검증 (완료)
- [x] 최종 빌드: MSBuild sgcl Debug x64
- [x] 런타임: start_up_debugging.ps1 실행 → XML 병합 로그 확인
- [x] XML 바인딩(`{}`/백틱) dev XML 사용 확인 → **사용 예 없음** (정적 속성만 사용) → 로더만 검증(성공 로드 + OnInit 동작)
- [x] **종료 시점 크래시 수정** — 윈도우 이중 autorelease(`ShowWindow`의 `autorelease()` 중복) 제거로 `Node::~Node` assert 해결
  - 재현: `[Window] ~Window name=ui_generic rc=0 running=1` (Clear/RemoveWindow 없이 소멸) / 스택: `AutoreleasePool::clear → Ref::release → ~UI_Generic → Node::~Node:191`
  - 검증: 빌드 0 에러 + `start_up_debugging.ps1` 연속 2회 정상 (크래시 덤프 미생성)
- [x] plan 체크리스트 전 항목 [x] 갱신
- [x] done/history 문서 최종 갱신

### Phase 6 — 마무리 (완료)
- [x] progress.md 최종 상태 기록
- [ ] (필요시) 사용자 확인 후 커밋

### Phase 7 — C++ 코딩 컨벤션 준수 (완료) [plan: 2026-08-02-cpp-convention-braces.md]
- [x] **중괄호 규칙**: `return`/`continue`/`break` 제외 모든 단일 제어문 본문에 중괄호 추가
  - 다중 줄(if/for/while/else/else-if 다음 줄 본문) + 동일 줄(`if (x) stmt;`) 모두 처리 → 위반 0건
  - 스캔 정규식 v1 누락(`else if`) → v2로 재적용. 중첩 루프(for→for→if)는 다중 pass로 처리
  - 대상: `Game/UI/**` + `Game/UI_Implementation/**` 29파일, 백업: `%TEMP%\opencode\ui_backup_braces`·`ui_backup_after_braces`
- [x] **가상 함수 규칙**: override 함수 `virtual` 누락 105건/26파일 추가 (사용자 확인)
- [x] 빌드 0 에러 + 런타임 정상(`OnInit(XML)`, AddWindow, 크래시 없음)
- [ ] **명명 규칙(금지 접두사 dw/ui/i/v)**: 약 1966건 추정 + for(int i) 50건 — 대규모라 별도 작업으로 보고. 진행 시 사용자 확인 필요

### Phase 8 — Grid ColumnDefinitions XML + 채널 선택 XML 전환 (완료) [plan: 2026-08-02-grid-columndefinitions-and-channelselect-xml.md]
- [x] UIXmlLoader: `Grid.ColumnDefinitions`/`Grid.RowDefinitions` 자식 요소 파싱 추가 (ColumnDefinition Width/MinWidth/MaxWidth, RowDefinition Height/MinHeight/MaxHeight)
- [x] `BuildElement` 자식 순회: Grid 타입일 때 정의 컬렉션은 부착 대신 Grid 등록 분기
- [x] 기존 `Columns`/`Rows` 속성 문법 유지 (하위 호환)
- [x] ui_channel.xml: `Rows`/`Columns` 속성 → `<Grid.RowDefinitions>`/`<Grid.ColumnDefinitions>` 문법으로 재작성 (dev 배포 폴더)
- [x] UI_ChannelSelect: `BuildCodeFallback` 제거 (코드 잔여 0건), `BuildChannelButtons`를 Grid + `SetGridColumn`/`SetGridRow` 부착 배치로 전환
- [x] 빌드 0 에러
- [x] 런타임 검증: `[UI_ChannelSelect] channel_list Grid col=2 row=5 children=10` + `OnInit(XML)` + `AddWindow ui_channel rc=2 running=1` (크래시 없음)

### Phase 9 — XML 중괄호 파싱 규칙 변경 + dev XML 5종 전환 (완료) [plan: 2026-08-02-xml-brace-hash-element-ref.md]
- [x] UIResolve.cpp: `ParsePathBase`에 `#` 케이스 추가(`#이름` → FindElementByName), bare-ident 요소 조회 제거 (키워드 외 Fail)
- [x] UIResolve.cpp: `ResolveAttribute` — 단일 `{...}`(닫힘이 끝)이면 내부 전체 `ResolveExpression`, 실패 시 보간 폴백 → **외부 수식(`{a} + 30`) 제거**
- [x] UIResolve.h 문법 주석 갱신
- [x] dev XML 5종 새 문법 전환 (ui_generic/ui_login/ui_channel/ui_inventory/ui_test): DataList + `{@key}`/`{#요소.프로퍼티}`/중괄호 표현식/보간, Name/구조 유지
- [x] ui_test.xml 표현식 테스트 24개 + UI_Test.cpp 검증 로그 (logExprText), UI_Generic.cpp 크기 검증 로그 추가
- [x] 빌드 0 에러 / 런타임 크래시 없음
- [x] **런타임 검증 완료** — 표현식 24개 전부 기대값 일치 (elem=160 `#`참조, elemExpr=200 중괄호 내부 수식, chain=220 체인, root=640, interp=`HP: 87 (99Lv)`, logic=true, cmp=false, max/min, nestedIdx 중첩 인덱스, empty=빈값 bare 제거) / ui_generic 크기 체인 정확 (60→120→200, 120=60*2, 200=120*2-40) / ui_channel·ui_login·ui_inventory OnInit(XML) 정상 / ui_inventory `equip` nullptr은 기존 코드 특성(멤버 미할당, 사용처 없음)
- [x] Scene_Login.cpp 임시 Show 4종 추가 후 원복 (최종: `Show("ui_channel")`만)
- [x] plan 체크리스트 + done/history 문서 작성 (2026-08-02-xml-brace-hash-element-ref.md)

---

## 다음 phase (Phase 5)
1. 최종 빌드 (MSBuild sgcl Debug x64) — 0 에러 확인
2. start_up_debugging 실행 → `[UI_Generic] OnInit(XML)` 로그 재확인
3. dev XML에 `{}`/백틱 바인딩 사용 여부 검색 → 바인딩 실동작 검증
4. plan 체크리스트 [x] 전체 갱신
5. done 문서 최종 갱신

## 주의/유의사항
- `ui_popup`은 PopupManager가 직접 생성(REGISTER_WINDOW 미사용) → XML 병합 대상 아님
- `ui_hud/ui_chat/ui_chat_input/ui_select_character/ui_select_charactor`는 등록 팩토리 없음 → Show되지 않으므로 임의 변환 금지 (필요 시 별도 작업)
- 빌드는 반드시 x64 (Debug/Release)
- 실행 인자는 단일 따옴표 문자열: `"assets=... mode=1 auth_ep=127.0.0.1:10110"`
