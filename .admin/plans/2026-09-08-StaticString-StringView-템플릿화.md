# StaticString·StringView 템플릿화 plan (2026-09-08)

## 배경
- TCHAR 감사 #12/#13·§84는 char 전용 유지를 결정했으나 사용자 지시로 템플릿화 진행
- `String`=wchar 전환 후 `StringView(const String&)`·`StaticString` 상호운용이 깨짐

## 설계
- `StaticString<_u32 Size, typename CharT = char>` — 기존 `StaticString<N>` 그대로 동작
  - 내부 `char`→`CharT`, `AString`→`StrType=BasicString<CharT, StringImpl_SSO<CharT>>`
  - `ToString()`→`StrType`, `operator<<`→`basic_ostream<CharT>`, `std::string`→`basic_string<CharT>`
- `BasicStringView<typename CharT = char>` + 별칭 `StringView`(char)·`WStringView`(wchar)
  - `StringView.cpp`(790줄)→`StringView.inl` 템플릿 정의로 이동, 헤더 끝에서 include
  - `Split`→`Vector<BasicStringView<CharT>>`, `ToLower/Upper`→`StrType`
- `StringUtil` 추가(기존 변경 없음): `CTLength(const CharT*)`·배열 오버로드, `CTCompare` 본문 `char*`→`auto*`
- `Checker.h`: `IsStringType<StaticString<Size, CharT>>` 2파라미터 특수화
- `String.h` Comparator: `StaticString<Size>` 기본인자(char)로 그대로 유효, `Comparator<String>` 대 `StaticString<Size,char>`는 ToWide 브릿지

## 검증
1. `jc.bat -C Debug -P x64` 통과
2. 이후 gtest AString 전환(별도 단계): TYPED_TEST `{AString, PooledString}`, 테스트 파일 bulk AString

## 결과 (2026-09-08 작업 기록)
- 감사 #12/#13·§84(char 전용 유지) 대신 사용자 지시로 템플릿화 강행. 감사 문서와 충돌하므로 Phase C 절차는 이 plan이 override한다.
- `StaticString<Size, CharT=char>` + `BasicStringView<CharT=char>` + 별칭(`StringView`/`WStringView`), `StringView.cpp`(931줄) 삭제 → `StringView.inl`로 이동. `BasicString.h` fwd를 템플릿 선언으로 교체.
- `StringUtil`: `CTLength` CharT 오버로드 추가, `CTCompare` 본문 `char*`→`auto*` (char 동작 동일, wchar 순서 정상화).
- jc Debug + jc_gtest Debug 빌드 통과, **409/409 통과**.
- `Checker.h`의 `IsStringType<String>`·`IsString<String>` 소실(원인: step6 bulk, 본인 작업 아님)이 `HashSet<AString, CDefaultArrayAllocator>` Expand에서 PlacementNew assert를 유발함을 확인. `String.h`에 `String` 특수화 유지 + `_UNICODE` 한정 `AString` 특수화 추가로 해결.
- gtest bulk 전환 시 PowerShell `-replace` 대소문자 미구분으로 `std::string`→`std::AString` 오변환 + BOM 없는 파일 CP949 재인코딩 손상 발생. 전수 복구 완료(BOM/LF 일치 확인, `std::AString` 잔류 0건). 교훈: bulk 치환은 Python 바이트 단위로, `#include`·대소문자·인코딩을 명시적으로 처리할 것.
