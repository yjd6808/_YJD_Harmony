# JC_DEFER_SCOPE gtest 추가 (2026-09-25)

## 목표
`ScopeExit` / `JC_DEFER_SCOPE` 동작을 `jc_gtest`에서 검증한다.

## 설계
- `Projects/jc_gtest/Sources/jc_gtest/Primitives/DeferScopeTest.cpp` 신규 생성
  - 기존 Primitives 테스트와 동일 형식 (`#if TEST_DeferScopeTest == ON`, `TEST(DeferScopeTest, ...)`)

  - 케이스: 스코프 종료 시 실행, early return 경로 실행, `=`/`&`/`&var` 캡처,
    같은 스코프 내 2개 defer 실행 순서(LIFO), `Dismiss()` 미실행,
    예외 unwind 중 실행, `ScopeExit` 직접 생성
  - 힙 할당 없음 → `LeakCheck` 미사용 (`BitFlagTest`와 동일)
- `Sources/jc_gtest/PCH/Core.h`의 `PrimitivesTestEnabled` 블록에
  `#define TEST_DeferScopeTest ON` 추가
- `Project/jc_gtest.vcxproj`, `jc_gtest.vcxproj.filters`에
  `ClCompile` + `Primitives` 필터로 등록

## 순서
1. DeferScopeTest.cpp 작성
2. PCH Core.h 매크로 추가
3. vcxproj / filters 등록
4. `Scripts\BuildProject\jc_gtest.bat -C Debug -P x64` 빌드 후 테스트 실행
5. MultiByte 빌드 확인
