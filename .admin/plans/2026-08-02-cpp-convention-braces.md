# 2026-08-02 C++ 코딩 컨벤션 준수 (중괄호 규칙) — sgui(UI) 코드

## 목표
`.opencode/rules/coding-conventions_cpp.md` 포매팅 규칙 중 **"`return`/`continue`/`break` 단일 제어문은 중괄호 생략 가능, 그 외 로직은 중괄호 필수"** 를 이번 리팩토링으로 작성/수정한 sgui(UI) 코드에 적용한다.

## 범위 (사용자 확정: UI(sgui) 코드만)
- `Projects/sgcl/Classes/sgcl/Game/UI/**` (sgui 프레임워크 + Theme)
- `Projects/sgcl/Classes/sgcl/Game/UI_Implementation/**`
- 기존 게임 코드(Actor/Action/AI/Contents/Scene/Layer 등)는 수정하지 않음

## 대상 위반 유형
```cpp
// 위반: 본문이 대입/호출 등 일반 로직인데 중괄호 없음
if (cond)
    _out.x = value;
for (auto& it : list)
    Use(it);

// 준수: return/continue/break 단일 제어문은 중괄호 생략 허용
if (cond)
    return;
```

## 체크리스트
- [x] 스캔: UI 코드에서 중괄호 위반 정확한 목록 추출 (96건 다중 줄 + 동일 줄, 이후 else/else-if 포함 정규식 재스캔)
- [x] Game/UI/Core, Controls, Panels, Host, Xml 파일 수정
- [x] Game/UI/Theme (+ Baker) 파일 수정
- [x] Game/UI_Implementation 파일 수정
- [x] 그 외 UI 코드 내 다른 컨벤션(naming/virtual/override/구분자) 위반 점검
  - **가상 함수 규칙**: override 누락 virtual 105건/26파일 수정 (사용자 확인)
  - **명명 규칙**: 금지 접두사 대규모(약 1966건 추정 + for(int i) 50건) → 별도 작업으로 보고
- [x] 빌드 (Build.ps1 sgcl Debug x64) 0 에러
- [x] 런타임 재확인 (start_up_debugging.ps1)
- [x] plan/progress/done 문서 갱신

## 진행 로그
- 2026-08-02: plan 생성. 스캔 결과 172건/45파일 중 UI 코드 범위 약 21파일 91건 확인
- 2026-08-02: 범위 확정 — UI(sgui) 코드만 수정 (사용자 답변)
- 2026-08-02: **중괄호 수정 완료 (96건 → 0건)** — 다중 줄/동일 줄 본문에 중괄호 추가. `else if`/`else` 패턴 정규식 누락 발견 → v2 정규식으로 재적용
- 2026-08-02: **가상 함수 규칙 수정 완료 (105건/26파일)** — override 함수에 `virtual` 추가 (사용자 확인)
- 2026-08-02: 빌드 0 에러 + 런타임 정상 (`[UI_Generic] OnInit(XML)`, AddWindow, 크래시 없음) 확인
