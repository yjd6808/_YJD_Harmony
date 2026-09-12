# 문자열 TCHAR 전환 계획 — String / StringUtil / StaticString (2026-09-11)

- 작성일: 2026-09-11
- 저장 위치: `.admin/plans/2026-09-11-문자열-TCHAR전환-String-StringUtil-StaticString.md`
- 범위: `String.h` 교체 + `StringUtil` 클래스 템플릿화 + `StaticString` 2파라미터화. 선행(`Type.h`, `Ascii.h`, `Digit.h`, `CRuntime`)과 후행(`Comparator.h`, `Checker.h`, `BasicString.inl` 호출부) 포함. `StringFwd.h`는 만들지 않는다(2026-09-11 사용자 지정). `class String;` 7곳은 `String.h` 직접 include로 교체한다.
- 상위 기준: `D:\Programming\참고 문서\01_확정_결정사항.md`가 최상위. 하위 문서·기존 plan과 충돌 시 이 문서가 아니라 `01`을 따른다
- 관련 문서:
  - `00_INDEX.md` — 문서 인덱스, 신뢰도 표기 (`P05 §3.2`, `P06 §3.3`은 `07`에서 철회됨)
  - `01_확정_결정사항.md` — §A(타입), §B(StringUtil), §C(StaticString), §D(나머지), §E(실무), §F(일상 규칙)
  - `02_논의필요_항목.md` — Q-1~Q-24 (착수 전 필수 Q-1~Q-6)
  - `03_전체_로드맵.md` — S-0, 단계 A(A-0~A-12, 분해 불가), 단계 B~I
  - `04_프로젝트별_계측표.md` — jc 300파일 / `const char*` 470 / 리터럴 1991 / `String` 1043 / 로그 60
  - `05_기존_결함_목록.md` — A(오동작 10) / B(위험 18) / C(오타 7) / 인코딩 6+
  - `06_검증_스크립트.md` — §0~§9 grep 판정식
  - `07_조작내용_철회_고지.md` — `P05 §3.2`, `P06 §3.3` 신뢰 불가, 구버전 문서 폐기 내역
  - 기존 plan: `.admin/plans/2026-09-06-String-BasicString-재설계.md`, `2026-09-08-StaticString-StringView-템플릿화.md`, `2026-09-08-TCHAR-경계-감사.md`
- 프로젝트 지침: `AGENTS.md` — plan 파일 없이 코드 수정 금지, 5개 이상 파일 수정 시 사전 확인, 한국어 답변, 전 파일 UTF-8 저장, 아키텍처 변경 필요 시 임의 진행 금지, 주석에 안 채택안 비교 근거 기록 금지(코드 의도·동작만 기록)

---

## 0. 작업 배경 및 목적

### 0-1. 왜 이 3파일인가

`jc` 문자열 3종(`String`, `StringUtil`, `StaticString`)은 전 코드베이스(약 1574파일, `String` 2449곳, `StringUtil::` 88파일 698곳)의 공통 분모다. 로드맵 `03` §4는 `A-1~A-12`를 분해 불가 원자 단위로 규정한다. 셋 중 하나만 바꾸고 멈추면 모든 하위 프로젝트가 빌드 실패한다. 그래서 이번 작업은 "String.h 1개"가 아니라 "선행 4종 + 본체 3종 + 후행 2종"을 한 묶음으로 계획한다.

### 0-2. 이번 plan의 목적

1. 참고 문서(`01` 확정 19건)와 현행 소스(실측)의 차이를 고정한다.
2. `String.h` → 별칭 전환, `StringUtil` → 클래스 템플릿, `StaticString` → 2파라미터 전환의 함수 단위 변환표를 확정한다.
3. 착수 전 필수 결정(Q-1~Q-6)을 사용자 확인용으로 분리한다. 결정 없이는 코드 손대지 않는다.
4. `AGENTS.md`의 확인 게이트(5파일 이상 수정 시 사전 확인, 아키텍처 변경 시 알림)를 어느 시점에 거칠지 명시한다.

---

## 1. 분석한 기존 코드 현황 (실측, 2026-09-11 기준)

> 아래 행 번호는 현재 워크트리 실측이다. 참고 문서의 행 번호(`:45`, `:223` 등)와 어긋난 곳은 별도 표기했다.

### 1-1. `Primitives/String.h` (228줄) — 구 힙 String, 아직 별칭 아님

- `class String final` 직접 정의. `pBuffer_(char*)` + `len_` + `capacity_` 3멤버(`:219~221`).
- `static const String Empty / Null(:34~35)`, `IsNull()(:55)`은 `pBuffer_ == nullptr` 판정. `BasicString.h:14` 주석("Null 상태 없음")과 모순. 확정 D-2 제거 대상.
- `friend class StringUtil(:223)` 1곳. `StringUtil.cpp`의 `Format(:265 `szResult.len_ = ...`)`, `GetRange(:294~296 `pBuffer_/len_/capacity_` 대입)`이 private에 직접 대입한다. 신규 `BasicString`은 Storage 기반이라 이 멤버가 없어서 본문 재작성이 필수다(확정 B-4).
- `Source()/SafeSource()`가 `char*`/`const char*` 고정(`:47~48`). `Append/Find/Compare/Replace/Split/Format` 전부 `char`/`const char*` 고정. `_char` 미적용.
- 전방 선언 `class String;` 7곳이 `String.h` 교체 시 전부 깨진다: `IO/Directory.h:16`, `IO/File.h:18`, `StringUtil.h:23`, `StringView.h:10`, `Stream.h:24`, `Time.h:521`, `Checker.h:9`. 별칭은 전방 선언이 불가하므로 7곳 모두 `String.h` 직접 include로 교체한다(2026-09-11 사용자 지정. `StringFwd.h` 신설 안 함). 실측 추가: `BasicString.inl`이 어디에서도 include되지 않은 고아 상태다. `String.h` 허브 끝에서 include해야 본문 정의가 컴파일된다.

### 1-2. `Primitives/StringUtil.h` (430줄) + `StringUtil.cpp` (796줄) — 비템플릿 클래스

- `class StringUtil final(:24)`. `StringUtil<CharT>`가 아니다. 호출부 88파일 698곳이 전부 `StringUtil::` 형태다.
- CT 계열 전면 `const char*`/`char*` 고정: `CTCompare(:37)`, `CTCopy(:69)`, `CTCount(:86)`, `CTCountChar(:107)`, `CTFind 3종(:111~146)`, `CTFindChar(:149~157)`, `CTFindCharReverse(:160)`, `CTLength(:165 + 배열 :180)`, `CTToInt32/64(:186~194)`, `CTTrim 계열(:196~256)`, `CTToNumber(:365)`.
- `CTCompare(:48~49)`에 `char* pSrc = (char*)_src;` 강제 캐스팅. 확정 C-3 제거 대상. wide에서 2바이트를 1바이트씩 훑는 조용한 오동작원이다.
- `FillLeft(:261)` 내부 `char szFill[1024]` + `'\0'` 고정. 템플릿화 시 `CharT`로 교체해야 한다.
- 런타임 API 전면 narrow: `ConcatInnerBack/Front`, `Copy/CopyUnsafe`, `Find 7종`, `FindChar 5종`, `Format/FormatBuffer 4종`, `GetRange/SubStr/GetRangeUnsafe`, `IsEqual/Compare/Length`, `SkipLeadingChar/SkipLeadingNumberZero`, `Split/Swap`.
- `ToNumber<TInteger>(:335)` + `TryToNumber(:347)` 멤버 템플릿 2종. `ToStringBuffered<TNumber>(:338)` + `ToString<TNumber>(:341)` 멤버 템플릿 2종. 클래스 템플릿화 시 멤버 템플릿의 명시적 특수화는 문법상 불법이라 확정 B-2 분리가 필요하다.
  - `ToNumber/TryToNumber`: 명시적 인자 호출부 48곳 존재(`BasicString.inl:1245~1308` 10곳, `StringView.inl` 10곳, `Time.cpp` 10곳, `_Extern` 22곳 등). 멤버 템플릿 유지 + `if constexpr` 단일본문으로 간다.
  - `ToString/ToStringBuffered`: 명시적 인자 호출부 0곳. 일반 오버로드로 격하한다. 호출부 무수정.
