# Done: 04_CoStack ConsoleMenuItem 서브메뉴 추가

## 날짜
2026-04-20

## 작업 요약
`04_CoStack.cpp`에 ConsoleMenuItem 서브메뉴를 도입하여 기능을 두 항목으로 분리 완료.

## 변경 내용

### 변경 파일
- `Projects/jcr/Sources/jcr/04_CoStack.cpp`

### 구조 변경
| 항목 | 내용 |
|------|------|
| `Test_StackExpansion` (신규 분리) | 기존 `call_04_CoStack` 본문 그대로 이전 |
| `Test_StackExpansionPerf` (신규) | VEH 1000회 트리거 성능 측정 |
| `call_04_CoStack` | ConsoleMenuItem 서브메뉴 빌드 + Show |

### 성능 측정 로직 (`Test_StackExpansionPerf`)
- `QueryPerformanceCounter` 고해상도 타이머 사용
- **(A)** StackInit 전용 1000회 루프 → 기준 비용 측정
- **(B)** StackInit + GUARD 페이지 터치(VEH 트리거) 1000회 루프 → 총 비용 측정
- 순수 VEH 비용 = (B) - (A)
- 출력: 합산 ms / 1회 평균 µs + ns

## 빌드 결과
- Debug x64: 경고 0, 오류 0 — **성공**
