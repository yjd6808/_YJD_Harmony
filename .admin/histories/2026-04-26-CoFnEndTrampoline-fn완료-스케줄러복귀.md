# CoFnEndTrampoline - fn 완료 후 스케줄러 복귀 구현

## 날짜
2026-04-26

## 작업 개요
코루틴 함수(fn)가 정상 종료(ret)될 때 스케줄러로 올바르게 복귀하는 `CoFnEndTrampoline` 어셈블리 함수를 구현하고, `CoRun`에서 call 대신 push-trampoline + jmp 패턴을 적용했다.

## 변경 파일
- `Projects/jcr/Sources/jcr/06_CoroutineImpl2.asm`
- `Projects/jcr/Sources/jcr/06_CoroutineImpl1.h`

## 구현 내용

### CoFnEndTrampoline
fn()이 ret할 때 진입하는 트램폴린 함수.

**역할:**
1. `CoFindCtxByAddr`로 현재 `CoContext` 탐색 (현재 RSP를 인자로 전달)
2. 스케줄러의 GS(TEB StackBase/StackLimit) 복원
3. 스케줄러의 callee-saved 정수 레지스터(RSI, RDI, R12~R15) 복원
4. 스케줄러의 callee-saved XMM 레지스터(XMM6~15) 복원
5. `state = csEnd` 설정
6. 스케줄러 스택으로 전환 + 스케줄러의 RBP 복원
7. `regs_.rip_`(스케줄러의 YIELD 레이블)로 점프

**진입 시 스택 레이아웃:**
- RSP = pStackBase_ - 32 (코루틴 스택, 16-byte 정렬)
- fn()의 ret이 이미 CoFnEndTrampoline 주소를 pop 완료한 상태

### CoRun push-trampoline + jmp 패턴
기존 `call fn()` 방식을 `push CoFnEndTrampoline + jmp fn()` 패턴으로 변경:
- fn() 정상 종료 시 CoFnEndTrampoline으로 복귀
- 마지막으로 재개한 스케줄러(CoRun/CoResume)의 YIELD로 올바르게 점프
- 스택 레이아웃은 call과 동일 (push + jmp = 호출 효과)

### YIELD 레이블 처리 (CoRun/CoResume 공통)
YIELD 진입 시:
- 스케줄러 스택 top에서 CoContext* pop
- state 확인: csYield면 FIN으로 분기 (CoContext* 반환)
- csEnd면 CoFreeCtx 호출 후 nullptr 반환

### CoResume 구현
CoYield와 대칭적인 구조로 구현:
- 스케줄러의 모든 callee-saved 레지스터와 코루틴의 레지스터를 xchg
- 저장된 코루틴 RSP/RIP으로 점프하여 코루틴 재개
- 복귀 시 YIELD 레이블에서 동일 처리

## 핵심 설계 결정
- `CoFnEndTrampoline`은 코루틴 스택 위에서 동작하므로, `CoFindCtxByAddr`에 RSP를 전달하여 어느 코루틴인지 식별
- 복원 순서: GS → 정수 레지스터 → XMM → state 설정 → 스택 전환 → 점프
- `CoRun`과 `CoResume` 모두 동일한 YIELD 처리 구조로 csEnd(fn 정상 종료)와 csYield 양쪽을 처리
