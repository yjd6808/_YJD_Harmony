# Plan: 06-Coroutine-to-04-CoStack-통합

**날짜:** 2026-04-25  
**작업명:** 06-Coroutine-to-04-CoStack-통합

## 목표

`06_Coroutine`(asm 컨텍스트 스위칭)과 `04_CoStack`(VEH 기반 확장 가능 커스텀 스택)을 결합하여,  
`call_04_CoStack` 메뉴 3번 항목인 `Test_CoStack()`을 구현한다.

현재 상태:
- `04_CoStack.cpp` 메뉴 3번에 `Test_CoStack(_argc, _argv)` 호출이 있으나 함수 정의 없음 → 빌드 에러
- `06_Coroutine1.cpp`에 `co_run_1(fn, stackTop)` 단순 호출 데모가 있음
- `06_Coroutine2.asm`에 `co_run_1 proc` 구현: TEB StackBase/StackLimit 교체 + 커스텀 스택에서 코루틴 실행

## 구현 내용: `Test_CoStack`

### 목표 동작
1. CoStack(VEH 기반 확장 가능 스택) 할당
2. 그 스택 위에서 코루틴 함수 실행 (`co_run_1` 사용)
3. 코루틴 함수 내에서 스택을 깊게 사용 → VEH가 자동 확장 처리 검증
4. 실행 완료 후 CoStack 해제

### 구체적인 구현 계획

```cpp
static void Test_CoStack(int _argc, char** _argv)
{
    // 1. 설정
    constexpr size_t RESERVE_SIZE = 16 * 4096;
    size_t pageSize = GetPageSize();

    // 2. VEH 핸들러 + CoStack 준비
    void* pVeh = AddVectoredExceptionHandler(1, VectoredHandler);
    char* pStack = StackAlloc(RESERVE_SIZE, pageSize);
    RegisterStack(pStack, RESERVE_SIZE, pageSize);
    
    // 3. 상태 출력
    DumpPages(pStack, RESERVE_SIZE, pageSize, "Before co_run_1");
    
    // 4. 코루틴 함수 실행 (커스텀 스택 사용)
    co_run_1(코루틴함수, pStack + RESERVE_SIZE);
    
    // 5. 실행 후 상태 출력 (스택이 자동 확장됐는지 확인)
    DumpPages(pStack, RESERVE_SIZE, pageSize, "After co_run_1");
    
    // 6. 정리
    UnregisterStack(pStack);
    StackFree(pStack);
    RemoveVectoredExceptionHandler(pVeh);
}
```

코루틴 함수는 `co_func1` / `co_func2` (이미 `06_Coroutine1.cpp`에 정의)를 재사용하거나 유사한 깊은 재귀 함수를 `04_CoStack.cpp`에 로컬로 정의.

## 파일 수정 목록

1. `Projects/jcr/Sources/jcr/04_CoStack.cpp` — `Test_CoStack` 함수 구현 추가
   - `co_run_1` extern 선언 추가
   - 로컬 코루틴 테스트 함수(스택 깊게 사용) 정의
   - `Test_CoStack` 함수 구현

## 수정 파일 수
- 1개 (04_CoStack.cpp만 수정)

## 주의사항
- `co_run_1`은 `06_Coroutine2.asm`에 정의된 extern "C" 함수
- TEB StackBase/StackLimit을 커스텀 스택으로 교체하므로, VEH 핸들러가 반드시 등록되어 있어야 함
- `co_func1`, `co_func2`는 `06_Coroutine1.cpp`에 이미 있으므로 중복 정의 금지
  → `04_CoStack.cpp`에 별도의 테스트용 재귀 함수를 로컬로 정의하거나, extern 선언으로 참조
