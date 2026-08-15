# Plan: sgf/sgfr 프로젝트 props 중앙 집중화 (Directory.Build.props 전환)

## 날짜
2026-08-15

## 작업 배경
현재 props 적용 방식이 두 갈래로 나뉘어 있음:
- **중앙 집중**: Directory.Build.props (루트)에서 프로젝트 이름 조건으로 base/jc/mcp 등 props 자동 import (sgcl, mcpr 등)
- **명시 import**: 프로젝트 vcxproj의 PropertySheets ImportGroup에 `$(SolutionDir)Props\*.props` 직접 import (sgf, sgfr만 잔존)

아래 두 가지 문제 때문에 전환 수행:
1. 구성 4곳(Win32/x64 × Debug/Release)마다 import가 중복 기술 → 누락/불일치 위험
2. sgf/sgfr은 Directory.Build.props 조건에 없어 중앙 관리에서 누락됨

## 작업 내용

### 1. Directory.Build.props (루트) 수정
- `base.props` import 조건에 `'sgf'`, `'sgfr'` 추가
- `jc.props` import 조건에 `'sgf'`, `'sgfr'` 추가
- `sgf.props` import 블록 신설 — 조건: `'sgfr'`만 (sgf.props는 sgf.lib 소비자용: nanosvg/tinyxml2 include, sgf.lib/tinyxml2.lib 링크 → sgf 자기 자신은 제외, 기존 동작 그대로)

### 2. sgf.vcxproj 수정
- PropertySheets ImportGroup 4곳에서 `base.props`, `jc.props` import 제거 (8줄)

### 3. sgfr.vcxproj 수정
- PropertySheets ImportGroup 4곳에서 `base.props`, `jc.props`, `sgf.props` import 제거 (12줄)

## 검증
- props 3종 모두 추가형(additive) ItemDefinitionGroup만 보유 (`AdditionalIncludeDirectories`/`AdditionalDependencies`/`ForcedIncludeFiles`, 전부 `%(...)` 병합) → OutDir/IntDir/러타임 등 기존 값 충돌 없음 (전체 grep으로 확인 완료)
- 전환 후 `Scripts\BuildProject\sgf.bat -C Release` 로 sgf→sgfr 체인 빌드 성공 확인
- Directory.Build.props는 Microsoft.Cpp.props 내부(프로젝트 본문 이전)에서 import되나, props가 정의하는 값은 전부 추가형이라 평가 순서 변화의 영향 없음

## 예상 변경 파일 (3개)
- `Directory.Build.props`
- `Projects/sgf/Project/sgf.vcxproj`
- `Projects/sgfr/Project/sgfr.vcxproj`