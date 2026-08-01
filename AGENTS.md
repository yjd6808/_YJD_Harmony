# Harmony 프로젝트 지침

> 이 파일은 기존 `CLAUDE.md`의 내용을 opencode용(`AGENTS.md`)으로 이관한 문서입니다.
> 참고 문서는 `.claude/` → `.opencode/` 경로로 이동했습니다.

## Workflow

**[필수 준수 — 아래 단계를 건너뛰는 것은 절대 금지]**

1. 기존 코드를 먼저 분석
2. **작업 시작 전 `/plan {작업이름}` 커맨드를 반드시 실행할 것**
   - plan 파일 없이 코드 수정을 시작하는 것은 금지
   - 저장 위치: `.admin/plans/{날짜}-{작업이름}.md`
3. todo list를 만들고 순차적으로 수행
4. 완료 후 빌드 수행
5. **작업 종료 시 반드시 아래 중 하나를 수행할 것**
   - 작업 최종 완료: **`/done {작업이름}` 커맨드를 반드시 실행**
     - 저장 위치: `.admin/histories/{날짜}-{작업이름}.md`
   - 작업 중단: `.admin/memory/{날짜}-{작업이름}.md` 직접 작성
     - 해당 문서만 보고 다시 이어서 할 수 있도록 상세히 기록할 것

## Constraints
1. 최대한 모듈화 가능하도록 코드 작업
2. 파일이 5개 이상 수정이 필요한 경우 수행 전에 반드시 나에게 확인 (단, Projects/SteinsGate-Tools.* 프로젝트는 제외)
3. 무조건 한국어로 답변
4. 소스, 헤더, 텍스트 파일들은 모두 UTF8 인코딩으로 저장

## Build
- **`Scripts\BuildProject\{프로젝트명}.bat` 으로 빌드한다.** (프로젝트별 빌드 배치 — 더블클릭 또는 cmd에서 실행)
- 인자: `[-C Debug|Release]` 구성, `[-P x64|x86]` 플랫폼 (기본: Debug / x64)
- ⚠️ **빌드는 x64로만 할 것** (`-P x64`). x86도 빌드 가능은 하지만 배포/실행은 x64 기준이므로 x86으로 빌드하지 않는다.
```
Scripts\BuildProject\sgcl.bat -C Release -P x64
```
- 내부적으로 `Scripts\Build.ps1 -ProjectName ... -Configuration ... -Platform ...`을 호출한다.
- cocos2d 엔진이 필요한 프로젝트는 먼저 해당 구성의 엔진 DLL(`ThirdParty\Cocos2d-x\Output\%PLATFORM%\%CONFIG%\libcocos2d.dll`)이 빌드되어 있어야 한다 (없으면 `Scripts\Build-Cocos.bat`).

## How to Run Projects

**경로 정의** (바탕화면 `sgcl_빌드_실행_가이드.md`와 동일):