- `ToUtf8(const wchar_t*, int)(:343, 구현 :742)`는 타입 교차 함수라 클래스 템플릿에 두지 않는다. 외부 호출자 0곳이므로 `StringConvert::ToUtf8`로 흡수 후 제거(확정 B-6).
- `StringUtil.cpp:253~266 Format`: `vsnprintf(nullptr,0,...)` 길이 측정 후 `String(iExpectedLen+1+DEFAULT_BUFFER_SIZE)` 확보, `vsnprintf` 재호출, `SetAtForce` + `szResult.len_ = iExpectedLen` 직접 대입. 템플릿화 시 `SetLength()/Adopt()` 계약으로 재작성 + wide 분기는 `_vscwprintf` 래퍼(`CRuntime::FormatLengthV`) 필요(확정 B-4, 논의 Q-8).
- `StringUtil.cpp:288~299 GetRange`: `GetRangeUnsafe`가 `dbg_new char[]`로 만든 버퍼를 `subString.pBuffer_/len_/capacity_`에 직접 이식. 템플릿화 시 `Adopt()` 계약으로 재작성(논의 Q-9와 연결).
- `StringUtil.cpp:458~534 ToNumber` 13종 특수화 전부 `std::strtol/strtoll/strtoul/strtof/strtod` narrow CRT. wide는 `wcstol` 계열 + `CRuntime` wide 오버로드가 선행되어야 한다(확정 D-3).
- `StringUtil.cpp:540~739 ToStringBuffered/ToString` 15곳 전부 `ToStringBuffered(buf, sizeof(buf), _value)` 형태(`:629~725`). 2번째 인자는 문자 개수인데 `sizeof`를 넘긴다. char라 우연히 맞고 wide에서 2배 오보고. 전부 `_countof(buf)`로 교체(확정 E-2). 버퍼 크기는 8/8/8/16×6/32×2/64×2.
- 내부 자기호출 50곳 이상이 `StringUtil::` 무한정 호출이다(`:45~46`, `:87~88`, `:108`, `:113`, `:118~120`, `:137`, `:151`, `:156`, `:161~162`, `:188`, `:193`, `:464~536`, `:629~726`). 멤버 템플릿 방식이면 `<CharT>` 누락 1곳이 조용한 narrow 폴백이 된다. 클래스 템플릿 방식을 쓰는 결정적 근거다(확정 B-1).
- `#define JC_FMT(...) jc::StringUtil::Format(__VA_ARGS__)(:430)` → `StringUtilT::Format`으로 교체(확정 B-1).

### 1-3. `Primitives/StaticString.h` (361줄) — 1파라미터, char 고정

- `template <_u32 Size> struct StaticString(:20)`. `CharT` 파라미터 없음. `char Source[Size] = ""(:23)` — wide 초기화 불가 형태.
- 전 멤버 `const char*`/`char` 고정: `operator==/!=` 6종, `Compare`, `StartWith/EndWith/Find/Contain/Remove`, `operator=/SetString/SetStringUnsafe/CopyFrom`, `ToString()->String`, `operator<<`.
- `(char*)` 강제 캐스팅 6곳: `:96~98(CopyFrom)`, `:134~136`, `:167, :178(Find)`, `:222~223(Remove)`. 확정 C-3 제거 필수.
- `operator[](:61~63)` — `const` 함수에서 `const_cast<char*>`로 쓰기 허용. `CopyFrom(const char(&)[P]) const(:87)` — const인데 `Source` 변경. 템플릿화할 때 const 정합성 수정(확정 C-5).
- `SetString(const char*, int count)(:296~307)` 3종 — `StringUtil::Copy(Source, count, str)`에서 `count`를 용량 자리에 넘긴다. `count > Size`면 오버런(결함 B-2). 이번에 같이 고친다.
- 와이어(커맨드) 구조체 필드가 전부 `StaticString<N>`(≒char)이다. `CharT` 기본값을 `_char`로 뒤집는 순간 패킷 크기가 조용히 2배가 된다. 확정 C-4에 따라 와이어 필드는 `StaticStringA<N>`으로 못 박고 `static_assert(sizeof(...))`로 잠근다. 대상 12곳: `sg/Cmd_AUTHENTICATION.h:26,27`, `sg/Cmd_LOBBY.h:26`, `sg/Cmd_MESSAGE.h:25`, `sg/SteinsGate_CharBase.h:455,456`, `sgs/AuthenticationComponent.h:40`, `sgs/CmdHost_MESSAGE.h:23`, `sgs/CmdHost_SETUP.h:51`, `sgs/CmdRelay_AUTHENTICATION.h:26`, `sgs/CmdRelay_TEST.h:25,28`, `jnetr_common/Command.h:17`, `jnet/IPEndPoint.cpp:154`.
- `operator<<(std::ostream)` narrow 고정. 템플릿화 시 `std::basic_ostream<CharT>`로 교체한다.

### 1-4. 함께 봐야 하는 주변 파일 실측

| 파일 | 실측 | 이번 작업과 관계 |
|---|---|---|
| `Primitives/BasicString.h` 256줄 | `BasicString<CharT, Storage>` 선언 완료. Storage 계약 주석(`:10~16`) 유지. `:44~45 Empty/Null` 잔재, `:70 IsNull(){return false;}`, `:89 Append(const BasicStringView<char>&)` char 하드코드(결함 B-5), `:77~82` 산술 Append가 `std::to_string`+`AppendAscii` 우회(논의 Q-13) | `String.h` 교체 시 별칭 허브가 됨. `:89`, `:45`, 산술 Append 3곳은 이번에 같이 수정 |
| `Primitives/BasicString.inl` | `Append(_s32/_u32/.../_f64)` 12종이 `char b[32]`+`snprintf`+`AppendAscii`(`:249~271`). `ToNumber/TryToNumber` 20곳이 `StringUtil::ToNumber/TryToNumber` 무한정 호출(`:1245~1378`) | `StringUtil<CharT>` 전환 시 호출부 `StringUtilT::` 또는 `StringUtil<CharT>::`로 명시. 산술 Append는 Q-13 결정 후 `StringUtil<CharT>::ToString` 교체 |
| `Primitives/StringImpl_SSO.h` 380줄 | 24B SSO. `kSsoCapacity` char 22 / wchar 10(`:35~37`), `static_assert` 4종(`:82~85`), `Adopt(:336)` 이미 존재, `WString` 별칭(`:378`) 존재, `AString` 없음 | Q-1(48B 확대) 결정이 `static_assert`와 직결. `AString` 별칭은 `String.h` 허브에 둔다 |
| `Primitives/StringImpl_SSOCtxTlsPool.h` | 8B 핸들 Storage. `:355` 주석에 "using String 교체(step 6) 전까지 PooledString" | 이번 범위 밖. 별칭 위치만 충돌 없게 둔다 |
| `Primitives/StringView.h` 199줄 vs `StringView.inl` 1000줄+ | `.h`는 구 `class StringView`(char 고정, `StringView(const String&)`), `.inl`은 `BasicStringView<CharT>` 템플릿 정의. 둘이 짝이 맞지 않는다. `BasicString.h:34`의 전방 선언(`template <typename CharT = char> class BasicStringView;`)과 `.h`의 구 선언도 충돌한다 | 빌드에서 어느 쪽이 실제로 쓰이는지 선행 확인 필수. 09-08 plan의 "템플릿화 완료" 주장과 현행 `.h`가 불일치한다. 이번 plan은 `.h` 실측을 기준으로 삼는다 |
| `Primitives/StringConvert.h` 31줄 | `ToWide/ToUtf8` explicit 5종. `AString`/`WString`을 쓰는데 `AString` 정의가 워크트리 어디에도 없다(grep 0건) | `AString` 미정의 상태에서 이 헤더가 어떻게 빌드되는지 선행 확인 필수. `String.h` 허브에 `AString`을 두면 해소된다 |
| `Type.h` 38줄 | `_char` 없음. `_s32/_s8/_u8` 중복 using(`:11~12`, `:18~20`, `:24`). `_s16c = wchar_t(:25)` 유지 필요(`_Extern` C-ABI 의존) | 확정 A-1 적용 + 중복 정리. 단 중복 정리는 TCHAR와 무관한 정리라 diff 분리 권장 |
| `Ascii.h` 45줄 | `#include` 0개인데 `NS_JC_BEGIN` 사용(결함 B-7). 7함수 전부 `const char` 고정 | 확정 B-5. `StringUtil.h`보다 먼저 템플릿화 |
| `Digit.h` 22줄 + `Digit.cpp` | `Length(int)` 문자 무관 + `HexChar(const char*)` narrow | 논의 Q-10. `HexChar()` 오버로드 권장안으로 간다 |
| `Wrapper/CRuntime.h` 172줄 | `System/FileOpen/FileDelete/FileRename` 전부 `const char*` 고정. wide 오버로드 없음 | 확정 D-3/D-4. `StrTo*/FormatV/FormatLengthV/StrLen/StrCmp/StrRChr` wide 오버로드 추가 + 경로 `_char`화 |
| `Comparator.h` 105줄 | `Comparator<const char*>`는 `StringUtil::CTCompare` 호출. `Comparator<String>`의 `operator()(String, StaticString)`가 `_rhs.Source` 배열 멤버 접근(`:74`) vs `:51`은 `_rhs.Source()` 함수 호출 형태. `StaticString::Source`는 배열이라 `:49` 형태가 부적절(결함 B-3). sed 후 `_Extern/Comparator`는 `StringUtilA::` 되돌림 대상 | `StringUtilT::` 치환 + `StaticString<Size, CharT>` 2파라미터 대응. `:49/:74` 불일치는 이번에 같이 확정한다 |
| `TypeTraits/Checker.h` 269줄 | `IsStringType<String>(:122)`, `IsStringType<StaticString<Size>>(:123~124)`, `char[N]/wchar_t[N]/const char[N](:125~130)` — `const wchar_t[N]` 누락(결함 B-4). `template <_u32> struct StaticString(:11)` 1파라미터 전방 선언 | 2파라미터 전방 선언 + `const wchar_t[N]` 추가 + `IsString<String>` 유지 여부 확인(09-08 plan에서 소실 사고 전력 있음) |
| `Define.h` | `JC_FILENAME(:6)`이 `strrchr(__FILE__)` narrow 고정 | 확정 D-1. narrow 유지 + 출력 시 `%hs`. 이번에 건드리지 않고 규칙으로만 고정한다 |

