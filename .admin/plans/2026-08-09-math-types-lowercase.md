# jc/Math.h 타입명 소문자화 계획 (2026-08-09)

## 목표
- `jc/Math.h`의 벡터/행렬/색상 타입명을 소문자로 변경
  - `Vec2` → `vec2`, `Vec3` → `vec3`, `Vec4` → `vec4`, `Mat4` → `mat4`, `Color` → `color`
- 멤버 함수/정적 함수명은 유지 (예: `vec2::Zero()`, `mat4::Translation()`)
- sgf/sgfr 프로젝트의 모든 사용처 반영

## 범위 (사용자 확인 완료)
- jc: 1파일 (Math.h)
- sgf: 10파일
- sgfr: 35파일
- sgcl/sgcc 제외 (cocos2d 자체 타입, jc::Vec* 미사용 확인)

## 주요 사실
- 이전 작업(2026-08-09-sgf-math-to-jc.md)에서 SgfMath.h → jc/Math.h 이관 완료
- sgf는 `using namespace jc;` 로 비한정 사용 (18파일), sgfr은 `using namespace jc;` (33파일)
- `Color` 부분 문자열 포함 식별자(clearColor, SetColor 등)는 단어 경계(`\bColor\b`)로 오매치 방지

## 작업 단계
1. [x] jc/Math.h 타입명 치환 (Vec2→vec2, Vec3→vec3, Vec4→vec4, Mat4→mat4, Color→color)
2. [x] sgf 10파일 타입명 치환
3. [x] sgfr 35파일 타입명 치환
4. [x] 남은 `Vec2|Vec3|Vec4|Mat4|\bColor\b` 참조 0건 확인 (jc/sgf/sgfr 한정)
5. [x] 빌드 검증: jc / sgf / sgfr (Debug x64) — 0 error / 0 warning
6. [x] Docs/sgf, Docs/sgfr 문서 13파일 타입명 치환 (사용자 승인)
7. [x] /done 히스토리 작성

## 치환 중 수정된 코드 (타입명-변수명 충돌)
- sgfr/13_Input_Main.cpp: 로컬 변수 `color color` → `color tintColor` (3곳 + 정점 4곳)
- sgfr/14_SceneGraph_Main.cpp: `const color color(...)` → `const color ballColor(...)` (1곳)

## 검증
- 전 워크트리 grep (jc/sgf/sgfr 한정)에서 대문자 타입명 0건
- Docs/sgf, Docs/sgfr 대문자 타입명 0건
- jc/sgf/sgfr Debug x64 빌드 성공 (0 error / 0 warning)
