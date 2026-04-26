# Plan: CoRun 스택 Touch & Growth 테스트 코드 작성

## 날짜
2026-04-26

## 목표
`Test_CoRun()` 함수에 스택 touch & growth 로직에 대한 꼼꼼한 테스트 케이스 작성

---

## 분석 요약

### 핵심 구조 (Mid tier = 16 pages 기준)
```
pStackBase_   (높은 주소)
[pageInitCount_ pages : COMMIT RW]
pStackLimit_
[pageGuardCount_ pages : COMMIT GUARD]
pGuardLimit_
[나머지 : RESERVE]
pStackEnd_    (낮은 주소)
```

### ExpandStack 로직 요약
1. `pFaultStart` = 폴트 주소를 페이지 정렬
2. `pFaultEnd` = `pFaultStart + pageGuardCount_ * PAGE` (단, pStackBase_ 초과 시 클램프)
3. `pGrowthStart` = `pFaultStart - pageGrowCount_ * PAGE` (단, pStackEnd_ 미만 시 클램프)
4. `[pGrowthStart, pFaultEnd)` 전체를 RW로 commit
5. `pStackLimit_` = `pGrowthStart`
6. `pNewGuardStart` = `pGrowthStart - pageGuardCount_ * PAGE` (단, pStackEnd_ 미만 시 클램프)
7. 새 가드 페이지 commit + PAGE_GUARD
8. `pGuardLimit_` = `pNewGuardStart`

### 테스트 케이스 설계

#### pageGrowCount_ 변경 테스트
| TC | growCount | 예상 pStackLimit_ 이동 | 예상 pGuardLimit_ 이동 |
|---|---|---|---|
| TC01 | 0 | -1 PAGE | -1 PAGE |
| TC02 | 1 | -2 PAGE | -2 PAGE |
| TC03 | 2 | -3 PAGE | -3 PAGE |

(Mid tier, initCount=2, guardCount=3, 1회 VEH 트리거 기준)

#### 가드 페이지 설치 가능 수 테스트 (TC04~TC07)
- 설정: High tier (64 pages), initCount=60, guardCount=3, growCount=0
- 초기: pStackLimit_=pStackEnd_+4*PAGE, pGuardLimit_=pStackEnd_+1*PAGE
- 순차 터치 → 상태 누적 변화

| TC | 터치 횟수 | 설치 가능 가드 수 | pStackLimit_ | pGuardLimit_ |
|---|---|---|---|---|
| TC04 | 1 | 3 (정상) | pStackEnd_+3*PAGE | pStackEnd_ |
| TC05 | 2 | 2 (부족) | pStackEnd_+2*PAGE | pStackEnd_ |
| TC06 | 3 | 1 (부족) | pStackEnd_+1*PAGE | pStackEnd_ |
| TC07 | 4 | 0 (없음) | pStackEnd_ | pStackEnd_ |

---

## 필요 변경 파일

1. `06_CoroutineImpl1.h` — `Configure()` 메서드 선언 추가
2. `06_CoroutineImpl1.cpp` — `Configure()` 구현 추가
3. `06_Coroutine.cpp` — 테스트 케이스 작성 (전체 교체)

**총 3개 파일 (5개 미만 ✓)**

---

## 작업 순서

- [ ] 1. Configure() 메서드 헤더 추가 (`06_CoroutineImpl1.h`)
- [ ] 2. Configure() 메서드 구현 추가 (`06_CoroutineImpl1.cpp`)
- [ ] 3. 검증 헬퍼 함수 작성 (VerifyPtrs, VerifyPageState, VerifyGuardPages)
- [ ] 4. TC01~TC03 (pageGrowCount 0/1/2) 테스트 함수 작성
- [ ] 5. TC04~TC07 (가드 3→2→1→0 소진) 테스트 함수 작성
- [ ] 6. Test_CoRun() 정리 및 서브테스트 연결
- [ ] 7. 빌드 수행 및 오류 수정
