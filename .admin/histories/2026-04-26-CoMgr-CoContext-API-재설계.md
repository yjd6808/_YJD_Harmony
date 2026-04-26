# History: CoMgr API CoContext 기반으로 재설계

## 날짜
2026-04-26

## 목표
`CoMgr` 클래스의 API를 `CoStack*` 기반에서 `CoContext*` 기반으로 재설계.
스택과 컨텍스트의 생명주기가 동일하므로 `CoContext*`를 직접 관리.

---

## 완료된 변경 사항

### `06_CoroutineImpl1.h`
- `CoStackAllocator` → `CoMgr` 클래스 이름 변경
- public API 재선언
  - 추가: `InitCtx`, `AllocCtx`, `FreeCtx`
  - 추가: `FindContextByBase`, `FindContextByAddr`
  - 추가: `TryFindContextByBase`, `TryFindContextByAddr`
  - 유지: `FindStackByBase`, `FindStackByAddr`
  - 추가: `TryFindStackByBase`, `TryFindStackByAddr`
  - 유지: `ExpandStack`, `Clear()`, `Configure()`
  - 변경: `DumpPages` → `DumpStack`
  - private 이동: `Init` → `InitStack`, `Alloc(CoStack*)` → `AllocStack`, `Free(CoStack*)` → `FreeStack`
- 내부 멤버 타입 변경
  - `LinkedList<CoStack*>` → `LinkedList<CoContext*>`
  - `TreeMap<char*, CoStack*>` → `TreeMap<char*, CoContext*>`
- `CoStack` 구조체에 union alias 추가 (`pBaseEnd_`, `pBaseAddr_`)
- extern "C" 함수 이름 변경: `co_alloc_ctx` → `CoAllocCtx`, `co_free_ctx` → `CoFreeCtx`
- `CoValidateStack` 추가
- `#define g_cCoAllocator` → `#define g_cCoMgr`

### `06_CoroutineImpl1.cpp`
- `CoStackAllocator::` → `CoMgr::` 전체 교체
- `InitStack` / `AllocStack` / `FreeStack` private 구현 분리
  - `AllocStack`: tier→size 결정 로직 switch문으로 단순화, `jc_assert` → `jc_assert_msg`
  - `FreeStack`: 커스텀은 `MEM_RELEASE`, 풀 티어는 commit 영역만 `MEM_DECOMMIT`
- `InitCtx` / `AllocCtx` / `FreeCtx` 구현 (using_ 등록/해제)
- `FindContextByBase/Addr`, `TryFindContextByBase/Addr` 구현
- `FindStackByBase/Addr`, `TryFindStackByBase/Addr` 구현 (CoContext에서 `&stack_` 반환)
- `DumpPages` → `DumpStack`
- `CoVEH`: `FindStackByAddr` 유지 (내부적으로 CoContext→stack_ 반환)
- `CoAllocCtx`, `CoFreeCtx` → `g_cCoMgr.AllocCtx/FreeCtx` 래핑으로 단순화
- `CoValidateStack` 추가

### `06_Coroutine.cpp`
- `g_cCoAllocator` → `g_cCoMgr`
- `g_cCoAllocator.Alloc(...)` + `FindStackByBase` → `g_cCoMgr.AllocCtx(...)` 로 대체
- `g_cCoAllocator.Free(&init)` → `g_cCoMgr.FreeCtx(pCtx)`
- `DumpPages` → `DumpStack`
- `SingletonPointer<CoStackAllocator>::Free()` → `SingletonPointer<CoMgr>::Free()`

---

## 영향 파일 (3개)
1. `Projects/jcr/Sources/jcr/06_CoroutineImpl1.h`
2. `Projects/jcr/Sources/jcr/06_CoroutineImpl1.cpp`
3. `Projects/jcr/Sources/jcr/06_Coroutine.cpp`
