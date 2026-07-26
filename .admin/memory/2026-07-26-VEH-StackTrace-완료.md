# Memory: VEH + StackTrace 구현 완료 및 검증

## 최종 상태
- `InitializeJCore`에 VectoredExceptionHandler 등록 완료
- `StackTrace` 클래스 구현 완료 (DbgHelp 기반 심볼 리졸브)
- **심볼 리졸브 정상 동작 확인** — sgcl.exe 함수명 + 소스파일 + 라인번호까지 출력됨

## 변경 파일 (최종)
1. **`Projects/jc/Sources/jc/Utils/StackTrace.h`** — StackTrace 클래스 선언
2. **`Projects/jc/Sources/jc/Utils/StackTrace.cpp`**
   - `BuildSearchPath()`: EXE 경로 + 현재 디렉토리 + `_NT_SYMBOL_PATH` 자동 탐색
   - `SymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS)` 설정
   - `SymInitialize()` 실패시 `GetLastError()` 로깅
   - `SymLoadModule64()`로 EXE 모듈 명시적 로드
   - `Print()`에서 `SymFromAddr` 실패시 `error=GetLastError()` 출력
   - `Capture(CONTEXT*)`: `StackWalk64` 기반 (x64 정상 동작 확인)
3. **`Projects/jc/Sources/jc/Config.cpp`**
   - `IsFatalException()` 필터: AV/STACK_OVERFLOW/ILLEGAL_INSTRUCTION 등만 처리
   - Context 기반 + CaptureStackBackTrace 기반 이중 스택 출력
   - Debug 빌드에서 `TerminateProcess()` 호출
4. **`Projects/jc/Sources/jc/Utils/Console.cpp`**
   - `ReadLine()`: `std::cin.good()` 체크 + 실패시 `clear()` 추가

## 검증 결과
- MSVC 스레드명 예외(`0x406D1388`) 정상 필터링 → 프로그램 계속 실행
- `0xC0000005` (AV) 정상 캐치 → 풀 스택트레이스 출력
- 실제 크래시 위치: `UIGroup::InitFromXml` (UIGroup.cpp:95) — `groupInfo_`가 `0xFFFFFFFFFFFFFFFF` (INVALID_HANDLE_VALUE)
- 기존 추정이었던 `Console::ReadLine` crash는 잘못된 판단 (심볼 없던 시절의 추정)

## 빌드 방법
```
cd Scripts
.\Build.ps1 -ProjectName jc -Configuration Debug -Platform x64   # 또는 Rebuild
.\Build.ps1 -ProjectName sgcl -Configuration Debug -Platform x64   # Build (lnk만)
```

## 테스트
```
cd C:\Users\jdyun\Desktop\UltimateHarmony_Dev\release\sgcl
.\sgcl.exe "assets=C:\Users\jdyun\Desktop\UltimateHarmony_Dev\assets mode=1 auth_ep=127.0.0.1:10110"
```

## 남은 버그 (본 작업 범위 아님)
- `UIGroup::InitFromXml` crash: `groupInfo_` 가 `0xFFFFFFFFFFFFFFFF`. UI XML 로딩/초기화 문제로 추정
- 이전 작업에서 추정한 Console::ReadLine crash는 허위였음

## 배포
- EXE + PDB가 `UltimateHarmony_Dev/release/sgcl/`로 자동 복사됨 (PostBuildEvent)
- PDB 복사되어야 VEH에서 심볼 리졸브 가능
