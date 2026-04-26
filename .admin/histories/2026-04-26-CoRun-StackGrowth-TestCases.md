# Done: CoRun 스택 Touch & Growth 테스트 코드 작성

## 날짜
2026-04-26

## 개요
`Test_CoRun()` 함수에 스택 touch & growth 로직 전용 테스트 케이스 7개를 작성하고, 빌드까지 완료하였다.

---

## 작업 내용

### 추가/수정 파일

| 파일 | 변경 내용 |
|------|---------|
| `06_CoroutineImpl1.h` | `Configure(_u32, _u32, _u32)` 메서드 선언 추가 |
| `06_CoroutineImpl1.cpp` | `Configure()` 구현 추가 (pageInitCount_, pageGuardCount_, pageGrowCount_ 세터) |
| `06_Coroutine.cpp` | 헬퍼 3개 + 테스트 케이스 7개 전면 재작성 |

---

## 테스트 케이스 구조

### 헬퍼 함수
- `VerifyPtrs()` — pStackLimit_, pGuardLimit_ 값 비교 + PASS/FAIL 출력
- `VerifyPageState()` — VirtualQuery로 특정 주소의 State/Protect 검증
- `VerifyGuardPages()` — 가드 존 전체 PAGE_GUARD 상태, 가드 페이지 수, pStackLimit_ 위 RW 페이지 검증
- `PrintSection()` — 테스트 섹션 구분자 출력
- `TC_GrowCount()` — TC01~03 공통 실행 함수 (growCount 파라미터)

### TC01 — pageGrowCount_=0
- Mid tier (16 pages), initCount=2, guardCount=3
- 가드 1회 터치 후: pStackLimit_ -1 PAGE, pGuardLimit_ -1 PAGE

### TC02 — pageGrowCount_=1
- 가드 1회 터치 후: pStackLimit_ -2 PAGE, pGuardLimit_ -2 PAGE

### TC03 — pageGrowCount_=2 (기본값)
- 가드 1회 터치 후: pStackLimit_ -3 PAGE, pGuardLimit_ -3 PAGE

### TC04 ~ TC07 — 가드 페이지 설치 가능 수 소진 (동일 스택, 순차 터치)
- High tier (64 pages), initCount=60, guardCount=3, growCount=0
- 초기: pStackLimit_=pStackEnd_+4*PAGE, pGuardLimit_=pStackEnd_+1*PAGE

| TC | 터치 횟수 | 설치 가드 수 | pStackLimit_ | pGuardLimit_ |
|----|---------|------------|--------------|--------------|
| TC04 | 1 | 3 (정상) | pStackEnd_+3*PAGE | pStackEnd_ |
| TC05 | 2 | 2 (부족) | pStackEnd_+2*PAGE | pStackEnd_ |
| TC06 | 3 | 1 (부족) | pStackEnd_+1*PAGE | pStackEnd_ |
| TC07 | 4 | 0 (없음) | pStackEnd_ | pStackEnd_ |

---

## 검증 방법 (각 TC)
1. `VerifyPtrs()` — 포인터 기댓값과 실제값 비교
2. `VerifyGuardPages()` — VirtualQuery로 PAGE_GUARD 상태 및 개수 검증
3. `VerifyPageState()` — pStackLimit_ 위 RW 페이지 존재 확인
4. `g_cCoAllocator.DumpPages()` — 시각적 확인용 페이지 덤프

---

## 빌드 결과
- 경고 0개, 오류 0개 (Debug / x64)
