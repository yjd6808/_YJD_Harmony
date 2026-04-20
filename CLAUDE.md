## Docs
1. Project Architecture: .claude/docs/project_architecture.md

## Coding Conventions
1. C++ coding convention: .claude/rules/coding-conventions_cpp.md
2. C# coding convention: .claude/rules/coding-conventions_csharp.md
3. SQL coding convention: .claude/rules/coding-conventions_sql.md

## Basic Rules
1. 최대한 모듈화 가능하도록 코드 작업
2. 파일이 5개 이상 수정이 필요한 경우 수행 전에 반드시 나에게 확인 (단, Projects/SteinsGate-Tools.* 프로젝트는 제외)
3. 무조건 한국어로 답변
4. AI로 생성된 파일에는 모두 주석으로 "created by AI." 추가할 것. 단, 다른 자료로부터 복사한 내용에 대해서는 기입하지 말 것
5. 소스, 헤더, 텍스트 파일들은 모두 UTF8 인코딩

## Build
- `./Scripts/Build.ps1` 스크립트를 사용하여 빌드한다.
```
./Scripts/Build.ps1 -ProjectName <프로젝트명> [-Configuration|-C Debug|Release] [-Platform|-P x64|x86] [-Rebuild] [-Clean]
```
- `-Rebuild` 와 `-Clean` 은 동시에 사용할 수 없다.
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

