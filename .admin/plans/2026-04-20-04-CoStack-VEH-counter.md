# Plan: 04_CoStack VEH 카운터 추가

## 날짜
2026-04-20

## 작업 배경
성능 테스트 시 VEH 핸들러가 실제로 몇 번 호출됐는지 검증할 수 없는 문제.
테스트 모드 플래그와 카운터를 추가하여 실제 호출 횟수를 확인한다.

## 수정 계획

### 추가할 전역 변수 (04_CoStack.cpp 상단)
- `bool gVehCountMode = false` — 카운팅 활성 여부 (기본 꺼짐)
- `int  gVehCallCount = 0`    — 실제 VEH 호출 횟수

### VectoredHandler 수정
- `gVehCountMode == true` 이고 우리 스택에 대한 위반인 경우에만 `gVehCallCount++`
- 기존 확장 로직은 그대로 유지

### Test_StackExpansionPerf 수정
- 루프 (B) 시작 전: `gVehCountMode = true`, `gVehCallCount = 0`
- 루프 (B) 종료 후: `gVehCountMode = false`
- 결과 출력 시 `gVehCallCount` 출력 (기대값 TRIAL_COUNT와 비교)

## 예상 변경 파일
- `Projects/jcr/Sources/jcr/04_CoStack.cpp` (단일 파일)
