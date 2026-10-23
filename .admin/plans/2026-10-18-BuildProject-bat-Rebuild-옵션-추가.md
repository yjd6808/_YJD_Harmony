# BuildProject bat 스크립트 Rebuild 옵션 추가 계획

- 날짜: 2026-10-18
- 대상: `Scripts/BuildProject/*.bat`, `Scripts/BuildProject-Multibyte/*.bat` (총 64개)

## 1. 배경 및 목적
- `Scripts/Build.ps1`은 `-Rebuild`(msbuild 전체 재빌드)를 지원하지만, 이를 호출하는 프로젝트별 bat 래퍼들은 `-C`/`-P`만 파싱하고 `-Rebuild`를 전달하지 않는다 (`[WARN] Unknown option: -Rebuild` 발생).
- 문자셋 전환(Unicode ↔ MultiByte) 시 `-Rebuild` 사용이 권장되는데(루트 AGENTS.md Build 항목), bat 래퍼로는 실행할 수 없는 상태다.
- 64개 bat 모두 `-Rebuild` 옵션을 파싱해 `Build.ps1`로 전달하도록 수정한다.

## 2. 기존 코드 현황 (사전 분석)
- 64개 bat은 동일 템플릿이다.
  - 공통: `:parse` 루프에서 `-C`/`-P` 파싱, `:build` 서브루틴에서 `powershell ... Build.ps1 -ProjectName %~1 -Configuration %CONFIG% -Platform %PLATFORM% -FileLog -NoConsoleLogging` 호출.
  - BuildProject(33개): 위 호출 라인 그대로.
  - BuildProject-Multibyte(31개): 동일 라인 끝에 `-Multibyte` 추가.
  - 일부(sgcl/sge/jc_gbench 등)는 cocos2d DLL 존재 검사 등 추가 블록이 있으나 parse/:build 구조는 동일.
- `Build.ps1` 파라미터: `[-Configuration|-C]`, `[-Platform|-P]`, `[-Rebuild]`, `[-Clean]`, `[-FileLog]`, `[-NoConsoleLogging]`, `[-Multibyte]`.
- bat 인코딩: ASCII(noBOM), LF.

## 3. 구현 계획
각 bat 파일에 동일한 3개 치환 적용:
1. `set "PLATFORM=x64"` 다음 줄에 `set "REBUILD="` 기본값 추가 (환경 변수 상속 방지).
2. `:parse` 블록의 `-P` 파싱 라인 다음에 `-Rebuild` 파싱 라인 추가:
   `if /i "%~1"=="-Rebuild" ( set "REBUILD= -Rebuild" & shift & goto :parse )`
3. `:build`의 powershell 호출 라인에서 `-NoConsoleLogging` → `-NoConsoleLogging!REBUILD!` 로 변경 (미지정 시 빈 문자열로 확장되어 기존 동작 유지).
4. 루트 `AGENTS.md` Build 인자 설명에 `[-Rebuild]` 추가.

## 4. 예상 변경 파일 목록
- 수정: `Scripts/BuildProject/*.bat` 33개, `Scripts/BuildProject-Multibyte/*.bat` 31개 (파일당 +2줄/-0줄, 호출 라인 1줄 변경)
- 수정: `AGENTS.md` (Build 인자 설명 1줄)
- 신규: `.admin/plans/2026-10-18-BuildProject-bat-Rebuild-옵션-추가.md`

## 5. 검증
- 치환 전 dry run으로 각 파일의 치환 대상 문자열 개수(각 1개) 확인
- `Scripts\BuildProject\sgcc.bat -Rebuild`, `Scripts\BuildProject-Multibyte\sgcc.bat -Rebuild` 실행하여 로그에 "C++ Rebuild 성공" 확인
- `-Rebuild` 미지정 시 기존 동작(Build) 유지 확인
- `git diff --stat`으로 파일당 변경 줄 수(2줄 추가 + 1줄 변경) 확인