### 1-5. 현행 코드와 참고 문서의 불일치 3건 (작업 전 확인 필수)

1. 09-08 plan은 "`StaticString<Size, CharT=char>` + `BasicStringView<CharT=char>` 템플릿화 완료, jc Debug + gtest 409/409 통과"라고 기록했으나, 현행 `StaticString.h`는 1파라미터, `StringView.h`는 구 char 클래스다. 되돌려졌거나别 브랜치 기록이다. 이번 plan은 현행 실측을 정본으로 삼는다. 착수 전 `git log --oneline -- Primitives/StaticString.h Primitives/StringView.h`로 이력 확인이 필요하다.
2. `StringView.inl`은 `BasicStringView<CharT>` 템플릿인데 `StringView.h`가 구 선언이라 짝이 맞지 않는다. 빌드 포함 관계(`vcxproj`, `StringView.cpp` 잔재 여부) 확인이 필요하다.
3. `StringConvert.h`가 `AString`을 쓰는데 워크트리에 `AString` 정의가 없다. `String.h` 허브에 별칭 3종을 두면 해소되지만, 그 전까지 이 헤더가 단독으로 빌드되는지 확인이 필요하다.

---

## 2. 목표 형상 (확정 `01` 기준)

### 2-1. `Type.h` (확정 A-1)

```cpp
#include <tchar.h>

NS_JC_BEGIN
using _char  = TCHAR;      // _UNICODE 시 wchar_t, 아니면 char
using _achar = char;       // narrow 고정
using _wchar = wchar_t;    // wide 고정
NS_JC_END
```

- 분기 기준은 `_UNICODE`(`UNICODE` 아님). `<tchar.h>`가 `_UNICODE`를 본다.
- `_s16c = wchar_t` 유지. `_s32/_s8/_u8` 중복 using은 함께 정리하되 별도 커밋으로 분리한다.
- `jnet/IPAddress.h`의 `#ifdef UNICODE` 분기는 `_UNICODE` 기준으로 통일한다(리스크 R8). 단 이번 3파일 작업분이 아니라 `jnet` 단계에서 처리하고, 여기서는 목록에만 남긴다.

### 2-2. `String.h` — 별칭 허브 (확정 A-2, 로드맵 A-7)

```cpp
using AString = BasicString<char,    StringImpl_SSO<char>>;
using WString = BasicString<wchar_t, StringImpl_SSO<wchar_t>>;
using String  = BasicString<_char,   StringImpl_SSO<_char>>;   // = A 또는 W
```

- 구 `class String` 정의는 삭제한다. `BasicString.h` + `StringImpl_SSO.h`를 include한 뒤 별칭 3종만 둔다.
- `StringImpl_SSO.h:378`의 `WString` 별칭은 허브로 이관하고 헤더 쪽은 삭제한다. 중복 정의 금지.
- `_T` 매크로는 직접 만들지 않는다. MSVC `<tchar.h>` 2단 매크로를 그대로 쓴다(확정 A-3). `_T(MACRO)` 형태가 깨지지 않게 한다.
- 템플릿 파라미터 이름은 `CharT`로 통일한다(확정 A-4).
- `D-2`에 따라 `String::Null`/`IsNull()`을 제거하고 `IsEmpty()/Empty`로 통일한다. 참조 5곳 처리: `jnet/Buffer/PacketBuffer.cpp` 2곳(`jc::CMessage::Null`), `jc/Container/CMessage.cpp:917`, `jc/Sync/WaitHandle.cpp:77`, `sg JsonUtil::GetStringOrNull`. `StringView::IsNull`은 별개 의미라 유지한다.
- `BasicString.h:89`의 `Append(const BasicStringView<char>&)`를 `Append(const BasicStringView<CharT>&)`로 고친다.
- 산술 `Append`(`BasicString.h:77~82`, `BasicString.inl:249~277`)의 `std::to_string`+`AppendAscii` 우회는 Q-13 결정 후 `StringUtil<CharT>::ToString`으로 교체한다.

### 2-3. `StringUtil` — 클래스 템플릿 (확정 B-1~B-6)

```cpp
template <typename CharT>
class StringUtil final { ... };

using StringUtilA = StringUtil<char>;
using StringUtilW = StringUtil<wchar_t>;
using StringUtilT = StringUtil<_char>;

#define JC_FMT(...) jc::StringUtilT::Format(__VA_ARGS__)
```

- 멤버 템플릿(`StringUtil::method<CharT>()`) 안은 기각한다. 내부 자기호출 50곳에서 `<CharT>` 1개 누락이 조용한 narrow 폴백이 되기 때문이다. 클래스 템플릿은 즉시 컴파일 에러라 안전하다.
- `ToNumber/TryToNumber` 48곳 명시 호출은 멤버 템플릿 유지 + `if constexpr` 단일본문으로 살린다(확정 B-2).
- `ToString/ToStringBuffered` 명시 호출 0곳은 일반 오버로드로 격하한다. 호출부 무수정.
- 포맷 분기는 `if constexpr (std::is_same_v<CharT, char>)` + `CRuntime::FormatBuffered(buf, cap, "%d"/L"%d", v)` 형태다. `_T()`를 쓰면 `_UNICODE`를 따라 `CharT`와 어긋난다(확정 B-3).
- `friend class StringUtil(:223)`은 재작성한다. `Format`은 `SetLength()/Adopt()` 계약으로, `GetRange`는 `Adopt()` 계약으로 바꾼다. `vswprintf(nullptr,0,...)`는 길이를 반환하지 않으므로 `CRuntime::FormatLengthV`(`_vscwprintf` 래퍼, Q-8)로 감싼다.
- `Ascii.h`/`Digit.h`를 먼저 처리한다(확정 B-5, 로드맵 A-1/A-2).
- `ToUtf8`은 `StringConvert::ToUtf8`로 흡수 후 제거한다(확정 B-6).

### 2-4. `StaticString` — 2파라미터 (확정 C-1~C-5)

```cpp
template <_u32 Size, typename CharT = _char>
struct StaticString {
    CharT Source[Size] = {};     // = "" 금지 (wide 불가)
    // ...
};

template <_u32 N> using StaticStringA = StaticString<N, char>;
template <_u32 N> using StaticStringW = StaticString<N, wchar_t>;
template <_u32 N> using StaticStringT = StaticString<N, _char>;
```

- `Size`는 문자 개수. 바이트 수는 `Size * sizeof(CharT)`.
- 전 멤버 `CharT` 치환 + 호출 `StringUtil<CharT>::CTLength(Source)` 형태(확정 C-2).
- `(char*)` 6곳 + `StringUtil.h:48~49` 1곳 제거(확정 C-3).
- 와이어 필드는 `StaticStringA<N>` 고정 + `static_assert(sizeof(CAU_Login) == EXPECTED)` 잠금(확정 C-4).
- `const_cast` 쓰는 `operator[]`와 const인 `CopyFrom`의 const를 정리한다(확정 C-5).
- `SetString(str, count)` 3종의 `count→용량` 오용(결함 B-2)을 이번에 같이 고친다.

### 2-5. 일상 규칙 (확정 F, 작업 중 준수)

1. 문자 타입은 `jc::_char`, 문자열은 `jc::String`만 쓴다.
2. 리터럴은 전부 `_T("")`.
3. `StringUtil`은 `StringUtilT::` (경계에서만 `StringUtilA::`).
4. narrow 값을 TCHAR 포맷에 넣을 때는 `%s`가 아니라 `%hs`.
5. 새 `const char*` 파라미터를 만들지 않는다.
6. 경계 예외 11항목(narrow 유지 허용)은 `01` §F 표를 그대로 따른다. 이번 3파일에서는 `_Extern/*`, `StringConvert`, `__FILE__/__FUNCTION__/JC_FILENAME`, 파일 내용(바이트)이 해당한다.

