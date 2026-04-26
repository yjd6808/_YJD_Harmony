# Plan: jcr 06_Coroutine 파일 추가

**날짜:** 2026-04-21  
**작업명:** jcr-06-Coroutine-추가

## 목표

jcr 프로젝트에 코루틴 학습용 파일 추가:
- `06_Coroutine1.cpp` — `call_06_Coroutine` 진입 함수만 포함
- `06_Coroutine2.asm` — `co_run proc`만 포함 (`option casemap: NONE` + `include` 포함)
- `main.cpp` — 6번 메뉴 항목 추가
- `Core.h` — `call_06_Coroutine` 선언 추가
- `jcr.vcxproj` — 두 파일 등록

## 작업 내용

### 1. `06_Coroutine1.cpp`
- `#include "Core.h"`
- `extern "C" { void co_run(); }` 선언
- `call_06_Coroutine(int _argc, char** _argv)` 함수 구현 (co_run() 호출)

### 2. `06_Coroutine2.asm`
- `option casemap: NONE`
- `include ../../jc/Sources/jc/_Extern/Extern.asm`
- `.code` 섹션에 `co_run proc` / `co_run endp`

### 3. `Core.h`
- `int call_06_Coroutine(int _argc, char** _argv);` 선언 추가

### 4. `main.cpp`
- `pRoot->Add("6", "Coroutine - 코루틴 학습", ...)` 추가

### 5. `jcr.vcxproj`
- `<ClCompile Include="..\Sources\jcr\06_Coroutine1.cpp" />`
- `<MASM Include="..\Sources\jcr\06_Coroutine2.asm">` 추가

## 파일 수정 목록
1. `.admin/plans/2026-04-21-jcr-06-Coroutine-추가.md` (신규)
2. `Sources/jcr/06_Coroutine1.cpp` (신규)
3. `Sources/jcr/06_Coroutine2.asm` (신규)
4. `Sources/jcr/Core.h` (수정)
5. `Sources/jcr/main.cpp` (수정)
6. `Project/jcr.vcxproj` (수정)
