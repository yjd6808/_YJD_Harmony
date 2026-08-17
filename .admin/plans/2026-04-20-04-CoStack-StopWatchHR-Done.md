# Done: 04_CoStack StopWatchHR 교체

## 날짜
2026-04-20

## 작업 요약
`Test_StackExpansionPerf`의 타이밍 코드를 `QueryPerformanceCounter` → `jc::StopWatchHR`로 교체.

## 변경 내용

### 변경 파일
- `Projects/jcr/Sources/jcr/04_CoStack.cpp`

### 제거
- `LARGE_INTEGER freq, t0, t1`
- `QueryPerformanceFrequency`, `QueryPerformanceCounter` 직접 호출
- 수동 경과시간 계산식 `(t1 - t0) * 1000.0 / freq`

### 추가
- 루프 (A): `StopWatchHR swA; swA.Start(); ... TimeSpan tsA = swA.Stop();`
- 루프 (B): `StopWatchHR swB; swB.Start(); ... TimeSpan tsB = swB.Stop();`
- 시간 추출: `tsA.GetTotalMiliSeconds()`, `tsB.GetTotalMiliSeconds()`

## 빌드 결과
- Debug x64: 경고 0, 오류 0 — **성공**
