# 작업 완료: jc/Math.h 타입명 소문자화 (2026-08-09)

## 완료 요약
- `jc/Math.h`의 벡터/행렬/색상 타입명을 소문자로 변경
  - `Vec2` → `vec2`, `Vec3` → `vec3`, `Vec4` → `vec4`, `Mat4` → `mat4`, `Color` → `color`
- 멤버 함수/정적 함수명은 유지 (예: `vec2::Zero()`, `mat4::Translation()`, `color::White()`)
- sgf/sgfr 프로젝트 및 Docs 문서의 모든 사용처 반영

## 변경 파일
### 코드 (총 57파일)
- jc: 2파일 (Math.h, Container/TreeTable_Node.h)
- sgf: 14파일 (Core/Graphics/Scene 등)
- sgfr: 41파일 (튜토리얼 전체)

### 문서 (13파일)
- Docs/sgf, Docs/sgfr의 마크다운 13파일

## 치환 중 수정된 코드 (타입명-변수명 충돌)
`Color` → `color` 소문자화로 인해 타입명과 로컬 변수명이 겹친 2곳:
- `sgfr/13_Input/13_Input_Main.cpp`: `color color` → `color tintColor`
- `sgfr/14_SceneGraph/14_SceneGraph_Main.cpp`: `color color` → `color ballColor`

## 검증
- jc / sgf / sgfr Debug x64 빌드: **0 에러 / 0 경고**
- jc/sgf/sgfr 코드 + Docs/sgf, Docs/sgfr 문서에서 대문자 타입명(`Vec2/3/4, Mat4, Color`) 참조 0건
- 범위 외 확인: sgcl/sgcc/sg는 cocos2d 자체 타입(cocos2d::Vec2 등) 사용 → 미변경
- HLSL 시맨틱(`COLOR0`, `COLOR`), 메서드명(`SetClearColor` 등)은 타입명이 아니므로 미변경 (의도)

## 메모
- 치환 시 UTF-8/BOM 보존 처리 (PowerShell에서 -ne 대신 -cne 사용 필수 — 대소문자 구분 비교)
- 단어 경계(`\b...\b`) + 대소문자 구분(`-creplace`)으로 부분 문자열 오매치 방지
