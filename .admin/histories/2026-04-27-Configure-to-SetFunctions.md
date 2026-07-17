# Configure-to-SetFunctions 작업 히스토리

- **날짜**: 2026-04-27
- **상태**: 완료
- **빌드 결과**: 경고 0, 오류 0

---

## 작업 요약

`CoMgr::Configure()` 함수를 제거하고 개별 Set 함수 4개로 분리. `exceptionPageCount_` 기본값 8로 설정. CoStack에 `exceptionPageCount_` 필드 추가 (InitStack에서 스택 크기 기반 클램프).

---

## 변경 파일 (4개)

1. `Projects/jcr/Sources/jcr/06_CoroutineImpl1.h`
2. `Projects/jcr/Sources/jcr/06_CoroutineImpl1.cpp`
3. `Projects/jcr/Sources/jcr/06_Coroutine_CoBasic.cpp`
4. `Projects/jcr/Sources/jcr/06_Coroutine_CoStack.cpp`

---

## 상세 변경 내용

### 06_CoroutineImpl1.h

- `CoStack` 구조체: `stackTier_` 뒤 implicit pad 자리에 `_u32 exceptionPageCount_ = 0` 추가 (sizeof 유지 = 48 bytes)
- `CoStack` 레이아웃 주석 업데이트 (비상 페이지 포함)
- `CoMgr` public에서 `exceptionPageCount_` public 멤버 제거
- `CoMgr::Configure()` 선언 제거
- `CoMgr` public에 Set 함수 4개 추가 (inline):
  - `SetPageInitCount(_u32)`
  - `SetPageGuardCount(_u32)`
  - `SetPageGrowCount(_u32)`
  - `SetExceptionPageCount(_u32)`
- `CoMgr` private: `exceptionPageCount_ = 8` (기본값 8로 설정)

### 06_CoroutineImpl1.cpp

- `CoMgr::Configure()` 구현 제거
- `InitStack()`:
  - effectiveExceptionCount 계산 추가 (스택 크기 기반 클램프: `min(exceptionPageCount_, totalPages - minInfraPages)`)
  - `_pStack->exceptionPageCount_ = effectiveExceptionCount` 저장
  - overflow guard/pOverflowGuardTop 계산을 effectiveExceptionCount 기반으로 변경
- `ExpandStack()`: `exceptionPageCount_` → `_pStack->exceptionPageCount_` 사용
- `CoVEH()` 2곳: `g_cCoMgr.exceptionPageCount_` → `pCtx->stack_.exceptionPageCount_` 사용

### 06_Coroutine_CoBasic.cpp

- `Configure(2, 3, 2)` → `SetPageInitCount(2); SetPageGuardCount(3); SetPageGrowCount(2);` (3곳)
- `Configure(2, 3, 2, 4)` → 3개 Set + `SetExceptionPageCount(4)` (CO14-B 설정)
- `Configure(2, 3, 2, 0)` → `SetExceptionPageCount(8)` (CO14-B 복원, 기본값으로)
- `Test_CoBasic()` 초기화: `SetExceptionPageCount(8)` 명시적 추가
- `Test_CO14()` CO14-A: `SetExceptionPageCount(0)` 추가 (비상 페이지 비활성, 스택 최대 확장 테스트)

### 06_Coroutine_CoStack.cpp

- `Configure(2, 3, _growCount)` → 3개 Set 함수로 분리
- `Configure(60, 3, 0)` → 3개 Set 함수로 분리

---

## 설계 결정 기록

- `exceptionPageCount_` 를 CoStack에 저장: InitStack에서 스택 크기에 맞게 클램프한 유효값을 저장하여, ExpandStack/CoVEH가 per-stack 값 참조
- cstLow(4 pages): minInfra=6 > 4 → effectiveExceptionCount=0 (비상 페이지 자동 비활성)
- cstMid(16 pages) + exceptionPageCount_=8: min(8, 16-6)=8 → usable=7 pages=28KB
- CO14-A: `SetExceptionPageCount(0)` 명시 (RecursiveSum(80) ~40KB가 28KB 한도 초과 방지)
- CO14-B: `SetExceptionPageCount(4)` + `cstMid` 사용
