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
1. 기존 코드를 먼저 분석
2. 수정 계획을 세움
   - ./.admin/plans/{날짜}-{작엄이름}.md 문서로 저장할 것
3. todo list를 만들고 순차적으로 수행
4. 완료 후 빌드 수행
   1. 작업 최종 완료 후
      - ./.admin/histories/{날짜}-{작업이름}.md 문서로 저장할 것
   2. 작업 중단 후
      - ./.admin/memroy/{날짜}-{작업이름}.md 문서로 저장하여 해당 문서만 보고 다시 이어서할 수 있도록 할 것

