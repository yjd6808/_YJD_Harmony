# History: 06-Coroutine-to-04-CoStack-통합

**날짜:** 2026-04-25  
**작업명:** 06-Coroutine-to-04-CoStack-통합

## 작업 요약

`06_Coroutine`(asm 컨텍스트 스위칭)과 `04_CoStack`(VEH 기반 확장 가능 커스텀 스택)을 결합하여,  
`call_04_CoStack` 메뉴 3번 항목 `Test_CoStack()`을 구현하고, 스택 방향 버그(위→아래 성장)를 전면 수정.

## 수정된 파일

- `Projects/jcr/Sources/jcr/04_CoStack.cpp`

## 주요 변경 내용

### 1. 스택 방향 버그 수정 (아래쪽 방향으로 성장)
- `StackAlloc`: 스택 top(높은 주소)에서 아래 방향으로 커밋/가드 페이지 배치
- `StackInit`: 동일하게 top 기준 아래 방향으로 재초기화
- `VectoredHandler`: GUARD 트리거 시 `pPage - pageSize` (아래 방향)으로 확장
- `DumpPages`: 높은 주소 → 낮은 주소 순으로 출력
- `Test_StackExpansion` / `Test_StackExpansionPerf`: `pTop - pageSize - 100` 기준으로 터치

### 2. `Test_CoStack` 구현
- VEH 핸들러 등록 → `StackAlloc` → `RegisterStack` → `co_run(co_func1, pStack + reserveSize)` 호출
- 커스텀 스택 위에서 코루틴 함수 실행 검증
- 실행 후 `UnregisterStack` → `StackFree` → `RemoveVectoredExceptionHandler` 정리

### 3. 코루틴 테스트 함수 추가
- `co_func2(int _depth)`: 재귀적으로 스택 4KB 배열 사용 (최대 3depth)
- `co_func1()`: `co_func2` 호출 후 로컬 변수 출력

### 4. ASM 함수 선언 추가
- `extern "C" void co_run(FN_COROUTINE, void*)` 선언 (04_CoStack.asm에서 구현)

### 5. `ManagedStack` 멤버 이름 변경
- `base` → `base_`, `reserveSize` → `reserveSize_`, `pageSize` → `pageSize_` (코딩 컨벤션)

## 주의사항 / 미완성 사항

- `co_func2` 내부 `char stk[4096 * 3]`에서 컴파일러 `__chkstk` 호출로 인한 stack probing 이슈 존재
  - 커스텀 스택에서 실행 시 commit되지 않은 페이지 접근 → 크래시 가능성
  - VEH 기반 on-demand 확장과 chkstk probing의 충돌 문제 → 추후 연구 필요
- `CoroutineMgr` 클래스는 스켈레톤 수준으로 추가됨 (미완성)
