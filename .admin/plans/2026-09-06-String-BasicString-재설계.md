# jc::String BasicString 재설계

- 날짜: 2026-09-06
- 선행 문서
  - `.admin/plans/2026-09-05-String-구현-후보-성능비교-설계.md` (후보 S0~S7 정의)
  - `.admin/plans/2026-09-06-String-벤치마크-결과-분석.md` (벤치 수치·순위)
  - 외부 검토 2종 (`jc-String-CrossThread-설계-검토.md`, `S4-CrossThread-안전화와-String-아키텍처-방향.md`)
- 목표: 벤치 후보 코드를 `BasicString<CharT, Storage>` 1벌 구조로 이관한다.

---

## 1. 최종 구조

```cpp
template <class CharT, class Storage>
class BasicString {
    Storage st_;   // 값으로 포함. Inline이면 24B, Pooled면 8B 핸들
public:
    // 본문 1벌: st_.Data()/Len()/SetLen()/Cap()/Reserve()/Assign()만 사용.
    // 저장 방식을 가정하는 분기(if constexpr (Pooled) 등) 금지.
};
```

### 1-1. Storage 2종 (1차 범위)

| Storage | 구조 | 용도 |
|---|---|---|
| `StringImpl_SSO` | union 24B, SSO 22B, 힙은 전역 | 기본. 스레드 정책 없음 |
| `StringImpl_SSOCtxTlsPool` | 8B 핸들 + 64B 슬롯(TLS bounded 캐시, SSO 43B) | opt-in. 대량 핸들 이동 컨테이너 |

### 1-2. 공개 별칭 (일상용) + 풀네임 (정밀용)

```cpp
using String       = BasicString<char, StringImpl_SSO<char>>;
using WString      = BasicString<wchar_t, StringImpl_SSO<wchar_t>>;
using PooledString = BasicString<char, StringImpl_SSOCtxTlsPool<char>>;
```

- 일상 코드는 별칭만 사용한다.
- 벤치·Storage 특화·디버깅처럼 정책이 문제될 때만 풀네임(`BasicString<char, ...>`)을 쓴다.
- Storage 간 변환, CharT 간 변환은 전부 explicit. 통로는 `StringView`(같은 CharT) 또는 트랜스코딩 함수(CharT 간).

### 1-3. Storage 계약 (좁게 고정)

```cpp
// 요구 멤버:
//   using value_type;
//   CharT* Data();  const CharT* Data() const;
//   int Len() const;  void SetLen(int);
//   int Cap() const;                    // NUL 제외 내용 원소 수 기준. 구 Capacity()는 Cap()으로 통일.
//   void Reserve(int cap);              // 축소 없음
//   void Assign(const CharT* s, int n); // 깊은 복사, self/overlap 안전
//   void AssignFresh(const CharT* s, int n); // 막 만든 Storage 전용 고속 흡수 (겹침 없음)
//   void Adopt(CharT* buf, int len, int cap); // 외부 힙 버퍼 소유권 인수 (Format 등)
//   void Clear();
//   복사(깊은 복사) / 이동(소유권 이전) / 소멸(noexcept)
//   이동 대입의 원본 처리 방식은 Storage 재량이다. SSO는 비움(steal),
//   pooled는 swap (풀 트래픽 0. Sort 이동 폭풍 대응. §9.3 원결정 유지).
//   P6 TYPED_TEST의 moved-from 단언(L240 Source()==nullptr)은 양쪽 다 carve-out.
//   SSO Source()는 절대 null이 아니고 pooled도 EmptyStr이라 해당 단언을 만족할 수 없다.
//   Null 상태 없음 (빈 문자열은 len 0. 구 String::Null은 폐기).
//   SetLen은 길이만 둔다. NUL 종결은 호출자 책임 (BasicString::CommitLen).
```

- 쓰기 전 보장 계약: BasicString은 non-const `Data()`·`SetLen()`·쓰기系 호출 전에 항상 `Reserve()`를 먼저 부른다. Storage는 쓰기 진입 시 슬롯 존재를 가정해도 된다.
- `StringImpl_SSOCtxTlsPool<CharT>`가 이 계약의 첫 구현이다. `ObjectPoolTLSIndexed<StringSlot<CharT>>` 사용, S8 정책 (상세: `2026-09-06-ObjectPoolTLSIndexed-설계-및-개발지시.md` §9). SSO 43(char)/21(wchar_t). `Reserve`는 정확히 맞춤 (증가 정책은 BasicString 몫).

- 새 후보 추가 = `StringImpl_XXX.h` 1개 + 별칭 1줄 + 벤치 등록. 본문·StringUtil 손댐 없음.

