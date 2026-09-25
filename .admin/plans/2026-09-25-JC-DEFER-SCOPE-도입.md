# JC_DEFER_SCOPE 도입 (2026-09-25)

## 목표
스코프 종료 시 람다를 실행하는 `ScopeExit` + `JC_DEFER_SCOPE(capture)` 매크로를 jc에 추가한다.

사용 형태:
```cpp
JC_DEFER_SCOPE(=)
{
    delete app;
    app = nullptr;
};
```

## 설계
- `Projects/jc/Sources/jc/Primitives/DeferScope.h` 신규 생성
  - `template <typename TFunc> class ScopeExit` : 생성자에서 callable 보관, 소멸자에서 호출, 복사 금지, 이동 가능, `Dismiss()` 지원
  - `detail::ScopeExitHelper` : `operator+` 로 람다를 받아 `ScopeExit` 반환 (매크로의 `+ [...]()` 트릭용)
  - `NS_JC_BEGIN` / 탭 들여쓰기 / 한국어 주석, 코딩 컨벤션 준수
- `Projects/jc/Sources/jc/Define.h` 에 매크로 추가
  - `#define JC_DEFER_SCOPE(capture) auto JC_CONCAT_COUNTER(_jcDefer_) = ::jc::detail::ScopeExitHelper() + [capture]()`
  - `;` 필수 형태 (지역 변수 선언식으로 확장)
  - `JC_CONCAT_COUNTER` 는 Macro.h에 정의되어 있으나 사용 시점(PCH 포함 후)에 확장되므로 Define.h에서 직접 참조 가능
- `Project/jc.vcxproj`, `jc.vcxproj.filters` 에 `Primitives/DeferScope.h` 를 `ClInclude` + `Primitives` 필터로 등록

## 순서
1. DeferScope.h 생성
2. Define.h 매크로 추가
3. vcxproj / filters 등록
4. `Scripts\BuildProject\jc.bat -C Debug -P x64` 빌드 검증
