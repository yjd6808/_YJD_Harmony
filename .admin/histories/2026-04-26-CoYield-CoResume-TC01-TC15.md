# Done: CoYield/CoResume 종합 테스트 TC_CO01~TC_CO15 구현

## 날짜
2026-04-26

## 개요
`Test_CoYeild()` 함수를 15개 종합 테스트 케이스(TC_CO01~TC_CO15)로 확장하였다.
아울러 ASM 구현과 불일치했던 `CoResume` 헤더 반환형을 수정하였다.

---

## 변경 파일

| 파일 | 변경 내용 |
|------|---------|
| `06_CoroutineImpl1.h` | `CoResume` 반환형 `void` → `CoContext*` 수정 |
| `06_Coroutine.cpp` | TC_CO01~TC_CO15 전면 재작성 |

---

## 사전 수정 사항

### CoResume 반환형 불일치 해결
- ASM 구현: `rax`에 `CoContext*` 또는 `nullptr` 반환 (기존부터 구현되어 있었음)
- 헤더 선언이 `void`로 남아 있어 반환값 활용 불가 → `CoContext*`로 수정

---

## 테스트 케이스

| TC | 테스트명 | 핵심 검증 |
|---|---|---|
| TC_CO01 | fn 즉시 완료 | yield 없이 fn() 완료 시 CoRun=nullptr |
| TC_CO02 | yield-resume 1회 | 기본 CoRun→yield→CoResume 흐름 |
| TC_CO03 | yield-resume 3회 루프 | 반복 동작 안정성 |
| TC_CO04 | 3개 코루틴 인터리브 | 실행 순서 1~9 검증 |
| TC_CO05 | fn 인자 CoContext* 검증 | fn()에 전달된 포인터 == CoRun 반환값 |
| TC_CO06 | 10KB 로컬 배열 | VEH 발동 + 스택 페이지 자동 확장 |
| TC_CO07 | Low/Mid/High tier | 각 tier별 코루틴 정상 실행 |
| TC_CO08 | 로컬 변수 보존 | yield/resume 사이 volatile 지역변수 유지 |
| TC_CO09 | 중첩 코루틴 | 코루틴 내에서 CoRun/CoResume 호출 |
| TC_CO10 | GS 복원 검증 | CoRun/CoResume 전후 gs:[8]/gs:[16] 값 일치 |
| TC_CO11 | CoResume(nullptr) | nullptr 전달 시 nullptr 반환 (안전 처리) |
| TC_CO12 | 동일 fn 5회 반복 | 컨텍스트 풀링 안정성, count=10 검증 |
| TC_CO13 | cstCustom tier | 사용자 정의 32페이지 스택 코루틴 |
| TC_CO14 | 깊은 재귀 | 재귀 80단계 ≈ 40KB, VEH 스택 확장 발동 |
| TC_CO15 | 1000회 yield 스트레스 | 대량 반복 안정성, yield=resume=1000 검증 |

---

## 빌드 결과
- 경고 0개, 오류 0개 (Debug / x64)