---

## 3. 사용자 결정 확정 (2026-09-11 반영)

> 아래가 최종 결정이다. §4 이후 단계는 이 결정을 기준으로 읽는다.

| 번호 | 결정 | 의미 |
|---|---|---|
| Q-1 | 권고안 승인 (48B 확대 A안) | `StringImpl_SSO` 24B→48B로 확대한다. `kTotalBytes/kSsoUnits/kSsoCapacity/static_assert` 전부 변경 |
| Q-2 | **C안 확정** | 표시용 `Name[]` 테이블은 TCHAR 유지. `sgcl`(`cc::Label`, narrow 필수)에서 매번 변환한다 |
| Q-3 | **sgcl 작업 제외** | sgcl은 손대지 않는다. 엔진 전면 교체 전까지 jc 뒤집기 시 sgcl 빌드 실패는 예정된 상태로 둔다 |
| Q-4 | 승인 | S-0/A-0 인코딩 정규화 선행한다. 단 sgcl 파일(`UIManager.h:7`)은 Q-3에 따라 제외한다 |
| Q-5 | **단일 TCHAR API로 정리** | `ErrorMessageUTF8/LastErrorMessageUTF8`에서 `UTF8` 접미 제거 → `ErrorMessage/LastErrorMessage` 2종만 남기고 전부 `jc::String`(`_char` 기반) 반환. `ErrorMessageMBCS`는 호출부 0곳이라 삭제. 상세는 §3-1 Q-5 파급 |
| Q-6 | 승인 | `main` 유지 + `StringConvert` 변환. 단 sgcl은 Q-3에 따라 보류한다 |
| Q-7/Q-8/Q-9/Q-10/Q-13/Q-24 | 전부 승인 | Q-7 유지, Q-8 `CRuntime::FormatLengthV`, Q-9 B안(public `Adopt`), Q-10 B안(`HexChar()` 오버로드), Q-13 `StringUtil<CharT>::ToString` 교체, Q-24 `Null` 제거 |

---

## 3-1. 결정에 따른 파급 (반드시 숙지)

### Q-2 C안 파급 — 표시용 테이블 TCHAR 유지

- 확정 `01` D-8/Q-2 권고안(A안: 전부 narrow)은 이번 결정으로 폐기한다. `P07(sg)`에서 표시용 테이블을 TCHAR로 분류한 기존 기술은 유지된다.
- 대상 테이블(재확인): `SteinsGate_ChannelBase.h:39`, `SteinsGate_Server.h:28,66,90`, `SteinsGate_UI.h:43`, `SteinsGate_Action.h:30`.
- 서버 콘솔(`sgs`, TCHAR 유리)은 그대로 쓰고, 클라이언트(`sgcl`, `cc::Label` narrow 필수)는 경계에서 `StringConvert::ToUtf8(TCHAR→UTF-8 narrow)`로 매번 변환한다. 변환 비용은 `sgcl` 측 부담이며 이번 작업 범위 밖이다(Q-3).
- 로그에 표시용 테이블을 넣을 때는 TCHAR 포맷이라 `%s` 그대로 쓴다. `%hs`로 바꾸지 않는다(A안에서만 필요했던 조치).
- `CMD_FUNC_DEF_NAME`(`#struct` narrow 고정)은 데이터키라 이번 결정과 무관하게 narrow 유지한다(Q-11 A안).

### Q-3 파급 — sgcl 제외, 빌드 실패 예정

- 확정 `01` E-1 3단계(`sgcl/Game/UI/`의 `StringUtilT::`→`StringUtilA::` 되돌림)는 수행하지 않는다.
- `06` §6 sgcl 전용 검증 4종은 이번 종료 조건에서 제외한다.
- jc의 `String`이 `_char`(=wchar, `_UNICODE` 기준)로 뒤집히는 순간 sgcl은 타입 불일치로 빌드 실패한다. 이는 결정된 예정 상태이며 복구하지 않는다. 엔진 전면 교체 시점에 별도 plan으로 처리한다.
- Phase 0 인코딩 대상에서 `sgcl/UIManager.h:7`을 제외한다. Phase 8의 `C-0~C-7` 순서도 전부 보류한다.

### Q-5 파급 — 단일 TCHAR API (실측 기반, 2026-09-11 사용자 지정)

- 현행 실측(`jnet/Winsock.h:21~23`, `Winsock.cpp:71~119`):
  - `LastErrorMessageUTF8() → ErrorMessageUTF8(WSAGetLastError())`
  - `ErrorMessageMBCS(code)`: `FormatMessageA` 직접 → narrow(MBCS). 호출부 0곳.
  - `ErrorMessageUTF8(code)`: `FormatMessageW` → `wchar_t buf[512]` → `WideCharToMultiByte(CP_UTF8)` → narrow(UTF-8). 호출부 3곳(`TcpSession.cpp:64`, `sgcl C_LOBBY.cpp:35`, `C_AUTH.cpp:35`).
- 목표 형상(사용자 지정 그대로):

```cpp
static jc::String LastErrorMessage();
static jc::String ErrorMessage(_u32 _errorCode);
```

- `UTF8` 접미 2종은 이름만 바꾸는 게 아니라 반환을 `jc::String`(`_char` 기반)으로 바꾼다. `ErrorMessageMBCS`는 호출부가 없어 삭제한다.
- 구현 방침: 접미 없는 `FormatMessage` 매크로를 그대로 쓴다. SDK 매크로가 `UNICODE`를 보고 `W/A`를 골라주고, 버퍼(`msg.Source()`, `_char*`=`TCHAR*`)는 `_UNICODE`를 따른다. Phase -1에서 전 프로젝트가 Unicode로 통일됐고 아래 동기화 가드가 어긋남을 컴파일 단계에서 잡으므로 분기가 필요 없다. `WideCharToMultiByte` 변환 없음. `StringUtil` 경유 금지(계층 순환 방지). 접미 없는 `FormatMessage` 매크로는 `UNICODE`를 보고 버퍼(`_char`)는 `_UNICODE`를 보므로 쓰지 않는다. `UNICODE` 상태와 무관하게 동작하므로 양쪽 매크로 조합에서 안전하다.
- 동기화 가드(`Type.h`, `_char` 정의 직후). 어긋난 TU가 있으면 컴파일 단계에서 잡는다:

```cpp
#if defined(_UNICODE) != defined(UNICODE)
#error "UNICODE와 _UNICODE가 어긋나 있습니다. CharacterSet 설정을 확인하십시오."
#endif
```

```cpp
jc::String Winsock::ErrorMessage(_u32 _errorCode)
{
    constexpr int BUF_SIZE = 512;
    jc::String msg{BUF_SIZE};
    // FormatMessage는 UNICODE 기준 매크로(W/A 자동 선택). 버퍼 _char*와 타입이 일치한다.
    DWORD n = FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
                            nullptr, _errorCode,
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                            msg.Source(), BUF_SIZE, nullptr);
    msg.SetLength(int(n));
    return msg;
}
```

- 호출부 3곳은 `ErrorMessageUTF8`→`ErrorMessage`, `LastErrorMessageUTF8`→`LastErrorMessage`로 개명한다. TCHAR 로그(`%s`)라 `%hs` 전환 불필요하다. 단 sgcl 2곳(`C_LOBBY/C_AUTH`)은 Q-3 제외라 개명하지 않고 남긴다. sgcl 빌드 실패는 예정 상태에 포함된다.
- 기존 권고(`ErrorMessageUTF8 → AString` 고정)는 폐기한다.

---

## 3-2. 착수 전 결정 요청 (원 요청 기록, 상용 확정으로 대체됨)

> 코드 수정 전에 결정이 있어야 한다. 특히 Q-1은 `static_assert`와 `String` 크기를 바꾸므로 되돌리기 비용이 크다.

