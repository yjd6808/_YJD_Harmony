# 작업 완료: CoMgr ThreadLocal + CurrentCtx

## 작업 일자
2026-04-26

## 목표
- `CoMgr` singleton → `thread_local` 값 객체 전환
- `CoContext`에 `callerCtx_` 추가 (중첩 코루틴 currentCtx_ 복원용)
- `CoMgr`에 `currentCtx_` + `GetCurrentCtx()` 추가
- `CoOnBeforeLaunch` / `CoOnAfterLaunch` ASM 호출용 C++ 함수 추가
- `CoCurrentCtx` / `CoValidateAddr` extern "C" 추가
- `CoFnEndTrampoline`, `CoYield`에서 `CoFindCtxByAddr` 제거 → `CoCurrentCtx` 사용
- `CoVEH` 단순화: TreeMap O(log n) → `GetCurrentCtx()` O(1) + `s_inCoVEH` bool

## 수정 파일

### 06_CoroutineImpl1.h
- `CoContext`에 `callerCtx_ = nullptr` 필드 추가 (offset 320)
- `CoMgr`: `jc::SingletonPointer<CoMgr>` 상속 제거
- `CoMgr`: `currentCtx_` (public) + `GetCurrentCtx()` 추가
- `#define g_cCoMgr` → `extern thread_local CoMgr g_cCoMgr`
- extern "C": `CoCurrentCtx`, `CoValidateAddr`, `CoOnBeforeLaunch`, `CoOnAfterLaunch` 선언
- `CoFindCtxByAddr` 선언 제거

### 06_CoroutineImpl1.cpp
- `thread_local CoMgr g_cCoMgr` 정의 추가
- `CoMgr::InitCtx`: `callerCtx_ = nullptr` 초기화 추가
- `CoMgr::Clear`: `currentCtx_ = nullptr` 추가
- `CoVEH` 재작성: `s_inCoVEH bool` + `GetCurrentCtx()` O(1) 범위 체크
- `CoOnBeforeLaunch`: `callerCtx_` 저장 + `currentCtx_` 교체
- `CoOnAfterLaunch`: `currentCtx_` → `callerCtx_` 복원
- `CoCurrentCtx`: `GetCurrentCtx()` 래퍼
- `CoValidateAddr`: 스택 범위 체크
- `CoFindCtxByAddr` 제거

### 06_CoroutineImpl2.asm
- extern 선언: `CoFindCtxByAddr` 제거 → `CoCurrentCtx`, `CoValidateAddr`, `CoOnBeforeLaunch`, `CoOnAfterLaunch` 추가
- `CoFnEndTrampoline`: `CoFindCtxByAddr(rsp)` → `CoCurrentCtx()` (인자 없음)
- `CoYield`: `CoFindCtxByAddr(rsp)` → `CoCurrentCtx()` + `CoValidateAddr(pCtx, rbp)`
  - rbx를 CoContext* 임시 보관용으로 활용
- `CoRun`: 코루틴 스택 전환 후 `CoOnBeforeLaunch`, YIELD에서 `CoOnAfterLaunch`
- `CoResume`: 코루틴 스택 전환 후 `CoOnBeforeLaunch`, YIELD에서 `CoOnAfterLaunch`

### 06_Coroutine.cpp
- `jc::SingletonPointer<CoMgr>::Free()` 호출 2곳 제거 (Test_CoStack, Test_CoYeild)

## 빌드 결과
- 경고 0개, 오류 0개 (Debug/x64)

## 주요 설계 결정
- `currentCtx_`를 `public`으로 선언: `CoOnBeforeLaunch/AfterLaunch`가 extern "C"이므로
  friend 선언 시 linkage 충돌(C2732) 발생 → public으로 해결
- `CoValidateAddr` 인자로 `rbp` 사용: CoYield 진입 시 rbp = 코루틴 스택 내 주소

## ASM 스택 정렬 요약
| 위치 | RSP 상태 | 조치 |
|------|----------|------|
| CoRun/CoResume: 코루틴 스택 전환 직후 | 0 mod 16 | push rax + sub 40 → 0 ✓ |
| CoRun/CoResume: YIELD 진입 | 8 mod 16 | mov rcx [rsp] + sub 40 → 0 ✓ |
| CoFnEndTrampoline 진입 | 0 mod 16 | sub 32 → 0 ✓ |
| CoYield: sub rsp 40 후 | 0 mod 16 | 직접 call ✓ |
