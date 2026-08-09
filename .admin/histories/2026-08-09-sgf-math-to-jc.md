# 작업 이력: sgf-math-to-jc (2026-08-09)

## 완료 요약
- `sgf/Math/SgfMath.h`(650줄, 헤더 온리)의 모든 수학 기능을 `jc/Sources/jc/Math.h`로 이관 완료
- sgf/sgfr의 사용 코드 전면 수정, SgfMath.h 파일·프로젝트 등록 제거

## 변경 내용
### jc/Math.h (이관 대상)
- 기존 `struct Math` 유지, 그 아래에 Vec2/Vec3/Vec4/Color/Mat4 + FloatEqual/Lerp/Clamp/Epsilon_v 이관 (jc 네임스페이스)
- 상수/단위변환은 **매크로**로 전환 (사용자 요청):
  - `jc_math_pi`, `jc_math_pi2`, `jc_math_pi_half`, `jc_math_pi_div2`, `jc_math_pi_div4`, `jc_math_deg2rad`, `jc_math_rad2deg`
  - 매크로라 네임스페이스와 무관하게 전역 사용 가능
- `Epsilon_v`는 constexpr, `FloatEqual/Lerp/Clamp`는 인라인 함수로 유지
- `PerspectiveFovLH` 가드의 파이 비교 → `jc_math_pi` 매크로 사용

### sgf
- include 15곳: `sgf/Math/SgfMath.h` → `jc/Math.h` (SgfHeader.h, Vertex/Texture/Shader/RenderTarget/Renderer2D/3D/GraphicDevice/Buffers/BatchRenderer/Camera/InputManager/Window.h, SoundEngine.cpp)
- 수학 타입 사용 18개 파일에 namespace sgf 블록 내 `using namespace jc;` 삽입
- 상수 사용처 갱신: Camera.cpp(Deg2Rad→jc_math_deg2rad, HalfPi_v→jc_math_pi_half), Renderer2D.cpp(Pi_v*2→jc_math_pi2), SoundEngine.cpp(TwoPi_v→jc_math_pi2)

### sgfr
- TutorialCommon.h include 교체
- 9개 Function.h/cpp의 명시 `sgf::Vec2/Vec3/Mat4/Color` → `jc::` 치환 (01,03,07,13,14,16,19,22)
- 수학 사용 38개 cpp에서 `using namespace sgf;` 뒤에 `using namespace jc;` 추가
- 상수 치환: `Pi_v/4`→`jc_math_pi_div4`(6곳), `TwoPi_v`→`jc_math_pi2`(7곳), `Deg2Rad`→`jc_math_deg2rad`(9곳), `Pi_v`→`jc_math_pi`

### 삭제
- `Projects\sgf\Sources\sgf\Math\SgfMath.h` 파일 및 빈 Math 폴더 삭제
- sgf.vcxproj / sgf.vcxproj.filters의 SgfMath.h 항목 제거

## 검증
- jc / sgf / sgfr Debug x64 빌드: 전부 **0 에러 / 0 경고**
- 워크트리 grep: `SgfMath`·`sgf/Math` 참조 없음 (jc/Math.h 주석 2건 제외)
- 참고: 참조 코드(문서 md 등)에서 `sgf/Math/SgfMath.h` 언급 시 jc/Math.h로 갱신 필요

## 메모
- HLSL raw 문자열(`float4 color` 등)의 `color` 단어는 대소문자 구분 제외 오탐으로 걸리므로 치환 스크립트는 `-cmatch`(대소문자 구분) 사용
- 18_ShadingModel_Function.cpp의 `TwoPi_v`는 `Pi_v` 치환보다 먼저 처리해야 함 (부분 문자열 포함 문제) — 별도 수정 완료