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
- `Build.ps1` 스크립트를 사용하여 빌드한다.
- **반드시 `Scripts` 디렉토리로 이동(`cd Scripts`) 후에 실행할 것.**
```
cd Scripts
.\Build.ps1 -ProjectName <프로젝트명> [-Configuration|-C Debug|Release] [-Platform|-P x64|x86] [-Rebuild] [-Clean]
```
- `-Rebuild` 와 `-Clean` 은 동시에 사용할 수 없다.

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
