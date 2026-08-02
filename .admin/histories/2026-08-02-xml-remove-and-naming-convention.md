# 2026-08-02 레거시 XML 제거 + UI 명명 규칙 수정 (완료)

## 목표
1. **A: SGA 시대 레거시 layout XML 6종 제거** (사용자 확정: 삭제)
   - `ui_hud.xml` / `ui_chat.xml` / `ui_chat_input.xml` / `ui_popup.xml` / `ui_select_character.xml` / `ui_select_charactor.xml`
2. **B: UI(sgui) 코드 명명 규칙 수정** (사용자 확정: UI 코드만 / 명확한 헝가리안만)
   - 위반 50건 전부 `for (int i = ...)` → `for (int idx = ...)` rename

## 변경 내용

### A. 레거시 XML 6종 삭제 (배포 폴더)
- 경로: `C:\Users\jdyun\Desktop\UltimateHarmony_Dev\assets\res_data\layout\`
- 삭제: `ui_hud.xml` `ui_chat.xml` `ui_chat_input.xml` `ui_popup.xml` `ui_select_character.xml` `ui_select_charactor.xml`
- 근거: 전체 코드 grep 결과 **소비처 0건** (데드 파일). `ui_popup`은 코드 기반 `UI_Popup`(PopupManager 직접 생성)으로 대체됨. SGA 속성은 시스템에서 이미 제거됨
- 잔여: 신형 5종 `ui_channel/ui_generic/ui_inventory/ui_login/ui_test.xml` 유지

### B. UI 코드 루프 카운터 rename (`i` → `idx`)
- 대상: 50건 / 18파일
  - UI\Core\UIElement.cpp(7), UI\Controls\Border.cpp(1), UI\Core\InputDispatcher.cpp(1), UI\Host\UIHost.cpp(1), UI\Panels\Grid.cpp(4), UI\Theme\Baker\UIGaussianBlur.cpp(2), UI\Theme\Baker\UIMaskBuffer.cpp(3), UI\Theme\UITextureBakeService.cpp(3), UI\Theme\UITextureCache.cpp(5), UI\Theme\UITextureEntry.cpp(3), UI\Theme\UIThemeBinding.cpp(1), UI\Theme\UIThemePropertyDescriptor.cpp(1), UI\Xml\UIResolve.cpp(4), UI\Xml\UIXmlLoader.cpp(1), UI_Implementation\UI_ChannelSelect.cpp(8), UI_Implementation\UI_Generic.cpp(1), UI_Implementation\UI_Inventory.cpp(2), UI_Implementation\UI_Test.cpp(2)
- 스크립트: `%TEMP%\opencode\rename_loop_i_to_idx.ps1` (brace-aware, 중첩 for 대응, Start 내림차순 멱등 처리, 작은 범위 순환식 포함)
- 대상 이름 `idx` 선정 사유: 기존 사용 0건. `index`는 UIResolve.cpp/UIValue.cpp/UI_Generic.cpp에서 이미 사용 중이라 배제
- 유지 사항: `for (size_t i ...)` (규칙상 대상 아님), `vAlignment_`/`vSpacing_`(vertical 약어), `dwThreadId`(WinAPI), `uiResolutionWidth_`(cocos pInfo)

## 발생 이슈 및 해결
- **빌드 오류**: rename 스크립트가 Grid.cpp:121 `rowSizes[i - 1]`(for `idx` 루프 본문) 미처리 → `error C2065: 'i': undeclared identifier` / `C2676`
  - 원인: 119번 루프 decl 위치가 이전 for end보다 뒤라 해당 for 헤더의 `\bi\b`만 rename하고 몸통은 건너뜀
  - 해결: 수동 수정 `rowSizes[i - 1]` → `rowSizes[idx - 1]` (Grid.cpp:121)

## 검증 결과
- [x] A: 레거시 XML 6종 부재, 신형 5종만 잔여, 코드 내 소비처 0건
- [x] B: UI 코드 `for (int i/v =` 잔여 **0건**, `for (int idx =` **50건** 확인
- [x] 빌드: `Build.ps1 -ProjectName sgcl -Configuration Debug -Platform x64` — **경고 0 / 오류 0**
- [x] 런타임: `start_up_debugging.ps1 -ProjectName sgcl` — `[UI_Generic] OnInit(XML)` / `[UIHost] AddWindow ui_generic rc=2 running=1` / `-- 씬전환 완료`, 신규 크래시 덤프 없음

## 추가 판단 (사용자 확정)
- 스캔 중 발견된 범위 밖 비루프 로컬 변수 3건(`UIPixelBuffer.cpp:31` `int i`, `UIStyleOverride.cpp:16` `uint64_t v`, `UIResolve.cpp:41` `PVal v`)은 **수정하지 않기로 확정** (사용자 지시) — 원본 유지
- (시도 후 원상 복구 확인: UIPixelBuffer.cpp:31-40 원문 유지)