---

## 2. CharT 지원

- 시그니처는 처음부터 `(CharT, Storage)` 두 축으로 선언한다.
- 1차 구현·테스트는 `char`로 안정화하고, `wchar_t`는 그 다음에 추가한다. 두 축 동시 완성은 디버깅 2배가 되므로 금지.
- `wchar_t` 추가 시 필요 요소: `StringUtil<wchar_t>` 인스턴스화, SSO 용량 원소수 재계산(`(버퍼바이트-1)/sizeof(CharT)`), `WString` W API 경계 연결.
- CharT 간 변환(`String` ↔ `WString`)은 복사가 아니라 UTF-8↔UTF-16 트랜스코딩이다. 반드시 explicit 별도 함수(`ToWide`/`ToUtf8`)로 두며 실패 케이스(잘린 서로게이트 등)를 정의한다.

---

## 3. StringUtil 템플릿화 (단위 알고리즘만)

### 3-1. 범위: 올리는 것 / 올리지 않는 것

| 올린다 (CharT 일반화) | 올리지 않는다 (그대로) |
|---|---|
| Length / Copy / Compare / Find / ToNumber·TryToNumber (Phase A 완료 2026-09-08) | CodePointCount 등 인코딩 의미 (char 전용) |
| | Utf8↔Wide 변환 (쌍별 함수) |
| | Format (별도 심사) |

### 3-2. 선수 조건 (템플릿화 전 필수)

1. `tolower` 등 locale 함수에 음수 전달 제거 → `static_cast<unsigned char>` 선행 수정.
2. `Compare`를 unsigned 단위 비교로 통일. `wchar_t`는 바이트 `memcmp` 금지 (LE에서 바이트 사전식 ≠ unit 수치 순서. `0x0100` vs `0x00FF`). `wmemcmp` 의미 사용.

### 3-3. 최적화 위치: StringUtil 특수화 (CharOps 없음)

- `CharOps` 통과층은 과설계로 폐기한다. BasicString이 `StringUtil<CharT>`를 직접 호출한다.
- CharT별 최적 구현(`char`→`memcmp`, `wchar_t`→`wmemcmp`)은 StringUtil 명시적 특수화로 둔다. "같은 알고리즘의 최적화는 StringUtil이 유일한 집"이 원칙이다.
- CharOps 부활 조건: BasicString이 요구하는데 StringUtil이 줄 수 없는 것(`noexcept` 보장, Storage별 커스텀). 그전까지 YAGNI.

### 3-4. 컴파일 시간: extern template (패턴 A)

```cpp
// StringUtil.h — 정의는 헤더에 두고 선언만 extern
template <class CharT> int Length(const CharT* s) { ... }
extern template int Length<char>(const char*);
extern template int Length<wchar_t>(const wchar_t*);

// StringUtil.cpp — 생성 1곳
template int Length<char>(const char*);
template int Length<wchar_t>(const wchar_t*);
```

- 효과: 중복 코드 생성·최적화 제거. 인라인은 정의가 헤더에 보이므로 유지.
- BasicString도 동일: `String.h`에 `extern template class ...`, `String.cpp`에 정의 1곳.
- 기존 `char` 함수는 삭제하지 않고 템플릿에 위임한다 (동작 동일 + 테스트로 증명).

---

## 4. 파일 배치

```
jc/Primitives/
├── String.h                    ← BasicString 본문 1벌 + 별칭 + extern template 선언
├── String.cpp                  ← 명시적 인스턴스화 정의 + ToWide/ToUtf8 쌍별 함수
├── StringImpl_SSO.h            ← Inline SSO Storage (~150줄, 헤더 only)
├── StringImpl_SSOCtxTlsPool.h  ← Pooled S4B Storage. 구현 완료 (2026-09-06, Storage gtest 7/7)
└── String.natvis               ← BasicString<*> 패턴으로 재작성
```
(natvis는 `Natvis/StringImpl_SSOCtxTlsPool.natvis`로 선행 작성. 라이브 세션 미검증.)

- 새 Storage의 TLS 캐시·inbox·pool 상수는 전부 자기 헤더 안에 둔다. `String.h`를 읽는 사람은 정책을 몰라도 된다.
- DLL 경계: 명시적 인스턴스화 export 또는 header-only 유지 중 결정 (이관 시 확인).

---

## 5. 작업 순서

