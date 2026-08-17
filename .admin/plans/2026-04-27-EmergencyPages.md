# EmergencyPages 작업 이력

**날짜:** 2026-04-27

---

## 작업 요약

코루틴 스택 오버플로우 시 `__try/__except`로 `STATUS_STACK_OVERFLOW`를 포착하기 위한
**비상 페이지(Emergency Pages)** 기능 구현 완료.

---

## 변경된 파일

1. `Projects/jcr/Sources/jcr/06_CoroutineImpl1.h`
2. `Projects/jcr/Sources/jcr/06_CoroutineImpl1.cpp`
3. `Projects/jcr/Sources/jcr/06_Coroutine_CoBasic.cpp`

---

## 상세 변경 내용

### 06_CoroutineImpl1.h

- `CoMgr` public 멤버 추가: `_u32 exceptionPageCount_ = 0;`
  - CoVEH에서 직접 접근 필요하여 public 배치
- `Configure()` 선언 업데이트: 4번째 파라미터 `_u32 _exceptionPageCount = 0` 추가

### 06_CoroutineImpl1.cpp

**Configure()**
- `exceptionPageCount_` 설정 로직 추가

**InitStack()**
- 스택 레이아웃 변경:
  - 오버플로우 가드 위치: `pStackEnd_` → `pStackEnd_ + exceptionPageCount_ * CO_PAGE_SIZE`
  - `pOverflowGuardTop = pStackEnd_ + (exceptionPageCount_ + 1) * CO_PAGE_SIZE`
  - 비상 페이지 `[pStackEnd_, pStackEnd_ + exceptionPageCount_*PAGE)` → RESERVE 유지 (추가 처리 불필요)

**ExpandStack()**
- `pOverflowGuardTop` 공식 갱신
- `_pFaultAddr < pOverflowGuardTop` 감지 시:
  - `VirtualAlloc(pStackEnd_, exceptionPageCount_*PAGE, MEM_COMMIT, PAGE_READWRITE)` 호출
  - `__writegsqword(16, pStackEnd_)` → TEB StackLimit 갱신 (SEH 디스패처 인식용)
  - return false

**CoVEH()**
- 오버플로우 가드 범위 체크 2곳 업데이트:
  - 기존: `pStackEnd_ + CO_PAGE_SIZE`
  - 변경: `pStackEnd_ + (g_cCoMgr.exceptionPageCount_ + 1) * CO_PAGE_SIZE`

### 06_Coroutine_CoBasic.cpp

**CO14-B**
- CO14-B 시작 전 `g_cCoMgr.Configure(2, 3, 2, 4)` 추가 (비상 페이지 4개=16KB 활성화)
- `CoRun(fn_CO14B, cstLow)` → `CoRun(fn_CO14B, cstMid)` 변경
  - 이유: exceptionPageCount_=4 기준 최소 스택 = (4+1+3+2)*4KB = 40KB > cstLow(16KB)
  - cstMid(64KB) 사용 시 여유 = 64KB - 40KB = 24KB
- CO14-B 종료 후 `g_cCoMgr.Configure(2, 3, 2, 0)` 으로 복원
- 관련 주석 업데이트

---

## 빌드 결과

- 경고 0개, 오류 0개 (빌드 성공)

---

## 설계 결정 기록

- `exceptionPageCount_` 기본값 = 0 (기존 동작 유지, cstLow와의 호환성 유지)
- CO14-B에서만 명시적으로 4로 설정
- `exceptionPageCount_`를 public으로 배치: CoVEH(friend 아님)에서 직접 접근 필요

---

## 스택 레이아웃 (exceptionPageCount_ = N > 0 시)

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
  [EMERGENCY: RESERVE → 오버플로우 시 COMMIT]  ← N pages
pStackEnd_            ← 낮은 주소
```
