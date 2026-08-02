# Plan: WPF 단색 테마 시스템 적용 (2026-08-02)

## 목표
`wpf_solid_theme_update.zip` + `WPF_Theme_Color_System_Final.md` 내용을
Harmony sgcl 프로젝트 (`Projects\sgcl\Classes\sgcl\Game\UI`)에 적용하고,
빌드 + Log 기반 테스트로 검증한다.

## 작업 단계
1. ZIP 소스 27개 파일을 `Projects\sgcl\Classes\sgcl\Game\UI`에 복사(덮어쓰기)
2. 신규 헤더 2개 (`Theme/UIThemeColor.h`, `Theme/UIThemeColorTable.h`)를 `sgcl.vcxproj`/`.filters`에 추가
3. `Scripts\BuildProject\sgcl.bat -P x64 -C Debug` 빌드 → 오류 수정 (반복)
4. Log 기반 테스트:
   - 앱 실행 → 15초 대기 → 종료 → 로그 확인 (`Scripts\start_up_debugging.ps1`)
   - 테마 적용/상태 전환/글자색 상속 관련 로그 확인
5. 진행상황 MD 문서 작성/갱신

## 파일 목록 (ZIP → 프로젝트 매핑)
- `src/sgcl/Game/UI/...` → `Projects\sgcl\Classes\sgcl\Game\UI\...`
- `res/theme/*.json` → 사용자가 이미 `UltimateHarmony_Dev\assets\res_data\theme`에 복사함 (79개 확인)

## 참고
- 빌드는 x64만 (Debug 기본)
- 신규 파일: UIThemeColor.h, UIThemeColorTable.h (vcxproj에 추가 필요)
