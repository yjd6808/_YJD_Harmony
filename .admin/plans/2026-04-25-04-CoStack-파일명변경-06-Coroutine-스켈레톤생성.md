# Plan: 04-CoStack 파일명변경 + 06-Coroutine 스켈레톤 생성

**날짜:** 2026-04-25  
**작업명:** 04-CoStack-파일명변경-06-Coroutine-스켈레톤생성

## 배경

- 현재 `04_CoStack.cpp` / `04_CoStack.asm`에 코루틴 관련 로직(co_run, CoroutineMgr 등)이 혼재
- 향후 코루틴 연구를 독립 파일(`06_Coroutine*.cpp/.asm`)로 분리할 계획
- 이전 작업(`2026-04-21-jcr-06-Coroutine-추가.md`)은 플랜만 작성되고 실제로 수행되지 않은 것으로 보임
- 현재 `04_CoStack.asm`에는 `co_run proc`가 들어 있으나, 이것은 코루틴 전용 로직

## 목표

### Phase 1: 04_CoStack 파일명 변경
- `04_CoStack.cpp` → `04_CoStack.cpp` (이름 유지, 내부 정리)
- `04_CoStack.asm` → **파일명 변경**: `04_CoStack.asm`에서 `co_run`을 제거하고 빈 `.code` 섹션만 남기거나, asm 파일을 삭제/대체
  - 실제로는: `04_CoStack.asm`에서 `co_run proc`를 제거하고, 별도 `06_Coroutine2.asm`으로 이동

### Phase 2: 06_Coroutine 스켈레톤 생성
- `06_Coroutine1.cpp` 신규 생성 — 스켈레톤 수준 (call_06_Coroutine 함수 + CoroutineMgr 클래스 이동)
- `06_Coroutine2.asm` 신규 생성 — `co_run proc` 이동
- `Core.h` — `call_06_Coroutine` 선언 추가
- `main.cpp` — 6번 메뉴 항목 추가
- `jcr.vcxproj` — 새 파일 등록 및 04_CoStack.asm에서 co_run 제거(또는 asm 파일 정리)

## 세부 작업 내용

### 1. `04_CoStack.asm` 정리
- `co_run proc` ~ `co_run endp` 블록 제거
- 빈 `.code` 섹션만 남겨두거나(또는 .data/.code 모두 남겨둠) — 차후 CoStack 전용 ASM이 필요할 경우를 대비

### 2. `04_CoStack.cpp` 정리
- `CoroutineMgr` 클래스 및 `g_cCoMgr` 매크로 → `06_Coroutine1.cpp`로 이동
- `co_func1`, `co_func2`, `co_run` extern 선언 → `06_Coroutine1.cpp`로 이동
- `Test_CoStack`에서 `co_run` 호출 부분: 06_Coroutine으로 이동 후 04_CoStack에서 제거하거나 인터페이스를 통해 유지
  - **판단**: `Test_CoStack` 자체는 `04_CoStack.cpp`에 남기되, co_run은 extern 선언을 `Core.h`나 `04_CoStack.cpp` 상단에 유지

### 3. `06_Coroutine1.cpp` 신규 생성 (스켈레톤)
```cpp
// created by AI.
#include "Core.h"

// co_run ASM 함수 선언
extern "C" { void co_run(void(*)(), void*); }

// CoroutineMgr 클래스 (스켈레톤)
class CoroutineMgr : public jc::SingletonStatic<CoroutineMgr>::TSingleton
{
public:
    char* AllocStack(int _size) { return new char[_size]; }
};
#define g_cCoMgr JC_DECL_SINGLETON_BODY(CoroutineMgr)

// call_06_Coroutine 진입 함수
int call_06_Coroutine(int _argc, char** _argv)
{
    // 추후 코루틴 학습 메뉴 구성 예정
    return 0;
}
```

### 4. `06_Coroutine2.asm` 신규 생성
- `04_CoStack.asm`의 `co_run proc` 블록을 그대로 이동

### 5. `Core.h` 수정
- `call_06_Coroutine(int _argc, char** _argv)` 선언 추가

### 6. `main.cpp` 수정
- `pRoot->Add("6", "Coroutine - 코루틴 학습", CONSOLE_MENU_ACTION(call_06_Coroutine(_argc, _argv)))` 추가

### 7. `jcr.vcxproj` 수정
- `06_Coroutine1.cpp` ClCompile 등록
- `06_Coroutine2.asm` MASM 등록

## 파일 수정 목록

1. `.admin/plans/2026-04-25-04-CoStack-파일명변경-06-Coroutine-스켈레톤생성.md` (이 파일, 신규)
2. `Sources/jcr/04_CoStack.asm` (수정 — co_run proc 제거)
3. `Sources/jcr/04_CoStack.cpp` (수정 — CoroutineMgr/co_func1/co_func2 제거)
4. `Sources/jcr/06_Coroutine1.cpp` (신규)
5. `Sources/jcr/06_Coroutine2.asm` (신규)
6. `Sources/jcr/Core.h` (수정 — call_06_Coroutine 선언 추가)
7. `Sources/jcr/main.cpp` (수정 — 6번 메뉴 추가)
8. `Project/jcr.vcxproj` (수정 — 새 파일 등록)

## 주의사항

- `04_CoStack.cpp`의 `Test_CoStack`에서 `co_run`을 호출하므로,  
  `co_run` extern 선언은 `04_CoStack.cpp` 상단에 그대로 유지
- `CoroutineMgr`과 co_func1/co_func2는 `06_Coroutine1.cpp`로 이동 (04_CoStack.cpp에서 제거)
- `06_Coroutine1.cpp`의 `call_06_Coroutine`은 현재 스켈레톤 수준 (빈 메뉴 or 추후 확장)
- 빌드 후 정상 동작 확인 필요