| 번호 | 항목 | 선택지 | 권고안(참고 문서 기준) | 이번 작업에 미치는 영향 |
|---|---|---|---|---|
| Q-1 | SSO 버퍼 크기 | A. 48B 확대(char 46자/wchar 22자) / B. 24B 유지(wide 10자) / C. CharT별 상이 | A | `StringImpl_SSO.h:35~37, :82~85` static_assert 전부 변경. B면 wide heap 할당 빈발을 감수. C면 코드 복잡. 미결정 시 일단 24B 유지로 plan을 그리되 `String` 크기 가정 코드를 쓰지 않는다 |
| Q-2 | sg 표시용 Name 테이블 타입 | A. 전부 narrow+서버 `%hs` / B. A/W 2벌 / C. TCHAR+sgcl 변환 | A | 이번 3파일 직접 대상은 아니나 `StaticString` 와이어 고정 목록과 경계가 겹친다. A 승인 시 C-4 목록 확정 |
| Q-3 | sgcl UI narrow 고정(확정 D-8 승인) | A. sgui `AString` 고정 / B. 전면 TCHAR | A안 승인 요청 | 승인 시 `E-1` 3단계 sed에 sgcl 되돌림이 포함된다. 미승인 시 sed 범위에서 sgcl 제외 |
| Q-4 | 소스 인코딩 UTF-8 통일(A-0/S-0) | 승인 필요 | 승인 요청 | `_T()`를 붙이기 전에 깨진 6파일+를 고치지 않으면 깨짐이 상수화된다. 이번 작업 선행 조건 |
| Q-5 | `Winsock::ErrorMessageUTF8` 반환형 | `jc::AString` 고정 권고 | 권고 수용 요청 | `StringUtil::ToUtf8` 이관과 함께 `StringConvert` 경계 예시로 쓴다 |
| Q-6 | sgcl 진입점 `main` 유지 | `main` 유지+`StringConvert` 변환 권고 | 권고 수용 요청 | 이번 3파일 범위 밖. 기록용 |
| Q-7 | `_s16c` 별칭 | 유지 권고(C-ABI 의존) | 유지 | `ToNumber<_s16c>` 특수화 주석만 보강한다 |
| Q-8 | `_vscwprintf` 래퍼 위치 | `CRuntime::FormatLengthV` 권고 | 수용 | `StringUtil<CharT>::Format` wide 분기 선행 조건 |
| Q-9 | `Adopt` 노출 방식 | A. friend 유지 / B. public `Adopt` / C. `GetRange`를 멤버로 이동 | B 권고 | `StringUtil::GetRange` 재작성과 직결. B면 `BasicString`에 public `Adopt`를 둔다(현재 SSO Storage에는 이미 있음) |
| Q-10 | `Digit` 처리 | A. 클래스 템플릿 / B. `HexChar()` 오버로드 | B 권고 | B면 `Length` 불변 + `HexChar()` wide 오버로드 1개 추가다 |
| Q-13 | 산술 `Append` narrow 경로 | `StringUtil<CharT>::ToString` 교체 권고 | 수용 | `BasicString.inl:249~277` 12종 처리 방식 결정 |
| Q-24 | `String::Null` 제거 승인 | 승인 요청 | 진행 예정 | D-2 참조 5곳 정리와 함께 처리한다 |

Q-11/Q-12/Q-14~Q-23은 sg/sgcl/jdb 단계에서 결정한다. 이번 plan에 포함하지 않는다.

---

## 4. 구체적인 수정 계획 (단계별)

> 원칙: `A-0~A-12` 분해 불가. 다른 프로젝트 착수 금지. 별도 브랜치에서 진행하고 각 단계 끝에 `06` grep 검증을 통과한다. 5개 이상 파일 수정 단계 진입 전에 사용자 확인을 받는다(`AGENTS.md` Constraints 2).

### Phase -1. 전 프로젝트 Unicode 전환 (2026-09-11 사용자 지정, 확정 D-10 override)

> 확정 `01` D-10은 "CharacterSet 통일은 사용자 범위(범위 외)"였으나, 사용자 지정으로 이번 작업에 포함한다. `_char`가 `_UNICODE`를 따르므로 MultiByte 빌드가 남아 있으면 같은 코드가 TU마다 다르게 해석된다.

1. 전수 조사 결과(실측): MultiByte 전용 6곳(`jc` 4, `jcr` 4, `jdbr` 4, `jnet_gtest` 1, `sgf` 4, `sgfr` 4) + 혼재 3곳(`jnet` 2, `jnetr_common` 2, `mcp` 2) = 9파일 27곳. ThirdParty win10 3곳(`libcocos2d/libSpine/librecast`, CharacterSet 없음)은 외부라 손대지 않는다.
2. 교체 내용: `<CharacterSet>MultiByte</CharacterSet>` → `<CharacterSet>Unicode</CharacterSet>`. 바이트 치환으로 인코딩·BOM 보존한다.
3. 검증: 전환 후 `MultiByte` 잔류 0건(`06` CharacterSet 집계 재실행). 베이스라인 빌드는 Phase 0에서 jc부터 수행한다.
4. 주의: 이 전환 자체가 `_char`(char→wchar) 뒤집기이므로, 코드 수정 없이 빌드하면 jc부터 에러가 난다. 그 에러 목록이 Phase 1~7의 작업 목록이 된다(감사 문서 `2026-09-08-TCHAR-경계-감사.md` §5 절차와 동일).
5. 실행 결과(2026-09-11): 9파일 27곳 전환 완료, `MultiByte` 잔류 0건. jc Debug/x64 빌드 통과. 통과까지 필요했던 수정 2건:
   - `jc.vcxproj`에서 삭제된 파일 참조 2행 제거(`String.cpp`, `StringView.cpp`. 작업 트리에서 이미 삭제(D)된 상태로 커밋만 안 된 선행 잔재).
   - `IO/Directory.cpp` 3곳을 A 접미로 고정(`GetFileAttributesA`, `FindFirstFileA`/`FindNextFileA` + `WIN32_FIND_DATAA`). narrow 입출력이라 동작 변경 없음. 컴파일 명령줄에 `/D _UNICODE /D UNICODE` 동시 정의 확인됨.

### Phase 0. 준비 — 브랜치, 인코딩, 베이스라인 (로드맵 S-0/A-0, `06` §0)

1. 별도 브랜치 생성(또는 작업 전 백업). `E-1` sed는 되돌릴 수 없으므로 필수다.
2. 인코딩 정규화. `06` §0 3종 실행(Q-4 승인, Q-3에 따라 sgcl 제외):
   - `file -bi`로 non-UTF8 목록화. 우선 복구 5파일: `jnet/SendHelper.h`, `jnet/TcpServer.cpp`, `jnet/Config.cpp`, `sg/DescMgr_CharAnimation.cpp:38,49`, `sg/DescMgr_MapPhysics.h`(바이너리 감지). `sgcl/UIManager.h:7`은 제외한다.
   - `U+FFFD`(`EF BF BD`) 잔류 0건 확인(단 sgcl은 범위 밖이라 별도 집계한다).
3. 베이스라인 빌드. `Scripts\BuildProject\jc.bat -P x64`(Debug 기본)로 현재 통과 여부 기록. 실패 지점이 있으면 TCHAR 작업과 분리해 먼저 고친다.
4. 불일치 3건(§1-5) 확인: `git log`로 StaticString/StringView 이력, `vcxproj`에서 `StringView.cpp` 포함 여부, `AString` 미정의 상태의 `StringConvert.h` 빌드 경로.
5. **확인 게이트 1 (완료)**: Q-1~Q-6 결정 확정(§3). 베이스라인 결과만 보고하고 Phase 1 진입한다.

### Phase 1. `Type.h` + `String.h` 허브 (확정 A-1/A-2, 로드맵 A-7 선행)

1. `Type.h`:
   - `<tchar.h>` include + `_char/_achar/_wchar` 추가 + `UNICODE/_UNICODE` 동기화 가드.
   - `_s16c` 유지. `_s32/_s8/_u8` 중복 using은 별도 커밋으로 정리한다(TCHAR diff와 분리).
2. `String.h` 허브 교체 + `BasicString.inl` 연결:
   - 구 `class String` 정의 삭제 → include(`BasicString.h`, `StringImpl_SSO.h`) + 별칭 `AString/WString/String` 3종 + 끝에서 `BasicString.inl` include(현재 고아 상태라 연결 필수).
   - `StringImpl_SSO.h:378`의 `WString` 별칭은 허브로 이관하고 헤더 쪽은 삭제한다.
   - `class String;` 7곳을 `String.h` 직접 include로 교체: `IO/Directory.h:16`, `IO/File.h:18`, `StringUtil.h:23`, `StringView.h:10`, `Stream.h:24`, `Time.h:521`, `Checker.h:9`. (`StringFwd.h` 신설 안 함. 사용자 지정)
3. 검증: 교체 후 jc 빌드. `BasicString.inl` 연결로 기존에 미정의였던 `BasicString` 멤버 정의가 처음 컴파일되므로 신규 에러가 나오면 그 자리에서 해소한다.

### Phase 2. `Ascii.h` + `Digit.h` (확정 B-5, 로드맵 A-1/A-2)

1. `Ascii.h`:
   - `Namespace.h`에 `NS_JC_BEGIN`이 있으므로 직접 `#include "jc/Namespace.h"` 추가(단독 컴파일 가능화, 결함 B-7).
   - 7함수 템플릿화: `IsAlphabat/IsLowerCaseAlphabat/IsUpperCaseAlphabat/IsNumeric/IsSpace/ToLower/ToUpper` → `template <typename CharT> constexpr ... (CharT)`. 리터럴(`'a'`, `' '`)은 `CharT(...)` 형태로 Character 독립화한다. 파라미터 이름은 `CharT`(확정 A-4).
   - `StringUtil.h` CT 함수와 `BasicString` 내부 `ToLowerUnit` 호출처를 `Ascii<CharT>` 형태로 맞춘다.
