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
4. msbuild를 활용해서 프로젝트를 빌드할 것

## Workflow
1. 기존 코드를 먼저 분석
2. 수정 계획을 세움
3. 파일 수정
4. 완료 후 빌드를 한 번 수행

## Context Window 관리
1. 꽉 찰 경우: `memory/memory_{작업명}.md`에 작업 상태를 저장하고 다음 세션이 이어서 진행할 수 있도록 남긴 후 응답을 종료