| 키 | 정의 | 찾는 방법 |
|---|---|---|
| `SolutionDir` | `Harmony_Full.sln`이 위치한 디렉토리 | 워크트리 루트에서 `Harmony_Full.sln` 검색 |
| `$(SolutionDir)..\UltimateHarmony_Dev\` | 배포/실행 공용 폴더 (`DeployRoot`) | SolutionDir의 부모 디렉토리 안의 `UltimateHarmony_Dev` |

| 프로젝트 | 실행 방법 |
|---|---|
| sgcl | `$(SolutionDir)..\UltimateHarmony_Dev\release\sgcl`에서 실행 — 인자는 **단일 따옴표 문자열**: `"assets=$(SolutionDir)..\UltimateHarmony_Dev\assets mode=1 auth_ep=127.0.0.1:10110"` |
| sgs_auth · sgs_center · sgs_game · sgs_lobby · sgs_query | `$(SolutionDir)..\UltimateHarmony_Dev\release\{프로젝트명}`에서 실행 — 인자: `"assets=$(SolutionDir)..\UltimateHarmony_Dev\assets mode=1"` |

- **실행 인자 규칙** (sgcl 기준):
  - 인자는 반드시 **하나의 따옴표 문자열**로 전달 (`AppConfig::ReadEnvArgs`가 argv[1]만 파싱 — 분리 전달 시 exit code 1)
  - `assets` 누락 시 크래시(0xC0000005), `auth_ep=127.0.0.1:10110`은 sgs_auth 서버가 해당 주소에서 실행 중이어야 함
  - VS 디버거(F5)는 커밋된 `.user` 파일의 `$(SolutionDir)` 매크로로 동일하게 동작
  - 산출물: `$(SolutionDir)Output\x64\Debug\` (구성별 분리)

## Debugging

### 시작 시점 디버깅 (크래시/초기화 실패)

**프로그램 실행 → 15초 대기 → 프로그램 종료 → 남은 로그 확인.**

- **스크립트**: `Scripts\start_up_debugging.ps1 [-ProjectName <프로젝트명>] [-Args <인자 문자열>]` (기본 `-ProjectName sgcl`)
  - 흐름: exe 실행 → 15초 대기 → 프로세스 종료 → 이번 실행 로그(tail 60) 출력
  - 새 로그가 생성되지 않으면 실행 실패 가능성 (assets 경로 / DLL 누락 확인)
  - 실행 위치: `$(SolutionDir)..\UltimateHarmony_Dev\release\{ProjectName}\{ProjectName}.exe`
  - 경로 검증: 배포 폴더 / 실행 폴더 / exe / assets 존재 여부를 실행 전에 확인

## References
1. Project Architecture: .opencode/docs/project_architecture.md
2. C++ coding convention: .opencode/rules/coding-conventions_cpp.md
3. C# coding convention: .opencode/rules/coding-conventions_csharp.md
4. SQL coding convention: .opencode/rules/coding-conventions_sql.md

## Tools

1. **GitDiffer.bat** (`Scripts\GitDiffer.bat`)
   - Git staged/unstaged file diff viewer

## Guidelines

Behavioral guidelines to reduce common LLM coding mistakes. Merge with project-specific instructions as needed.

Tradeoff: These guidelines bias toward caution over speed. For trivial tasks, use judgment.

### 1. Think Before Coding
Don't assume. Don't hide confusion. Surface tradeoffs.

Before implementing:

- State your assumptions explicitly. If uncertain, ask.
- If multiple interpretations exist, present them - don't pick silently.
- If a simpler approach exists, say so. Push back when warranted.
- If something is unclear, stop. Name what's confusing. Ask.

### 2. Simplicity First
Minimum code that solves the problem. Nothing speculative.

- No features beyond what was asked.
- No abstractions for single-use code.
- No "flexibility" or "configurability" that wasn't requested.
- No error handling for impossible scenarios.
- If you write 200 lines and it could be 50, rewrite it.
- Ask yourself: "Would a senior engineer say this is overcomplicated?" If yes, simplify.

### 3. Surgical Changes
Touch only what you must. Clean up only your own mess.

When editing existing code:

- Don't "improve" adjacent code, comments, or formatting.
- Don't refactor things that aren't broken.
- Match existing style, even if you'd do it differently.
- If you notice unrelated dead code, mention it - don't delete it.

When your changes create orphans:

- Remove imports/variables/functions that YOUR changes made unused.
- Don't remove pre-existing dead code unless asked.
- The test: Every changed line should trace directly to the user's request.

### 4. Goal-Driven Execution
Define success criteria. Loop until verified.

Transform tasks into verifiable goals:

- "Add validation" → "Write tests for invalid inputs, then make them pass"
- "Fix the bug" → "Write a test that reproduces it, then make it pass"
- "Refactor X" → "Ensure tests pass before and after"

For multi-step tasks, state a brief plan:

1. [Step] → verify: [check]
2. [Step] → verify: [check]
3. [Step] → verify: [check]

Strong success criteria let you loop independently. Weak criteria ("make it work") require constant clarification.

These guidelines are working if: fewer unnecessary changes in diffs, fewer rewrites due to overcomplication, and clarifying questions come before implementation rather than after mistakes.
