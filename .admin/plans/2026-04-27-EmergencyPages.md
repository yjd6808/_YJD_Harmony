# Plan: EmergencyPages (2026-04-27)

## 목적
CO14-B 테스트에서 코루틴 내부 `__try/__except`로 `STATUS_STACK_OVERFLOW` 포착.
현재 실패 원인: 오버플로우 가드 페이지 터치 시 RSP가 스택 바닥 근처 →
SEH 예외 디스패처가 스택 프레임을 push할 공간 없음.

해결: 스택 맨 아래에 **비상 페이지(Emergency Pages)** N개를 RESERVE로 예약,
오버플로우 감지 시 COMMIT → SEH 디스패처 동작 공간 확보.

## 설계 결정

### exceptionPageCount_ 기본값
- `exceptionPageCount_ = 0` (기본값, 기존 동작 유지)
- CO14-B 테스트에서만 `Configure(2, 3, 2, 4)` 로 활성화
- 이유: 기본값 > 0 이면 cstLow(4 pages) 할당이 불가능해짐
  - exceptionPageCount_=4 기준 필요 최소 페이지 = 4+1+3+2 = 10 pages
  - cstLow = 4 pages → 불가
  - cstMid = 16 pages → 가능 (16-10=6 pages 여유)

### 스택 레이아웃 (exceptionPageCount_ = N > 0 시)
```
pStackBase_           ← 높은 주소, 초기 RSP
  [COMMIT RW]           ← pageInitCount_ pages
pStackLimit_
  [PAGE_GUARD]          ← pageGuardCount_ pages
pGuardLimit_
  [RESERVE]
pOverflowGuardTop = pStackEnd_ + (N+1)*PAGE
  [OVERFLOW GUARD]      ← 1 page, 영구 PAGE_GUARD
pStackEnd_ + N*PAGE
  [EMERGENCY: RESERVE]  ← N pages, 오버플로우 시 COMMIT
pStackEnd_            ← 낮은 주소
```

## 수정 대상 파일 (3개)

### 1. `06_CoroutineImpl1.h`
- `CoMgr` private 멤버 추가: `_u32 exceptionPageCount_ = 0;`
- `Configure()` 선언 업데이트:
  ```cpp
  void Configure(_u32 _pageInitCount, _u32 _pageGuardCount,
                 _u32 _pageGrowCount, _u32 _exceptionPageCount = 0);
  ```

### 2. `06_CoroutineImpl1.cpp`

**Configure()**
```cpp
void CoMgr::Configure(_u32 _pageInitCount, _u32 _pageGuardCount,
                      _u32 _pageGrowCount, _u32 _exceptionPageCount)
{
    pageInitCount_      = _pageInitCount;
    pageGuardCount_     = _pageGuardCount;
    pageGrowCount_      = _pageGrowCount;
    exceptionPageCount_ = _exceptionPageCount;
}
```

**InitStack()**
- `pOverflowGuardTop = pStackEnd_ + (exceptionPageCount_ + 1) * CO_PAGE_SIZE`
- Overflow guard COMMIT 위치: `pStackEnd_ + exceptionPageCount_ * CO_PAGE_SIZE` (1 page)
- Emergency pages [pStackEnd_, pStackEnd_ + exceptionPageCount_*PAGE) → RESERVE 유지

**ExpandStack()**
- `pOverflowGuardTop` 공식: `pStackEnd_ + (exceptionPageCount_ + 1) * CO_PAGE_SIZE`
- `_pFaultAddr < pOverflowGuardTop` 시 비상 페이지 COMMIT + TEB StackLimit 업데이트:
  ```cpp
  if (exceptionPageCount_ > 0)
  {
      VirtualAlloc(_pStack->pStackEnd_,
                   exceptionPageCount_ * CO_PAGE_SIZE,
                   MEM_COMMIT, PAGE_READWRITE);
      __writegsqword(16, (DWORD64)_pStack->pStackEnd_);
  }
  return false;
  ```

**CoVEH()**
- 오버플로우 가드 범위 체크 (2곳):
  - 기존: `pFaultAddr < pCtx->stack_.pStackEnd_ + CO_PAGE_SIZE`
  - 변경: `pFaultAddr < pCtx->stack_.pStackEnd_ + (g_cCoMgr.exceptionPageCount_ + 1) * CO_PAGE_SIZE`
  - → `exceptionPageCount_` public 접근 필요 (public 멤버로 선언)

### 3. `06_Coroutine_CoBasic.cpp`
- CO14-B 시작 전: `g_cCoMgr.Configure(2, 3, 2, 4);`
- `CoRun(fn_CO14B, cstLow)` → `CoRun(fn_CO14B, cstMid)`
- 관련 주석 업데이트

## 완료 기준
- 빌드 성공 (경고 0, 오류 0)
- CO14-B: `g_CO14B_caught == true` && `g_CO14B_code == STATUS_STACK_OVERFLOW`
- CO01~CO13, CO15: 모두 PASS 유지
