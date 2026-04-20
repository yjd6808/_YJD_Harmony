# Plan: 04_CoStack ConsoleMenuItem 서브메뉴 추가

## 날짜
2026-04-20

## 작업 배경
`call_04_CoStack` 함수에 ConsoleMenuItem 서브메뉴를 도입하여 기능별로 분리:
1. 스택확장 테스트 - 기존 코드 그대로
2. 스택확장 성능 - VEH 핸들러를 1000번 의도적으로 트리거하여 호출 비용 측정

## 작업 내용

### 1. 스택확장 테스트 (기존 코드)
- 기존 `call_04_CoStack` 본문을 별도 함수 `Test_StackExpansion`으로 분리
- 흐름: StackAlloc → TouchStack(VEH) → StackInit → TouchStack(VEH) → StackFree

### 2. 스택확장 성능 (신규)
- 별도 함수 `Test_StackExpansionPerf` 작성
- 동일한 스택을 재사용하며 1000번 루프:
  - StackInit으로 초기화
  - GUARD 페이지 터치 → VEH 핸들러 트리거
  - 총 소요 시간 / 평균 1회 VEH 비용 출력
- `QueryPerformanceCounter` / `QueryPerformanceFrequency` 로 고해상도 타이밍

### 3. ConsoleMenuItem 서브메뉴 구성
- `call_04_CoStack` 함수: 서브메뉴 빌드 + Show
  - "1" → 스택확장 테스트
  - "2" → 스택확장 성능
  - "0" → 뒤로가기

## 예상 변경 파일
- `Projects/jcr/Sources/jcr/04_CoStack.cpp` (단일 파일 수정)
