# Plan: 05_PageGuard VEH 연속 호출 수정

## 날짜
2026-04-19

## 작업 배경
`PageGuardVectoredHandler`에서 `EXCEPTION_CONTINUE_SEARCH`를 반환하면 OS가 스택 확장을
커널 레벨에서 직접 처리한다. 이 경우 OS가 새로 설정한 가드 페이지는 이후 커널이 직접
처리하므로 user-mode VEH 체인이 다시 호출되지 않는다.

## 원인 분석
- `EXCEPTION_CONTINUE_SEARCH` → 예외 핸들러 체인 계속 탐색
- OS 내부 핸들러(ntdll)가 스택 가드 페이지 확장을 커널 수준에서 처리
- 이후 OS가 설정한 새 가드 페이지 위반은 커널이 직접 처리 → user-mode VEH 미호출

## 수정 계획
1. VEH에서 스택 확장을 직접 처리하고 `EXCEPTION_CONTINUE_EXECUTION` 반환
2. 스택 범위 검사: `NT_TIB`의 `StackBase` / `StackLimit` 기반
3. 가드 페이지 확장 처리:
   - 가드 페이지 → PAGE_READWRITE 전환 (VirtualProtect)
   - 아래 페이지(스택 확장 방향) 커밋 + GUARD 설정 (VirtualAlloc)
   - `NT_TIB::StackLimit` 갱신 → 런타임 스택 범위 인식 유지
4. 스택 범위 외 가드 페이지는 `EXCEPTION_CONTINUE_SEARCH`로 위임 유지

## 예상 변경 파일
- `Projects/jcr/Sources/jcr/05_PageGuard.cpp` (VEH 핸들러 수정)
