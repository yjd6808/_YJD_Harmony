# Plan: CoMgr API CoContext 기반으로 재설계

## 날짜
2026-04-26

## 목표
`CoMgr` 클래스의 API를 `CoStack*` 기반에서 `CoContext*` 기반으로 재설계.
스택과 컨텍스트의 생명주기가 동일하므로 `CoContext*`를 직접 관리.

---

## 변경 사항

### `06_CoroutineImpl1.h`
- `CoMgr` public API 재선언
  - 추가: `InitCtx`, `AllocCtx`, `FreeCtx`
  - 추가: `FindContextByBase`, `FindContextByAddr`
  - 추가: `TryFindContextByBase`, `TryFindContextByAddr`
  - 유지: `FindStackByBase`, `FindStackByAddr`, `TryFindStackByBase`, `TryFindStackByAddr`
  - 유지: `ExpandStack(CoStack*, char*)`, `Clear()`, `Configure()`
  - 변경: `DumpPages` → `DumpStack`
  - private 이동: `Init` → `InitStack`, `Alloc(CoStack*)` → `AllocStack`, `Free(CoStack*)` → `FreeStack`
- 내부 멤버 타입 변경
  - `LinkedList<CoStack*>` → `LinkedList<CoContext*>`
  - `TreeMap<char*, CoStack*>` → `TreeMap<char*, CoContext*>`

### `06_CoroutineImpl1.cpp`
- `CoStackAllocator::` → `CoMgr::` 전체 교체
- `InitStack` / `AllocStack` / `FreeStack` private 구현
- `InitCtx` / `AllocCtx` / `FreeCtx` 구현
- `FindContextByBase/Addr`, `TryFindContextByBase/Addr` 구현
- `FindStackByBase/Addr`, `TryFindStackByBase/Addr` 구현 (CoContext에서 stack_ 반환)
- `DumpPages` → `DumpStack`
- `CoVEH` 업데이트: `FindStackByAddr` 유지 (CoStack* 반환)
- `CoAllocCtx`, `CoFreeCtx` → `AllocCtx`, `FreeCtx` 래핑으로 단순화

### `06_Coroutine.cpp`
- `FindStackByBase` → `FindContextByBase` 후 `&pCtx->stack_`
- `DumpPages` → `DumpStack`
- `g_cCoAllocator` 잔재 정리

## 영향 파일 (3개)
1. `06_CoroutineImpl1.h`
2. `06_CoroutineImpl1.cpp`
3. `06_Coroutine.cpp`
