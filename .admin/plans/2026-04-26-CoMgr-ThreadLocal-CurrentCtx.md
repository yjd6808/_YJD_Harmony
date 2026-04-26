# 계획서: CoMgr ThreadLocal + CurrentCtx

## 작업 목표
- `CoMgr` singleton → `thread_local` 값 객체 전환
- `CoContext`에 `callerCtx_` 추가
- `CoMgr`에 `currentCtx_` + `GetCurrentCtx()` 추가
- `CoOnBeforeLaunch` / `CoOnAfterLaunch` C++ 함수 추가 (ASM에서 호출)
- `CoCurrentCtx` / `CoValidateAddr` extern "C" 함수 추가
- `CoFnEndTrampoline` / `CoYield` 에서 `CoFindCtxByAddr` 제거 → `CoCurrentCtx` 사용
- `CoVEH` 단순화: `FindStackByAddr` (TreeMap O(log n)) → `GetCurrentCtx()` (O(1)) + `s_inCoVEH` bool

## 수정 파일 목록
1. `Projects/jcr/Sources/jcr/06_CoroutineImpl1.h`
2. `Projects/jcr/Sources/jcr/06_CoroutineImpl1.cpp`
3. `Projects/jcr/Sources/jcr/06_CoroutineImpl2.asm`
4. `Projects/jcr/Sources/jcr/06_Coroutine.cpp`

## CoContext 레이아웃 변경
- 기존: id_(0) + threadId_(4) + regs_(8,248B) + stack_(256,48B) + state_(304) + _pad0_(308) + fn_(312) = 320B
- 신규: + callerCtx_(320, 8B) = 328B
- callerCtx_는 C++에서만 접근 → ASM struct 변경 불필요

## 신규 함수
```
CoOnBeforeLaunch(CoContext*) - 코루틴 스택 전환 직후, fn 실행 직전
  pCtx->callerCtx_ = g_cCoMgr.currentCtx_;
  g_cCoMgr.currentCtx_ = pCtx;

CoOnAfterLaunch(CoContext*) - 스케줄러 스택 복귀 직후
  g_cCoMgr.currentCtx_ = pCtx->callerCtx_;

CoCurrentCtx() → CoContext* - currentCtx_ 직접 반환 (O(1))

CoValidateAddr(CoContext*, char* pAddr) → bool
  pAddr >= pCtx->stack_.pStackEnd_ && pAddr < pCtx->stack_.pStackBase_
```

## ASM 스택 정렬 계산

### CoRun/CoResume → CoOnBeforeLaunch 호출
- 코루틴 스택 전환 직후 RSP = 0 mod 16
- push rax → 8 mod 16, sub rsp 40 → 0 mod 16, call ✓
- add rsp 40, pop rax → RSP 복원

### CoRun/CoResume YIELD → CoOnAfterLaunch 호출
- YIELD 진입 시 RSP = 8 mod 16 (top = CoContext*)
- mov rcx [rsp] (peek), sub rsp 40 → 0 mod 16, call ✓
- add rsp 40, pop rcx

### CoFnEndTrampoline → CoCurrentCtx 호출
- 진입 시 RSP = pStackBase_ - 32 (0 mod 16)
- sub rsp 32, call CoCurrentCtx ✓, add rsp 32

### CoYield → CoCurrentCtx + CoValidateAddr 호출
- 진입 후 sub rsp 40 → RSP = 0 mod 16
- call CoCurrentCtx (no args), rax = CoContext*, 저장 → rbx
- mov rcx rbx, mov rdx rbp, call CoValidateAddr
- mov rax rbx → CoContext* 복원

## CoVEH 신규 구조
```
static thread_local bool s_inCoVEH = false;

CoVEH:
  if (not GUARD_PAGE_VIOLATION) → CONTINUE_SEARCH
  if (s_inCoVEH):
    ctx = GetCurrentCtx()
    if ctx && pFaultAddr in ctx.stack → CONTINUE_EXECUTION
    else → CONTINUE_SEARCH
  ctx = GetCurrentCtx()
  if (ctx == nullptr) → CONTINUE_SEARCH
  if (pFaultAddr not in ctx.stack) → CONTINUE_SEARCH
  s_inCoVEH = true
  result = ExpandStack(&ctx.stack, pFaultAddr) ? CONTINUE_EXECUTION : CONTINUE_SEARCH
  s_inCoVEH = false
  return result
```
