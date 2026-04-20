# Done: 04_CoStack VEH 카운터 추가

## 날짜
2026-04-20

## 작업 요약
`VectoredHandler`에 테스트 모드 플래그(`gVehCountMode`)와 호출 카운터(`gVehCallCount`)를 추가하여,
성능 테스트 시 VEH가 실제로 몇 번 호출됐는지 검증할 수 있게 했다.

## 변경 내용

### 변경 파일
- `Projects/jcr/Sources/jcr/04_CoStack.cpp`

### 추가된 전역 변수
```cpp
bool gVehCountMode = false;  // true이면 카운팅 활성화
int  gVehCallCount = 0;      // 실제 VEH 호출 횟수
```

### VectoredHandler 수정
- `gVehCountMode == true` 이고 우리 스택(`FindStack` 통과) 인 경우에만 `++gVehCallCount`
- 기존 확장 로직 변경 없음

### Test_StackExpansionPerf 수정
- 루프 (B) 직전: `gVehCountMode = true`, `gVehCallCount = 0` 리셋
- 루프 (B) 종료 후: `gVehCountMode = false`, 카운트 저장
- 결과 출력에 `VEH 실제 호출 횟수 : N / 1000 (OK | MISMATCH)` 라인 추가
  - 기대값(`TRIAL_COUNT`)과 일치하면 초록, 불일치하면 빨강

## 빌드 결과
- Debug x64: 경고 0, 오류 0 — **성공**
