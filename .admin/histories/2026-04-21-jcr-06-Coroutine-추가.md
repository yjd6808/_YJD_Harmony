# History: jcr 06_Coroutine 파일 추가

**날짜:** 2026-04-21  
**작업명:** jcr-06-Coroutine-추가  
**결과:** 완료 (빌드 성공, 경고 0 / 오류 0)

## 변경 파일 목록

| 파일 | 종류 | 내용 |
|------|------|------|
| `Projects/jcr/Sources/jcr/06_Coroutine1.cpp` | 신규 | `call_06_Coroutine` 함수 (co_run() 호출) |
| `Projects/jcr/Sources/jcr/06_Coroutine2.asm` | 신규 | `co_run proc` (option casemap: NONE + include Extern.asm) |
| `Projects/jcr/Sources/jcr/Core.h` | 수정 | `call_06_Coroutine` 선언 추가 |
| `Projects/jcr/Sources/jcr/main.cpp` | 수정 | 메뉴 6번 "Coroutine - 코루틴 학습" 추가 |
| `Projects/jcr/Project/jcr.vcxproj` | 수정 | ClCompile, MASM 항목 등록 |

## 상세 내용

### 06_Coroutine1.cpp
- `extern "C" { void co_run(); }` 선언
- `call_06_Coroutine` 함수에서 `co_run()` 호출

### 06_Coroutine2.asm
- `option casemap: NONE`
- `include ../../jc/Sources/jc/_Extern/Extern.asm`
- `co_run proc` / `co_run endp` (빈 구현, 이후 확장 예정)

### main.cpp
```cpp
pRoot->Add("6", "Coroutine - 코루틴 학습",
    CONSOLE_MENU_ACTION(call_06_Coroutine(_argc, _argv))
);
```