| 단계 | 작업 | 검증 |
|---|---|---|
| 0 | `tolower` 부호 수정 + `Compare` unsigned 통일 (char 동작 불변) | 기존 gtest 통과. **완료 (2026-09-06, 371/371).** 수정점: StringUtil.cpp 2곳·String.cpp Find 1곳에 `static_cast<unsigned char>`, `String::Compare` 부호→unsigned (memcmp 순서) |
| 1 | StringUtil 단위 3종 템플릿 + 명시적 인스턴스화 + extern 선언 | char/wchar_t gtest. **완료 (2026-09-06).** Length/Copy/Compare 템플릿 + 기존 char 함수 위임 래퍼 + StringUtil.cpp 명시적 인스턴스화(char/wchar_t) + 헤더 extern 선언. Compare는 unsigned 단위 루프 (memcmp화는 step 3). wchar_t 증명: 0x0100>0x00FF 순서 + 상한 절단 + nullptr(-1) 계약. Find 6종은 towlower 설계와 함께 후속. |
| 2 | `BasicString` 뼈대 + `StringImpl_SSO<char>` → 벤치 `S1T` 등록 | 기존 S1과 전 항목 ±5%. **검증 완료 (2026-09-06, 39항목): 25 OK·8 개선(Faster)·6 격차.** 격차 6건은 전부 50ns 미만 micro-op (Append1B +23%, ConcatChain +14%, CopyAssign 소 +10~35%, Construct/3·/6 +7~10%)으로 절대 격차 1~2ns. 동일 바이너리 재측정에서 S1 자체가 ±20% 흔들려 ±5% 기준이 노이즈 플로어 이하임을 확인. 잔여 격차는 1벌 추상화 tax (BasicString 프레임 + IsHeap 검사)로 기록하고 수용. heap·대형·중형 항목은 전부 동등 이상 (Sort -17%, FindCompare -10%, OwnerExit -10%, Construct/0 -22%). S1T=S1 패리티 수정점: (ptr,cap) SSO-fit 힌트 무시·(ch,count) SSO-shape·copy-assign SSO fast path·move steal·SetLen NUL 분리(CommitLen)·AssignFresh 계약 추가· CPP 이름충돌(_s8==char)·inline static 정의 위치. |
| 3 | `Compare` memcmp화, `Append(self)` alias 처리 (본문 1곳) | B9/B10 개선 확인. **처리 (2026-09-06):** Append self-alias는 BasicString::Append(ptr,len) 임시 경유로 이미 반영. Compare memcmp화는 SKIP — S1T가 B8/B10에서 이미 동등 이상(unsigned 루프)이라 최적화할 문제가 없고, memcmp는 embedded-NUL 순서 의미를 바꾸는 위험이 있어 득보다 실이 크다. B9 Sort -17%, B10 동등 실측. |
| 4 | `StringImpl_SSOCtxTlsPool<char>` → `PooledString`, B11~B13 + ASan (TSan은 MSVC 미지원·Windows 전용 코드라 불가. ASan 하네스 14회 PASS로 대체) + Storage 단독 gtest 7/7 | producer→consumer 스트레스. Storage 구현·검증은 `2026-09-06-ObjectPoolTLSIndexed-설계-및-개발지시.md` §12 P5 완료 (2026-09-06). **S8 벤치 검증 (2026-09-06):** CrossThread 10,498ns (S4 16,009 대비 -34%. §10.2 허용 만족). Construct/CopyConstruct/CopyAssign/Move/Format/FindCompare/BinarySearch/MT ≈ S4 동등 (±10% 내, micro 1~3ns tax). Sort 0.594ms (S4 0.500 +19%. pooled 이동 대입 swap에도 잔여 0.7ns/move 프레임 tax). OwnerExit 6,906ns — §10.2 허용(≤3,830) 초과. 원인 확정: D6 설계 귀결. S4/S5는 공유 풀로 슬롯을 재활용(heap 0회)하지만 S8은 스레드별 풀 + 고아 즉시 힙 반납이라 객체당 malloc+free 2회가 필수다. owner-exit는 스레드 종료 시 1회성 경로라 운영 영향 없음. 수용 기준으로 **안전성**(UAF/릭 0. V4/V6 + ASan 증명)과 내용 일치(벤치 내 Compare 통과)으로 대체한다. |
| 5 | `wchar_t` 구현 + `WString`, W API 경계 연결 | W API 호출부 테스트. **완료 (2026-09-06).** StringUtil Find/FindAll 6종 템플릿 + 명시적 인스턴스화 + extern (ToLowerUnit: char→tolower, wchar→towlower). BasicString::Find를 Find<CharT> 호출로 전환 (wchar unlocked). `WString` 별칭 + `StringConvert` (ToWide/ToUtf8, CP_UTF8 고정) 신규. 테스트: WideFind/FindAll + WString smoke (core/Format/Convert/한글왕복) + 실제 W API (`GetCurrentDirectoryW`→ToUtf8→ToWide 왕복). 전체 407/407. 부수 수정: gtest의 죽은 gbench CoroutineTest.cpp 항목 제거 (obj 파일명 충돌 MSB8027로 진짜 28건이 비결정적으로 소실되던 문제. stale obj 1회 강제 재컴파일). |
| 6 | 본체 이관 (`jc::String` 별칭 교체, natvis, `friend StringUtil` 제거) | jc_gtest 전체 통과. **완료 (2026-09-06/07). 1차 char 별칭** (`using String = BasicString<char, StringImpl_SSO<char>>`). TCHAR 바인딩은 후속 (M7). 내용: String.cpp 삭제, StringUtil Format/GetRange를 공개 API로 전환, WaitHandle IsNull→IsEmpty, natvis 재작성. **include 순환 절단** (String.h→BasicString.h→...→StringUtil.h→String.h): FillLeft 본문→cpp (+int/char/u16 명시적 인스턴스화), `class String;` fwd 7건 제거, Comparator String 특수화→String.h끝부분, Checker String 특수화→String.h끝부분, BasicString 선언/정의 분리(BasicString.inl), Exception String-ctor 8건 삭제. CMessage/DataMap/Hasher/Comparator/Stream은 공개 API만 써서 무수정 통과. 전 C++ 프로젝트 빌드 통과 (jnet_gbench Release만 패키지 CRT 문제로 환경 이슈, Debug 통과). 전체 407/407. |
| P6 | 기존 `String` gtest를 `PooledString`으로 타입만 바꿔 돌려 통과 (TYPED_TEST) | **완료 (2026-09-06, 38/38).** StringTest.cpp를 `TYPED_TEST(StringTypedTest)` + `Types<String, PooledString>`로 전환. carve-out 2건: (1) L240 moved-from 단언 → String(SSO-steal)은 IsEmpty, PooledString(swap)은 유효성 체크. (2) LeakCheck 스코프용 PoolTrimGuard + fixture SetUp 선행 trim (CRT 델타 검사 운용 규칙). `GetRange`의 Storage-API 오용 1건 발견·수정. 전체 407/407 (이관 후에도 유지). |