2. `Digit.h`:
   - Q-10 B안: `Length(int)` 유지 + `HexChar()` 오버로드(`const char*` + `const wchar_t*`) 추가. `Digit.cpp`에 wide 테이블 추가.
3. 검증: `06` §1-④(문자 리터럴), §1-⑥(Ascii 호출부) grep으로 영향 범위 재측정.

### Phase 3. `CRuntime` wide 오버로드 (확정 D-3/D-4, 로드맵 A-3)

1. 추가 대상: `StrToLong/StrToULong/StrToLLong/StrToULLong/StrToFloat/StrToDouble`, `FormatV/FormatLengthV/StrLen/StrCmp/StrRChr`. char판은 기존 CRT(`strtol` 계열), wide판은 `wcstol` 계열.
2. `FormatLengthV`는 `_vscwprintf` 래퍼다(Q-8). `StringUtil<CharT>::Format` wide 분기가 쓴다.
3. 파일 I/O 경로 분리(확정 D-4): 경로(`FileOpen/FileDelete/FileRename`, `System`)는 `_char`(`_tfopen_s/_tremove/_trename`), 내용(`FileRead/FileWrite`)은 `char/_u8` 유지. 이번 Phase에서는 wide 오버로드 추가까지만 하고 호출부 뒤집기는 A-10에서 처리한다.
4. 검증: char/wchar 양쪽 단위 호출 빌드. 기존 narrow 동작 변경 없음.

### Phase 4. `StringUtil<CharT>` 클래스 템플릿화 (확정 B-1~B-6, 로드맵 A-4/A-5)

> 가장 큰 단계. 헤더·cpp·호출부 3면을 동시에 바꾼다.

1. 클래스 선언 전환(`StringUtil.h:24`):
   - `template <typename CharT> class StringUtil final` + 별칭 `StringUtilA/W/T` + `JC_FMT → StringUtilT::Format`.
   - `class String;` 전방 선언을 `StringFwd.h` include로 교체한다.
2. CT(constexpr) 함수군 `CharT`화:
   - `CTLength(const CharT*)` + 배열 오버로드 `CTLength(const CharT(&)[CAP])` 유지. 기존 `StringUtil::CTLength<N>` 호출과 호환되게 한다.
   - `CTCompare/CTCopy/CTCount/CTCountChar/CTFind 3종/CTFindChar 2종/CTFindCharReverse/CTToInt32/CTToInt64/CTTrim/Left/Right/CTZeroMemory/CTToNumber` 전부 `char`→`CharT`, `'\0'`→`CharT(0)`, `'0'`→`CharT('0')` 치환.
   - `CTCompare(:48~49)`의 `(char*)` 캐스팅을 `const CharT*`로 교체. `jc::ToLower` 호출은 Phase 2의 `Ascii<CharT>::ToLower`로 교체한다.
   - `FillLeft`의 `char szFill[1024]`→`CharT szFill[1024]`, `'\0'`→`CharT(0)`. `jc::String(0)`/`szRet` 타입은 `BasicString<CharT, ...>`로 교체한다.
3. 런타임 함수군 `CharT`화:
   - `ConcatInnerBack/Front`, `Copy/CopyUnsafe`, `Find 7종`, `FindChar 5종`, `IsEqual/Compare/Length/LengthWithNull`, `SkipLeading*`, `Split/Swap` 전부 `CharT` 치환. `tolower` 직접 호출(`StringUtil.cpp:124`)은 `Ascii<CharT>` 경유로 바꾼다.
   - `Length(nullptr)`가 `-1`을 반환하는 현행 계약(`:309~320`)은 유지한다. 호출부 의존이 있을 수 있어 이번에 바꾸지 않는다.
4. `ToNumber/TryToNumber` (확정 B-2 전반):
   - 멤버 템플릿 유지. 본문을 `if constexpr` 단일본문으로 합치고 분기 내 CRT만 `CRuntime::StrTo*(...)` 경유로 바꾼다. `_s16c` 특수화의 `strtoul` 사용은 주석으로 명시한다(Q-7).
   - `TryToNumber`의 `errno` 판정 로직은 그대로 둔다.
5. `ToString/ToStringBuffered` (확정 B-2 후반 + E-2):
   - 멤버 템플릿 특수화 13종을 일반 오버로드 13종으로 격하한다. 시그니처는 `static int ToStringBuffered(CharT* buf, int cap, <산술타입> v)` 형태다.
   - 포맷 문자열은 `if constexpr (std::is_same_v<CharT, char>) "%d" : L"%d"` 분기다. `_T()` 금지(확정 B-3).
   - 15곳 `sizeof(buf)`→`_countof(buf)` 교체. 버퍼 선언 크기는 그대로 둔다(8/8/8/16×6/32×2/64×2).
6. `Format/FormatBuffer/GetRange` 재작성 (확정 B-4, Q-8/Q-9):
   - `Format(const CharT*, ...)` / `(const CharT*, va_list)`: 길이 측정은 `CRuntime::FormatLengthV`, 쓰기는 `CRuntime::FormatV` 경유. 결과 이식은 `SetLength()/Adopt()` 계약으로만 한다. `len_` 직접 대입 금지.
   - `GetRange`: `GetRangeUnsafe`의 `dbg_new char[]`를 `CharT`로 바꾸고 결과 이식은 `Adopt()`로만 한다. Q-9 B안이면 `BasicString::Adopt`를 public으로 노출한다.
   - `FormatBuffer`: 용량 초과 assert 유지. `vsnprintf` 직접 호출을 `CRuntime` 경유로 바꾼다.
7. `ToUtf8` 이관 + Winsock 단일 TCHAR API (§3-1 Q-5 확정 기준):
   - `StringUtil::ToUtf8` 삭제. `StringConvert::ToUtf8`로 흡수한다.
   - `ErrorMessageMBCS` 삭제(호출부 0곳). `ErrorMessageUTF8/LastErrorMessageUTF8` → `ErrorMessage/LastErrorMessage`로 개명하고 반환을 `jc::String`으로 바꾼다.
   - 구현은 `_UNICODE` 분기로 `FormatMessageW/A` 직접 호출. `WideCharToMultiByte` 없음. `StringUtil` 경유 금지(계층 순환 방지).
   - 호출부: `TcpSession.cpp:64`는 이번에 개명한다. sgcl 2곳(`C_LOBBY/C_AUTH`)은 Q-3 제외라 손대지 않는다.
8. 내부 자기호출 50곳을 `StringUtil<CharT>::` 명시 형태로 바꾼다. 하나라도 빠지면 narrow 폴백이므로 `06` §1-③ grep으로 0건을 확인한다.
9. sed 마이그레이션 (확정 E-1, Q-3 확정으로 sgcl 제외):
   - 전량 `StringUtil::`→`StringUtilT::`, 이후 narrow 고정 2곳(`_Extern/Extern_StringUtil.cpp`, `Comparator.h`)을 `StringUtilA::`로 되돌린다.
   - sgcl UI 계층 되돌림은 수행하지 않는다(Q-3 작업 제외). sgcl 빌드 실패는 예정 상태로 둔다.
   - 실행 전 반드시 백업한다.
10. 검증: `06` §1-①(캐스팅 0건), §1-②(sizeof 0건), §1-③(미치환 0건), §1-⑤(friend 현황), §1-⑦(명시 인자 호출부) 전부 통과.
11. **확인 게이트 2**: 파일 수가 5개를 넘으므로(헤더+cpp+호출부) Phase 5 진입 전에 diff를 사용자에게 확인받는다.

### Phase 5. `StaticString<Size, CharT>` 전환 (확정 C-1~C-5, 로드맵 A-6)

1. 선언부: `template <_u32 Size, typename CharT = _char> struct StaticString` + 별칭 `StaticStringA/W/T`. `CharT Source[Size] = {};`로 교체한다.
2. 멤버 전면 `CharT` 치환표:

| 현행 | 전환 후 |
|---|---|
| `operator==/!=(const char(&)[P])` | `operator==/!=(const CharT(&)[P])` |
| `operator==/!=(const char*)` | `operator==/!=(const CharT*)` |
| `operator==/!=(const String&)` | `BasicString<CharT, StringImpl_SSO<CharT>>` 오버로드로 교체 |
| `operator[](int) const` + `const_cast<char*>` | non-const `operator[]`로 const 정합성 수정 |
| `Compare(const char*)` | `Compare(const CharT*)` |
| `CopyFrom(const char(&)[P]) const` | `CopyFrom(const CharT(&)[P])`(const 제거) |
| `StartWith/EndWith/Find/Contain/Remove` | `CharT` 치환 + 내부 `(char*)` 제거 |
| `operator=(const char*/String/StaticString)` | `CharT` 버전 3종 |
| `SetString/SetStringUnsafe` 7종 + `count` 3종 | `CharT` 치환 + `count` 오용 수정(B-2) |
| `ToString()->String` | `ToString()->BasicString<CharT, StringImpl_SSO<CharT>>` |
| `CopyFrom(const char*/String/start,end,str)` | `CharT` 치환 |
| `operator<<(std::ostream)` | `operator<<(std::basic_ostream<CharT>)` |

