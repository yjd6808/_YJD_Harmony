# Plan: CoVEH 스택 확장 로직 구현

## 날짜
2026-04-25

## 목표
`06_CoroutineImpl1.cpp`의 `CoVEH` 함수를 `g_cCoAllocator.pageInitCount_` / `pageGuardCount_`를 기반으로 완성한다.

## 현황 분석

### CoStackAllocator::Init 이후 메모리 레이아웃
```
pStackBase_    ──── (high addr, 초기 RSP)
 [pageInitCount_ pages: COMMITTED, RW]  ← 2 pages = 8 KB
pStackLimit_   ──── (일반 커밋 영역 하단 = 가드 존 상단)
 [pageGuardCount_ pages: COMMITTED, PAGE_GUARD]  ← 3 pages = 12 KB
pGuardBottom   ──── (가드 존 하단 = 예약 영역 상단)
 [RESERVED]
pStackEnd_     ──── (low addr, 예약 영역 하단)
```

### 기존 CoVEH 문제점
1. `pStack->base_` 필드가 존재하지 않음 → `pStackEnd_` 이어야 함
2. `pageGuardCount_` 미사용 → 가드 페이지 1개만 하드코딩
3. 나머지 guard 페이지(pPage 외) GUARD 미해제
4. `pStackLimit_` 갱신 없음
5. `pageInitCount_`, `pageGuardCount_`가 private → 접근 방법 필요

## 설계

### STATUS_GUARD_PAGE_VIOLATION 발생 시 처리 흐름

가드 페이지 중 하나가 터치되면 (`pPage` ∈ `[pStackLimit_ - pageGuardCount_*PAGE, pStackLimit_)`):

```
[Before]
pStackLimit_
 GUARD page 0   ← pPage (OS가 GUARD 해제)
 GUARD page 1   ← 나머지 GUARD 유지
 GUARD page 2
pGuardBottom
 (reserved)
pStackEnd_

[After]
pStackLimit_ (old)
 RW page 0     ← 기존 가드 전체 RW로 전환
 RW page 1
 RW page 2
pStackLimit_ (new) = old pGuardBottom
 NEW GUARD 0   ← pageGuardCount_ 페이지 새로 commit + PAGE_GUARD
 NEW GUARD 1
 NEW GUARD 2
 (reserved)
pStackEnd_
```

### 오버플로우 체크
- 소프트 오버플로우: `pNewGuardStart < pStackEnd_ + pageInitCount_ * PAGE_SIZE`
  → 새 가드 아래 최소 pageInitCount_ 페이지 여유 없음 (경고)
- 하드 오버플로우: `pNewGuardStart < pStackEnd_`
  → 예약 영역 초과, 확장 불가

### ExpandStack 메서드 추가
`pageInitCount_`, `pageGuardCount_`가 private이므로, `CoStackAllocator`에 `ExpandStack` 메서드를 추가하여 CoVEH에서 호출.

## 수정 파일
- `Projects/jcr/Sources/jcr/06_CoroutineImpl1.cpp` (1개)
