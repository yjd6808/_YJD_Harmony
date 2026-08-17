# 2026-08-02 C++ 코딩 컨벤션 준수 (중괄호 + 가상 함수) — sgui(UI) 코드

## 작업 요약
`.opencode/rules/coding-conventions_cpp.md`의 포매팅/가상 함수 규칙을 이번 리팩토링으로 작성/수정한 sgui(UI) 코드에 적용했다.
사용자 확정 범위: **UI(sgui) 코드만** (`Game/UI/**` + `Game/UI_Implementation/**`). 기존 게임 코드는 수정하지 않음.

## 변경 사항

### 1. 중괄호 규칙 (핵심 — 사용자 강조)
> `return`/`continue`/`break` 단일 제어문은 중괄호 생략 가능, **그 외 로직은 반드시 중괄호 내에 작성**

- 다중 줄 본문: `if (cond) [다음 줄] stmt;` → 중괄호 추가
- 동일 줄 본문: `if (pBtn) pBtn->Refresh();` → 다중 줄 + 중괄호로 분리
- 중첩 루프(for→for→if)는 다중 pass로 안쪽까지 모두 중괄호 처리
- `else`/`else if` 패턴 포함 (정규식 v1에서 `else if` 누락 발견 → v2로 재적용)
- 결과: 위반 **96건 → 0건** (검증 스캔 0건)

### 2. 가상 함수 규칙
> 오버라이드 함수는 반드시 `virtual` + `override` 모두 명시

- `void Foo() override;` → `virtual void Foo() override;` (const override 포함)
- **105건 / 26파일** 수정 (사용자 확인)

### 3. 명명 규칙 (보고만)
- 금지 접두사(`dw`/`ui`/`i`/`v`) 약 1966건 추정, `for (int i ...)` 50건 — 대규모라 **별도 작업으로 남김**
- 진행 시 5파일 초과 확인 필요

## 대상 파일
`Game/UI/Core`·`Controls`·`Panels`·`Host`·`Xml`·`Theme`(+`Baker`)·`UI_Implementation` 전체 (29파일 중괄호 / 26파일 virtual)

## 검증
1. **빌드**: `Build.ps1 -ProjectName sgcl -Configuration Debug -Platform x64` — **0 에러** (경고 4건은 기존 경고: BOM/deprecated/macro)
2. **런타임**: `start_up_debugging.ps1` → `[UI_Generic] OnInit(XML)`, `[UIHost] AddWindow rc=2`, `-- 씬전환 완료`, **크래시 덤프 미생성**

## 백업
- `%TEMP%\opencode\ui_backup_braces` (중괄호 수정 전)
- `%TEMP%\opencode\ui_backup_after_braces` (virtual 추가 전)

## 계획 문서
- `.admin/plans/2026-08-02-cpp-convention-braces.md`