3. `(char*)` 6곳 제거 + `StringUtil<CharT>::CTLength/CTCompare` 호출로 교체.
4. 와이어 필드 12곳을 `StaticStringA<N>`으로 고정하고 각 커맨드 헤더에 `static_assert(sizeof(T) == EXPECTED)`를 추가한다. 기대값은 현재 `sizeof` 실측으로 채운 뒤 잠근다(프로토콜 변경 검출용).
5. 검증: `06` §2 3종(인스턴스화 전수, 와이어 A 고정, `.Source ==/!=` 포인터 비교 0건) 통과.

### Phase 6. `String.h` 별칭 교체 + `BasicString` 마무리 (확정 A-2/D-2, 로드맵 A-7)

1. `String.h`를 허브로 교체: include(`BasicString.h`, `StringImpl_SSO.h`, `StringFwd.h`) + 별칭 `AString/WString/String` 3종. `StringImpl_SSO.h:378`의 `WString`은 삭제한다.
2. `BasicString.h` 3곳 수정: `:89` char 하드코드 → `CharT`, `:44~45 Null` 제거, `:70 IsNull` 제거. `Empty/EmptySource`는 유지한다.
3. `String::Null` 참조 5곳을 `Empty`/`IsEmpty()`로 교체한다.
4. `BasicString.inl` 호출부: `StringUtil::ToNumber/TryToNumber` 20곳을 `StringUtilT::` 또는 `StringUtil<CharT>::`로 명시한다. 산술 `Append` 12종은 Q-13 승인에 따라 `StringUtil<CharT>::ToString`으로 교체한다.
5. 검증: `06` §5 타입 오염 grep에서 신규 `const char*` 0건(경계 예외 제외).

### Phase 7. 후행 — `Comparator.h` / `Checker.h` (로드맵 A-8, 결함 B-3/B-4)

1. `Comparator.h`: `StringUtil::`→`StringUtilT::`/`StringUtilA::`(窄 고정 2곳). `:49`의 `_rhs.Source()` 호출 형태와 `:74`의 `_rhs.Source` 멤버 접근을 `Source`(배열) 기준으로 통일한다. `StaticString<Size>` 참조를 2파라미터(`StaticString<Size, CharT>` 또는 `StaticStringA`)에 맞춘다.
2. `Checker.h`: `template <_u32> struct StaticString` 전방 선언을 2파라미터로 교체. `IsStringType<StaticString<Size, CharT>>` 특수화 + `const wchar_t[Size]` 추가(B-4). `IsStringType<String>`/`IsString<String>`이 소실되지 않게 대조한다(09-08 사고 재발 방지).
3. 검증: `Comparator`/`Checker` 사용처(`HashMap<String,int>` 등) 빌드.

### Phase 8. 잔여 A 블록 연결 (로드맵 A-9~A-12, 이번 3파일 밖이지만 순서 고정)

1. `Assert` 부분 전환(확정 D-1): `_fmt`만 `_char*`, `__FILE__/__FUNCTION__` narrow 유지 + `%hs` 출력.
2. `Console` VT 매크로(확정 D-6): `CSI`를 `_T` 기반으로 바꾸되 `sg/LogSpecifier.h`와 원자적으로 함께 바꾼다.
3. 파일 I/O 경로 TCHAR화(확정 D-4): Phase 3에서 추가한 `_char` 오버로드로 호출부 전환.
4. jc 전역 `_T()` 처리 + `%hs` 전환(리스크 R2). `06` §3/`§4` grep 0건 수렴.
5. 이번 plan의 종료 조건은 Phase 7까지다. Phase 8은 별도 plan으로 분리하되 순서를 어기지 않는다.

---

## 5. 예상 변경 파일 목록

### 5-1. 신설 (0개. `StringFwd.h`는 만들지 않음)

| 파일 | 내용 |
|---|---|
| (와이어 잠금 추가분) 각 커맨드 헤더 내 `static_assert` | `sizeof` 기대값 고정. 파일 신설이 아니라 기존 헤더追記 |

### 5-2. 본체 수정 (7개)

| 파일 | 변경 개요 | 근거 행 |
|---|---|---|
| `jc/Type.h` | `_char/_achar/_wchar` 추가, `<tchar.h>` | 확정 A-1 |
| `jc/Ascii.h` | 7함수 템플릿화 + `Namespace.h` include | 확정 B-5, 결함 B-7 |
| `jc/Digit.h` + `jc/Digit.cpp` | `HexChar()` wide 오버로드 | 논의 Q-10 |
| `jc/Wrapper/CRuntime.h` + `.cpp` | `StrTo*/FormatV/FormatLengthV/StrLen/StrCmp/StrRChr` wide 오버로드 | 확정 D-3 |
| `jc/Primitives/StringUtil.h` + `.cpp` | 클래스 템플릿화 + `ToNumber` 단일 본문 + `ToString` 오버로드 격하 + `Format/GetRange` 재작성 + `ToUtf8` 제거 + `sizeof→_countof` 15곳 + `JC_FMT` | 확정 B-1~B-6, E-2 |
| `jc/Primitives/StaticString.h` | 2파라미터 + `CharT` 전면 치환 + `(char*)` 6곳 제거 + const 정리 + `count` 오용 수정 + `basic_ostream` | 확정 C-1~C-5, 결함 B-1/B-2 |
| `jc/Primitives/String.h` | 구 정의 삭제 → 별칭 허브(`AString/WString/String`) | 확정 A-2 |

### 5-3. 후행 수정 (4개 + 호출부)

| 파일 | 변경 개요 |
|---|---|
| `jc/Primitives/BasicString.h` + `.inl` | `:89` CharT화, `Null` 제거(Q-24 승인), 산술 Append `StringUtil<CharT>::ToString` 교체(Q-13 승인), `StringUtil<CharT>::` 명시 20곳 |
| `jc/Primitives/StringImpl_SSO.h` | Q-1 A안 승인: 24B→48B 확대 + `static_assert` 갱신. `WString` 별칭을 `String.h`로 이관 |
| `jc/Comparator.h` | `StringUtilT/A` 치환 + `Source` 접근 통일 + 2파라미터 대응 |
| `jc/TypeTraits/Checker.h` | 2파라미터 전방 선언 + `const wchar_t[N]` 추가 |
| 호출부 88파일 698곳 | `E-1` sed + `_Extern`/`Comparator` 되돌림. sgcl은 제외(Q-3) |
| 와이어 12곳 | `StaticStringA` 고정 + `static_assert` |
| 전방 선언 7곳 | `StringFwd.h` 교체 |
| `String::Null` 5곳 | `Empty/IsEmpty` 교체(Q-24 승인) |
| `jnet/Winsock.h` + `.cpp` | Q-5 확정: `ErrorMessage/LastErrorMessage` 2종 `jc::String` 반환, `MBCS` 삭제, `TcpSession.cpp:64` 개명 |

### 5-4. 이번에 건드리지 않는 것

- `StringView.h/.inl` 불일치 해소, `sg/sgs/jnet/jdb` 뒤집기, `Assert/Console/Logger` 전환, 파일 I/O 호출부 전환, jc 전역 `_T()` 살포. 이유: `A-8` 이후 순서이며 diff를 늘리면 검증이 불가능해진다.
- `Type.h` 중복 using 정리, `sg/Config.{h,cpp}` 삭제, `Image.cpp` 삭제 등 TCHAR 무관 정리는 별도 커밋으로 분리한다.
- `sgcl` 373파일 전부 제외(Q-3 확정). 뒤집기 이후 sgcl 빌드 실패는 예정 상태로 두고 복구하지 않는다. `sgf/sgfr` 207파일, `mcp/mcpr` 35파일도 범위 밖이다.

---

## 6. 리스크 및 기존 결함 대응표

### 6-1. 로드맵 리스크 R1~R8 중 이번 범위 관련

