# Done: sgf/sgfr 프로젝트 props 중앙 집중화 (Directory.Build.props 전환)

## 날짜
2026-08-15

## 작업 내용

### 변경 파일 (3개)
1. **`Directory.Build.props`** (루트)
   - `base.props` import 조건에 `'sgf'`, `'sgfr'` 추가 (24~25행)
   - `jc.props` import 조건에 `'sgf'`, `'sgfr'` 추가 (89~90행)
   - `sgf.props` import 블록 신설 (113~116행) — 조건 `'sgfr'`만 (기존 동작 유지: sgf.props는 sgf.lib 소비자용)

2. **`Projects/sgf/Project/sgf.vcxproj`**
   - PropertySheets ImportGroup 4곳의 `$(SolutionDir)Props\base.props`, `jc.props` 명시 import 제거 (8줄)

3. **`Projects/sgfr/Project/sgfr.vcxproj`**
   - PropertySheets ImportGroup 4곳의 `$(SolutionDir)Props\base.props`, `jc.props`, `sgf.props` 명시 import 제거 (12줄)

이제 `$(SolutionDir)Props\*.props`를 vcxproj에 직접 import하는 프로젝트는 없음 (Projects/ 전체 검증 완료).

## 검증
- `Scripts\BuildProject\sgfr.bat -C Release` (x64) 빌드 성공 — jc → sgf → sgfr 체인 전부 OK
- 로그(sgs-20260815-123708.log)에서 props 적용 확인:
  - sgfr 컴파일: nanosvg/tinyxml2/sgf Sources include (sgf.props), jc Sources (jc.props), `..\Sources`/PCH/Classes (base.props), `/FICore.h` (base.props), `/wd4819` 등
  - sgfr 링크: `sgf.lib tinyxml2.lib jc.lib`, LIBPATH `ThirdParty\tinyxml2\lib\x64\Release` + `Output\x64\Release`
  - `DebugProps` 타겟 메시지 출력 (base.props import 확인)

## 비고
- props 3종 모두 추가형(AdditionalIncludeDirectories/AdditionalDependencies/ForcedIncludeFiles, `%(...)` 병합)이라 평가 순서 변화의 영향 없음
- sgcl/sge가 가진 `Cocos2d-x\cocos\2d\cocos2dx.props` import와 jc_gtest/jnet_gtest의 NuGet googletest import는 이번 작업 범위 아님 (별도 판단 필요)
- mcpr은 이미 중앙 집중 방식이었음 (변경 없음)