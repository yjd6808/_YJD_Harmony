# SgfMath.h → jc/Math.h 이동 계획 (2026-08-09)

## 목표
- `sgf/Math/SgfMath.h`(헤더 온리, 650줄)의 모든 내용을 `jc/Sources/jc/Math.h`로 이관
- `sgf::Vec2/3/4, Color, Mat4, Pi_v, Deg2Rad, Lerp, Clamp, ...` → `jc::`로 이동
- SgfMath.h 파일 및 sgf 프로젝트 등록 제거, sgf/sgfr의 사용 코드 전면 수정

## 주요 사실 (조사 결과)
- jc/Math.h 현재: `struct Math { Pow, Max, Min, Abs, Clamp }`만 존재 (정적 템플릿, NS_JC_BEGIN)
- SgfMath.h: 상수 4개(Pi/TwoPi/HalfPi/Epsilon) + Deg2Rad/Rad2Deg/FloatEqual/Lerp/Clamp + Vec2/Vec3/Vec4/Color/Mat4
- `_f32`, `_u8` 등 jc 타입 별칭은 전역(비네임스페이스) — 이관 후에도 그대로 사용 가능
- sgf 내부: `namespace sgf { ... }` 안에서 비한정 사용 (예: Camera.h의 Vec3/Mat4)
- sgfr: `using namespace sgf;` 후 비한정 사용 + Function.h 시그니처에 명시 `sgf::Vec2` 등
- SgfMath include 16곳: sgf 헤더 13 + SoundEngine.cpp + sgfr/TutorialCommon.h + SgfHeader.h

## 충돌 검토
- jc 기존 `struct Math`의 `Clamp`(static 멤버)와 이관되는 자유 함수 `jc::Clamp`는 서로 다른 스코프 — 공존 가능
- sgf::로 내보내던 타입명을 jc::로 옮긴 뒤 sgf:는 수학 심볼을 완전히 제거

## 작업 단계
1. [x] jc/Math.h에 전체 내용 이관 (NS_JC_BEGIN, include: jc/Type.h, <cmath>)
   - [수정] 상수/각도변환은 **`jc_math_*` 매크로**로 전환 (사용자 요청):
     `jc_math_pi, jc_math_pi2, jc_math_pi_half, jc_math_pi_div2, jc_math_pi_div4, jc_math_deg2rad, jc_math_rad2deg`
   - `Epsilon_v / FloatEqual / Lerp / Clamp`는 함수·constexpr 유지 (매크로 전환 대상 아님)
2. [x] sgf 15곳 include 교체: `sgf/Math/SgfMath.h` → `jc/Math.h`
3. [x] sgf 내부 파일에 `using namespace jc;` 삽입 (namespace sgf 블록 내, 18개 파일)
4. [x] sgfr Function.h/cpp의 명시 `sgf::Vec2/Color/Mat4` → `jc::` 치환 (9개 파일)
5. [x] sgfr cpp에 `using namespace jc;` 추가 (using namespace sgf; 뒤, 38개 파일)
6. [x] sgf.vcxproj / .filters에서 SgfMath.h 항목 제거, 파일 및 빈 Math 폴더 삭제
7. [x] 빌드 검증: jc / sgf / sgfr (Debug x64) — 전부 0 에러 0 경고
8. [x] /done 히스토리 작성

## 검증
- jc/sgf/sgfr Debug x64 빌드 성공 (0 error / 0 warning)
- 전 워크트리 grep에서 `SgfMath`/`sgf/Math` 참조 0건 (주석 2건 제외)
- 사용처 상수 치환: `Pi_v/4`→`jc_math_pi_div4`, `TwoPi_v`→`jc_math_pi2`, `Deg2Rad`→`jc_math_deg2rad`, `HalfPi_v`→`jc_math_pi_half` 등