| 번호 | 리스크 | 이번 plan의 대응 |
|---|---|---|
| R1 | 와이어 포맷 파손 | C-4 고정 + `static_assert` 잠금. Phase 5에서 처리 |
| R2 | `%s`에 narrow 값 | `%hs` 규칙. 이번에는 `Format` 분기까지만 만들고 전역 전환은 Phase 8에서 처리 |
| R3 | 인코딩 깨짐 상수화 | Phase 0 S-0 선행. `_T()` 살포 전에 6파일 복구 |
| R4 | 조용한 narrow 폴백 | 클래스 템플릿 강제. 내부 50곳 `StringUtil<CharT>::` 명시 + `06` §1-③ 검증 |
| R5 | SSO 용량 감소 | Q-1 A안 승인: 48B 확대. `kTotalBytes/kSsoUnits/kSsoCapacity/static_assert` 갱신 + `WStringTest Capacity()==10` 상수 수정(로드맵 B) |
| R6 | `sizeof` 오보고 | E-2 15곳 `_countof`. Phase 4에서 처리 |
| R7 | cocos 경계 대량 변환 | Q-2 C안 + Q-3 제외로 해소. 표시용 테이블은 TCHAR 유지, sgcl 변환은 엔진 교체 시점으로 이관. sed 되돌림·§6 검증에서 sgcl 제외 |
| R8 | `_UNICODE` vs `UNICODE` 혼용 | `Type.h`는 `_UNICODE`로 통일. `IPAddress.h`는 jnet 단계로 이관 |
| R9 | 외부 thirdparty 한쪽만 지원 (세션 규칙) | narrow/wide 한쪽만 지원해서 변경 불가 지점이 나오면 우회·변환 꼼수를 넣지 말고 작업 중지 후 보고 (대상 라이브러리, 막힌 지점, 영향 범위 포함). 이 세션의 전환 작업에만 적용 |

### 6-2. 기존 결함 `05` 중 이번에 같이 처리하는 것

- B-4(`Checker.h` wide 배열 누락): Phase 7에서 처리. TCHAR 전환을 바로 막는다.
- B-6(`sizeof` 15곳): Phase 4에서 처리. wide에서 2배 오보고다.
- B-7(`Ascii.h` include 없음): Phase 2에서 처리. 단독 컴파일 불가다.
- B-1(`StaticString` const 위반): Phase 5에서 처리.
- B-2(`SetString count` 오용): Phase 5에서 처리.
- B-3(`Comparator` Source 혼용): Phase 7에서 처리.
- B-5(`BasicString` 자기모순 2건 중 `:89` + `Null`): Phase 6에서 처리.

### 6-3. 분리하는 것 (이번에 건드리지 않음)

- A-1(댕글링 `Format(...).Source()`), A-2(`sizeof(sizeof(T))`), A-3(`WriteString` return 누락), A-4(임시 참조 반환), A-5(포인터 주소 비교), A-6(임시본 exception), A-7(죽은 `Image.cpp`), A-8(주석 세션 누수), A-9(죽은 초기화 함수), A-10(용량 계산 부족). 이유: TCHAR와 무관한 오동작이라 섞으면 되돌리기가 불가능해진다. 별도 수정으로 분리한다.

---

## 7. 검증 계획 (`06` 스크립트 + 빌드)

### 7-1. 단계 종료 조건

| 단계 | 판정 (`06` 번호) | 통과 기준 |
|---|---|---|
| Phase 0 | §0 인코딩 3종 | non-UTF8 0건, `U+FFFD` 0건, 바이너리 감지 0건 |
| Phase 2 | §1-④/⑥ | 문자 리터럴 CharT 전환 확인, Ascii 호출부 재측정 |
| Phase 4 | §1-①/②/③/⑤/⑦ | `(char*)` 캐스팅 0건, `sizeof` 문자수 자리 0건, 미치환 `StringUtil::` 0건(`_Extern/Comparator` 제외), friend 현황 확인, 명시 인자 호출부 재현 |
| Phase 5 | §2 3종 | `StaticString<` 전수 목록화, 와이어 `StaticStringA` 고정(비A 0건), `.Source ==/!=` 0건 |
| Phase 6~7 | §5 타입 오염 | 신규 `const char*` 0건(경계 예외 11항목 제외), narrow CRT 직접 호출 0건(예외 경로 제외), `char* = .Source()` 직접 취득 0건, `return Format(...).Source()` 0건 |
| 최종 | A 전역 | `jc` Unicode/MultiByte 양쪽 빌드 성공. `jc_gtest` AString/WString 동시 통과(B 단계 연결) |

### 7-2. 빌드 및 테스트 명령

- 빌드: `Scripts\BuildProject\jc.bat -P x64` (기본 Debug/x64. x86 금지. `AGENTS.md` Build 참조).
- `jc_gtest`는 D-7 `TYPED_TEST` 도입과 함께 A/W 양쪽 테스트로 간다. 현행 코드베이스에 `TYPED_TEST`가 없으므로 최초 도입이다. 단 googletest 1.8.1이라 `TYPED_TEST_SUITE` 대신 `TYPED_TEST_CASE`를 쓰고, 가변 인자라 콤마가 쪼개지므로 `typedef ::testing::Types<char, wchar_t> XxxTypes;`로 묶어서 넘긴다. `Lit<CharT>` 헬퍼가 필요하다.
- 최종 판정: `_UNICODE` 정의/미정의 두 설정 전수 빌드 + 테스트 통과. CharacterSet 통일(`MultiByte 27/Unicode 79` 혼재)은 사용자 범위라 여기서 바꾸지 않는다.

### 7-3. `AGENTS.md` 준수 사항

- UTF-8 저장, 한국어 주석, `Allman` 중괄호, jc 별칭 타입(`_s32/_u32` 등) 사용, 탭 들여쓰기.
- 주석에는 안 비교 근거를 쓰지 않는다. 코드 의도·동작만 기록한다.
- 5개 이상 파일 수정 단계(Phase 4~7)는 진입 전 사용자 확인을 받는다.
- 아키텍처 변경 판단(Q-1 48B 확대는 `sizeof(String)` 24→48 변경, Q-9 public `Adopt` 노출)이 필요하면 임의 진행하지 않고 알린다.

---

## 8. 작업 순서 요약 (실행 체크리스트)

```
[x] Phase -1 Unicode 전환: 9파일 27곳, MultiByte 잔류 0건
[x] Phase 1 Type/String: _char+가드, String.h 허브+inl 연결, 7곳 String.h 교체
[x] Phase 2/3 Ascii/Digit/CRuntime: 템플릿+HexCharW+wide/_char 오버로드
[x] Phase 4 StringUtil<CharT>: 헤더+inl+단일본문+격하+Adopt+sed 543곳(sgcl 제외)+Extern 되돌림
[x] Phase 5 StaticString<Size,CharT>: 전면 치환+캐스팅 제거+const+count+Checker
[x] Phase 6 String 마무리: Null/IsNull 제거+Empty 정의+CMessage/Stream String 복귀(TCHAR-프로토콜)
[x] Phase 7 Comparator/Checker: StringUtil 의존 제거+혼합 삭제+제네릭 trait
[x] StringView.h 템플릿 전환(BasicStringView+별칭, inl 연결)
[x] CMessage TCHAR-프로토콜(sizeof 배율+복사+Deallocate), Dump _T(), Winsock 단일 API 설계
[x] 파일별 마이그레이션(1차): Directory/File/FileService/FileStream/FileSource/FileDest/HttpHeaders/Env/ConsoleMenuItem/Thread/WaitHandle/HttpTypes
[x] 파일별 마이그레이션(2차): Time/Uri/StringTokenizer/WinApi/Console/Logger/Path/Http/SyncHttpTransport — jc Debug/x64 빌드 통과
[x] Q-1 확정: 24B 유지 (사용자 지정). SSO 변경 없음.
[x] 빌드 인프라: Build.ps1 -Multibyte + BuildProject-Multibyte 31종 + AGENTS.md 반영
[x] jc Unicode + MultiByte 양쪽 빌드 통과 (SyncHttpTransport W-only 5곳 인라인 분기)
[ ] 잔여: Time.h/Time.cpp, Logger*, Console.cpp, Uri, SyncHttpTransport, StringTokenizer, WinApi, HttpService, Path.cpp, PathResolver, LoggerDefine, FileLogger
```

---

## 9. 남은 확인 (결정 완료, 실행 전 최종 점검)

1. ~~Q-1~~ 확정: 24B 유지. `_char` 23개 배열(22문자 + NUL + len) 현행 그대로. 48B 확대 안 함.
2. ~~Q-2/Q-3~~ 확정(C안 + sgcl 제외). sgcl 빌드 실패 예정을 인지했는지 최종 확인만 남는다.
3. ~~Q-4~~ 승인. `sg/DescMgr_MapPhysics.h` 바이너리 건 처리 방침만 실행 시 보고한다.
4. ~~Q-8/Q-9/Q-10/Q-13~~ 승인(권고안대로). Q-9 public `Adopt` 노출도 아키텍처 변경이므로 실행 전 알린다.
5. ~~Q-24~~ 승인.
6. §1-5 불일치 3건 중 `StringView.h/.inl` 해소와 `AString` 미정의 확인을 Phase 0 선행에 포함한다.
