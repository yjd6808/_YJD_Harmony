# Math.h constexpr 전환 계획

- 날짜: 2026-10-05
- 대상: `Projects/jc/Sources/jc/Math.h` (1개 파일)

## 목표

`Math.h`의 모든 함수/메서드를 상수식에서 사용할 수 있도록 `constexpr`로 전환한다.
런타임 동작은 변경하지 않는다.

## 범위

1. `Math::Pow` → `constexpr`
2. `Math::Sin/Cos/Tan` 신규 추가 (런타임은 `std::` 버전, 상수식 평가 때만 Taylor 근사)
3. 자유 함수 `FloatEqual/Lerp/Clamp` → `constexpr` (`std::fabs` 의존 제거)
4. `vec2/vec3/vec4/size/rect/color/mat4` 모든 멤버 함수 → `constexpr`
5. `vec2/vec3/vec4` 정적 상수(`ZERO` 등) → `static constexpr` + `inline constexpr` 정의
6. `vec3::Normalized/Normalize`의 `jc_assert_msg`를 `is_constant_evaluated` 가드로 감싸 상수식 평가를 막지 않도록 함
7. `mat4` 회전/투영 함수의 `std::cos/sin/tan` 호출을 `Math::Sin/Cos/Tan`으로 교체
8. `<limits>`, `<type_traits>`, `jc/Assert.h` include 명시

## 제외

- `color` 프리셋 335종 (`Color.inl`): `Color.cpp` 주석에 기록된 대로 클래스 내부 초기화가 강제되어 분리 정의가 불가하므로 `const` 유지
- `jc_math_*` 매크로: 이미 상수식에서 사용 가능하므로 유지
- `Math::Pow` 음수 지수 동작 등 기존 의미 변경 없음

## 검증

1. `static_assert` 기반 상수식 평가 확인용 임시 파일로 `cl /c` 컴파일 검증
2. `Scripts\BuildProject\jc.bat -C Debug -P x64` 빌드
3. `Scripts\BuildProject-Multibyte\jc.bat -C Debug -P x64` 빌드
