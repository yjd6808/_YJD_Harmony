# Plan: 04_CoStack StopWatchHR 교체

## 날짜
2026-04-20

## 작업 배경
`Test_StackExpansionPerf`의 타이밍 코드를 `QueryPerformanceCounter` 직접 호출 대신
프로젝트 표준 API인 `jc::StopWatchHR`로 교체.

## 수정 계획

### Test_StackExpansionPerf 변경
- `LARGE_INTEGER freq, t0, t1` 제거
- `QueryPerformanceFrequency`, `QueryPerformanceCounter` 제거
- 루프 (A): `StopWatchHR swA; swA.Start(); ... TimeSpan tsA = swA.Stop();`
- 루프 (B): `StopWatchHR swB; swB.Start(); ... TimeSpan tsB = swB.Stop();`
- 시간 계산: `TimeSpan::GetTotalMiliSeconds()` → ms, `GetTotalMicroSeconds()` → µs
- ns는 µs × 1000 으로 계산 (TimeSpan에 ns 단위 없음)

## 예상 변경 파일
- `Projects/jcr/Sources/jcr/04_CoStack.cpp` (단일 파일)
