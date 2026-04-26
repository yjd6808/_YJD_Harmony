# Plan: CoYield/CoResume 종합 테스트 케이스 TC_CO01~TC_CO15 구현

## 날짜
2026-04-26

## 목표
`Test_CoYeild()` 함수를 TC_CO01~TC_CO15 종합 테스트 케이스로 확장.
CoRun / CoYield / CoResume / CoFnEndTrampoline 전체 흐름을 검증.

---

## 사전 발견 이슈

### CoResume 반환형 불일치
- ASM 구현: rax에 `CoContext*` 또는 `nullptr` 반환
- 현재 헤더: `void ASM_CALL CoResume(CoContext* _pCtx)` ← 수정 필요
- 수정 후: `CoContext* ASM_CALL CoResume(CoContext* _pCtx)`

---

## 테스트 케이스 설계

| TC | 테스트명 | 핵심 검증 |
|---|---|---|
| TC_CO01 | fn 즉시 완료 | yield 없이 fn() 완료 시 CoRun=nullptr |
| TC_CO02 | yield-resume 1회 | 기본 CoRun→yield→CoResume 흐름 |
| TC_CO03 | yield-resume 3회 루프 | 반복 동작 안정성 |
| TC_CO04 | 3개 코루틴 인터리브 | 여러 코루틴 동시 운용 |
| TC_CO05 | fn 인자 CoContext* 검증 | fn()에 전달된 포인터 == CoRun 반환값 |
| TC_CO06 | 10KB 로컬 배열 | VEH 발동 + 스택 페이지 자동 확장 |
| TC_CO07 | Low/Mid/High tier | 각 tier별 코루틴 정상 실행 |
| TC_CO08 | 로컬 변수 보존 | yield/resume 사이 volatile 지역변수 유지 |
| TC_CO09 | 중첩 코루틴 | 코루틴 내에서 CoRun/CoResume 호출 |
| TC_CO10 | GS 복원 검증 | yield/resume 전후 gs:[8]/gs:[16] 값 일치 |
| TC_CO11 | CoResume(nullptr) 안전 | nullptr 전달 시 nullptr 반환 |
| TC_CO12 | 동일 fn 5회 반복 | 컨텍스트 재사용(풀링) 안정성 |
| TC_CO13 | cstCustom tier | 사용자 정의 스택 크기 코루틴 |
| TC_CO14 | 깊은 재귀 | 재귀로 VEH 스택 확장 발동 |
| TC_CO15 | 1000회 yield 스트레스 | 대량 반복 안정성 |

---

## 필요 변경 파일

1. `06_CoroutineImpl1.h` — CoResume 반환형 void → CoContext*
2. `06_Coroutine.cpp` — TC_CO01~TC_CO15 구현 (전면 재작성)

**총 2개 파일 (5개 미만 ✓)**

---

## 작업 순서

- [ ] 1. CoResume 반환형 수정 (`06_CoroutineImpl1.h`)
- [ ] 2. TC_CO01~TC_CO05 구현 (기본 동작)
- [ ] 3. TC_CO06~TC_CO07 구현 (스택 확장 / tier 변형)
- [ ] 4. TC_CO08~TC_CO10 구현 (로컬 변수 / 중첩 / GS 검증)
- [ ] 5. TC_CO11~TC_CO13 구현 (엣지 케이스)
- [ ] 6. TC_CO14~TC_CO15 구현 (깊은 재귀 / 스트레스)
- [ ] 7. Test_CoYeild() 진입점 정리 및 서브테스트 연결
- [ ] 8. 빌드 수행 및 오류 수정
