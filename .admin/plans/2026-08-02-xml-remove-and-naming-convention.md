# 2026-08-02 레거시 XML 제거 + UI 명명 규칙 수정

## 목표
1. **A: SGA 시대 레거시 layout XML 6종 제거** (사용자 확정: 삭제)
   - `ui_hud.xml` / `ui_chat.xml` / `ui_chat_input.xml` / `ui_popup.xml` / `ui_select_character.xml` / `ui_select_charactor.xml`
2. **B: UI(sgui) 코드 명명 규칙 수정** (사용자 확정: UI 코드만 / 명확한 헝가리안만)
   - 위반 50건 전부 `for (int i = ...)` 루프 카운터 → `for (int idx = ...)` 로 rename
   - 대상 이름은 `idx` (기존 사용 0건으로 충돌 없음. `index`는 UIResolve/UIValue/UI_Generic에서 이미 사용 중이라 제외)

## 분석 결과
- 6개 XML 모두 **코드 소비처 없음** 확인 (grep 전체 코드 — 0건)
  - `ui_popup`은 코드 기반 `UI_Popup`(PopupManager가 직접 생성)으로 이미 대체됨
  - 나머지(HUD/Chat/캐릭터선택)는 미구현 기능
  - SGA 속성(`sga`/`img`/`sprite`/`index`)은 시스템에서 이미 제거됨 → 로드 불가능한 데드 파일
- 명명 규칙 정밀 스캔 (UI 코드만):
  - `i`+대문자 헝가리안: UI 코드 내 **0건**
  - `for (int i = ...)`: UI 코드 내 **50건 / 18파일**
  - `vAlignment_`/`vSpacing_`(vertical 약어), `dwThreadId`(WinAPI), `uiResolutionWidth_`(cocos pInfo)는 **헝가리안 아님 → 유지**
- 대상 파일 (18개):
  - UI\Core\UIElement.cpp(7), UI\Controls\Border.cpp(1), UI\Core\InputDispatcher.cpp(1), UI\Host\UIHost.cpp(1), UI\Panels\Grid.cpp(4), UI\Theme\Baker\UIGaussianBlur.cpp(2), UI\Theme\Baker\UIMaskBuffer.cpp(3), UI\Theme\UITextureBakeService.cpp(3), UI\Theme\UITextureCache.cpp(5), UI\Theme\UITextureEntry.cpp(3), UI\Theme\UIThemeBinding.cpp(1), UI\Theme\UIThemePropertyDescriptor.cpp(1), UI\Xml\UIResolve.cpp(4), UI\Xml\UIXmlLoader.cpp(1), UI_Implementation\UI_ChannelSelect.cpp(8), UI_Implementation\UI_Generic.cpp(1), UI_Implementation\UI_Inventory.cpp(2), UI_Implementation\UI_Test.cpp(2)

## 작업 절차
1. [x] 분석 완료 (XML 소비처 0건, 명명 위반 인벤토리 확보)
2. [x] plan 파일 작성 (본 문서)
3. [x] A: 배포 폴더에서 레거시 XML 6종 삭제
4. [x] B: 루프 카운터 rename 스크립트 작성·실행 (brace-aware, 중첩 for 대응) → 검증 스캔 0건
   - rename 스크립트가 Grid.cpp:121 `rowSizes[i - 1]`(for `idx` 루프 본문) 누락 → `error C2065/C2676` 발생, 수동 수정 `rowSizes[idx - 1]`로 해결
5. [x] 빌드 (Build.ps1 sgcl Debug x64) 0 에러 (경고 0 / 오류 0)
6. [x] 런타임 검증 (start_up_debugging.ps1, `UI_Generic` OnInit(XML) / `AddWindow ui_generic rc=2` / `-- 씬전환 완료` / 크래시 없음)
7. [x] plan/progress 업데이트 + /done 문서 작성

## 검증 기준
- [x] 레거시 XML 6종이 배포 폴더에서 제거됨 (신형 5종 `ui_channel/ui_generic/ui_inventory/ui_login/ui_test`만 잔여)
- [x] UI 코드 `for (int i ...)` → `for (int idx ...)` 전부 rename, 남은 위반 0건 (`for (int idx` 50건 확인)
- [x] 빌드 0 에러
- [x] 런타임 정상 (ui_generic 로드, 씬전환 완료, 크래시 없음)

## 추가 판단 (사용자 확정)
- 스캔 중 발견된 범위 밖 비루프 로컬 변수 3건(`UIPixelBuffer.cpp:31` `int i`, `UIStyleOverride.cpp:16` `uint64_t v`, `UIResolve.cpp:41` `PVal v`)은 **수정하지 않기로 확정** (사용자 지시) — 모두 원본 유지
