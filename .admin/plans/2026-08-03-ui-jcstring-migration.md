# 2026-08-03 UI jc::String 마이그레이션

## 목표
sgcl UI 시스템(Xml/Controls/Theme/Core)에서 `strcmp` / `std::string` / `const char*` 사용을 제거하고 `jc::String`으로 통일한다.

## 작업 순서
1. UIValue.h/cpp — std::string 멤버/시그니처를 jc::String으로 교체
2. UIXmlLoader.cpp/h — strcmp 제거, 파서 내부 jc::String 전환
3. UIResolve.cpp/h — 표현식 파서 std::string → jc::String
4. UIThemeColor.h — SGUI_PARSE_ENTRY 매크로 strcmp 제거
5. Controls/Theme/Core 잔여 (Image/TextBlock/TextBox/UIEvent/UIManager/UIThemeManager/UIThemeMapper 등)
6. 빌드 검증 (`Scripts\BuildProject\sgcl.bat -C Debug -P x64`) + 런치 확인

## 규칙
- tinyxml2/cocos 경계에서 반환되는 const char*는 jc::String으로 감싸는 경계로만 허용
- cocos API가 const char*를 요구하면 `.Source()`로 전달
- 빌드는 x64 Debug