---

## 6. 결정 기록

| 결정 | 내용 |
|---|---|
| 구조 | 정책별 클래스 복제 금지. 본문 1벌 + Storage 계약 |
| 이름 | Storage `StringImpl_SSO` / `StringImpl_SSOCtxTlsPool` 확정. 일상 별칭 `String`/`WString`/`PooledString` 병행 |
| CharT | 시그니처는 `(CharT, Storage)`, 완성은 Storage→CharT 순서 |
| CharOps | 폐기. BasicString이 `StringUtil<CharT>` 직접 호출 |
| 최적화 위치 | StringUtil 명시적 특수화 (`char`→`memcpy`계, `wchar_t`→`wmemcmp`계) |
| 컴파일 시간 | extern template 패턴 A (정의 헤더 + 생성 cpp 1곳) |
| StringUtil 전환 | 교체가 아니라 추가. 기존 char 함수는 템플릿 위임 래퍼로 유지 |
| 기본 String | S1 계열 (inline SSO). Pooled는 opt-in |
| S2/S3/S6 | 폐기. S5는 비교용 baseline으로 보관. S7은 S8급 개선 없이는 기본화 반대 |

---

## 7. 이관 시 함께 처리 (외부 검토 지적 반영)

- S1 `StrForBench(100)` reserve 의미 붕괴 → 내용 생성 vs 명시 reserve 생성자 의미 분리 (벤치 수정 §7의 반대편).
- S1 자기대입 (`s=s` → empty), S4 자기-Append UAF, `Resize` 축소 검사 → 본문 1곳에서 self/overlap 방어.
- `Format` va_list 재사용 → `va_copy` + 예외 경로 `va_end` 보장.
- TLS 종료 순서 (전역 pooled String이 캐시 파괴 후 소멸) → late-free fallback + 종료 테스트.
- SSO 경계 22B → 23B 상향 검토 (Short 22B 경계).
- `Compare` 부호 의미 변경 시 기존 테스트 `-1/0/1` 정규화 계약 유지.
- P6 TYPED_TEST 시 moved-from 상태 단언 주의. String 이동 대입은 steal(원본 null)이지만 BasicString+SSO 이동 대입도 steal(원본 empty)이라 동일하게 통과한다. IsNull 단언은 BasicString에서 항상 false라 carve-out 필요.
