# jcr 07_FuturePromise_Practice 파일 추가 계획

- 날짜: 2026-10-18
- 대상: `Projects/jcr/`

## 1. 배경 및 목적
- jcr 학습 프로젝트에 `07_FuturePromise_Practice.cpp`(future/promise 학습용)를 추가한다.
- 코드는 아직 작성하지 않고 `Console::WriteLine`으로 "Hello World"만 출력하는 자리표시자로 둔다. 실제 학습 코드는 사용자가 직접 작성한다.
- 파일 추가 시 Visual Studio 프로젝트(`jcr.vcxproj`)와 필터(`jcr.vcxproj.filters`)에 반드시 등록하고, 메뉴(main.cpp 7번)에 연결해 실행 가능한 상태로 만든다.
- 재발 방지를 위해 루트 `AGENTS.md`에 "cpp/h/inl 등 소스 파일 추가 시 프로젝트 파일·필터 등록 필수" 규칙을 추가한다.

## 2. 기존 코드 현황 (사전 분석)
- jcr은 `main.cpp`에서 `ConsoleMenuItem` 루트 메뉴를 구성하고, 각 실습 파일이 `call_0X_이름(int _argc, char** _argv)` 함수를 제공하는 구조다.
- 기존 실습: 01_AsmStudy1(call_01_AsmStudy), 02_HeapStudy1(call_02_HeapStudy), 03_RSPGame(call_03_RSPGame), 04_CoStack1(call_04_CoStack), 05_PageGuard(call_05_PageGuard), 06_Coroutine(call_06_Coroutine, 서브메뉴).
- `call_0X` 함수 원형은 `Core.h`의 "Test Functions" 섹션에 선언된다.
- 각 실습 파일은 `#include "Core.h"` 후 `call_0X` 함수에서 `Console::WriteLine(...)`으로 출력하고 `int`를 반환한다 (05_PageGuard.cpp 참조).
- 메뉴 등록은 `pRoot->Add(_T("번호"), _T("설명"), CONSOLE_MENU_ACTION(call_0X(_argc, _argv)))` 형태.
- 인코딩 현황: cpp 일부 BOM/일부 noBOM, 전부 LF. vcxproj·filters는 noBOM LF, Core.h는 BOM LF.

## 3. 구현 계획
1. `Sources/jcr/07_FuturePromise_Practice.cpp` 신규 생성 — `#include "Core.h"`, `int call_07_FuturePromise(int _argc, char** _argv)`에서 `Console::WriteLine(_T("Hello World"));` 후 `return 0;`.
2. `Core.h` Test Functions 섹션에 `int call_07_FuturePromise(int _argc, char** _argv);` 선언 추가.
3. `main.cpp` 루트 메뉴에 7번 항목 추가 — 코루틴 서브메뉴 블록 뒤(표시 순서 7), `pRoot->Add(_T("7"), _T("FuturePromise Practice - future/promise 학습"), CONSOLE_MENU_ACTION(call_07_FuturePromise(_argc, _argv)))`.
4. `Project/jcr.vcxproj` ClCompile 목록에 `..\Sources\jcr\07_FuturePromise_Practice.cpp` 등록.
5. `Project/jcr.vcxproj.filters` ClCompile 목록에 동일 파일 등록.
6. 루트 `AGENTS.md` Constraints에 "C++ 소스 파일(.cpp/.h/.inl 등) 추가 시 .vcxproj 및 .vcxproj.filters 등록 필수" 규칙 추가.

## 4. 예상 변경 파일 목록
- 신규: `Projects/jcr/Sources/jcr/07_FuturePromise_Practice.cpp`
- 수정: `Projects/jcr/Sources/jcr/Core.h`, `Projects/jcr/Sources/jcr/main.cpp`, `Projects/jcr/Project/jcr.vcxproj`, `Projects/jcr/Project/jcr.vcxproj.filters`, `AGENTS.md` (루트, 5개)
- 신규: `.admin/plans/2026-10-18-jcr-07-FuturePromise-Practice-파일추가.md`

## 5. 검증
- `Scripts\BuildProject\jcr.bat -C Debug -P x64` 빌드 통과 확인
- 빌드 후 실행 시 루트 메뉴 7번에서 "Hello World" 출력 확인(사용자 실행)
