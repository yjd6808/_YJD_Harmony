# OverflowGuardPage 작업 이력

## 날짜
2026-04-26

## 작업 목적
스택 가드 페이지가 모두 소진되었을 때 `STATUS_ACCESS_VIOLATION`으로 크래쉬하는 문제를 해결.
`pStackEnd_` 위치에 영구 오버플로우 가드 페이지를 두고, 해당 페이지 터치 시 `STATUS_STACK_OVERFLOW`로 변환하여 감지 가능하게 한다.

## 원인 분석
- 가드 페이지가 모두 소진되면 RSP가 `pStackEnd_` 아래 RESERVE(PAGE_NOACCESS) 영역으로 진입
- `STATUS_ACCESS_VIOLATION`(0xC0000005) 발생
- `CoVEH` 첫 줄 `ExceptionCode != STATUS_GUARD_PAGE_VIOLATION` 조건에서 즉시 `CONTINUE_SEARCH` → 잡히지 않고 프로세스 크래쉬

## 수정 내용

### 파일: `06_CoroutineImpl1.cpp`

#### InitStack
- `pStackEnd_` ~ `pStackEnd_ + CO_PAGE_SIZE` 범위를 오버플로우 가드 페이지로 `MEM_COMMIT | PAGE_GUARD` 커밋
- init/guard 클램프를 기존 `pStackEnd_` 기준에서 `pOverflowGuardTop = pStackEnd_ + CO_PAGE_SIZE` 기준으로 변경

#### ExpandStack
- 함수 진입 초반에 `pOverflowGuardTop = pStack->pStackEnd_ + CO_PAGE_SIZE` 계산
- `_pFaultAddr < pOverflowGuardTop` 이면 즉시 `false` 반환 (오버플로우 가드 터치 감지)
- `pGrowthStart`, `pNewGuardStart` 클램프를 `pStackEnd_` → `pOverflowGuardTop`으로 변경

#### CoVEH
- `ExpandStack` 반환값이 `false`이고 `pFaultAddr < pCtx->stack_.pStackEnd_ + CO_PAGE_SIZE` 이면:
  - `ExceptionCode = STATUS_STACK_OVERFLOW`
  - `ExceptionFlags |= EXCEPTION_NONCONTINUABLE`
  - `EXCEPTION_CONTINUE_SEARCH` 반환
- 재귀 진입 경로(`s_inCoVEH == true`)에서도 동일하게 오버플로우 가드 감지 후 `STATUS_STACK_OVERFLOW` 변환

## 빌드 결과
- 경고 0개, 오류 0개 (Debug / x64)

## 관련 파일
- `Projects/jcr/Sources/jcr/06_CoroutineImpl1.cpp`
