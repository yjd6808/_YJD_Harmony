# Hasher Fnv1a 알고리즘 탑재 개편 — 계획서

> 작성일: 2026-08-23
> 목표: `jc::Hasher`에 해시 알고리즘을 탑재 가능하도록 구조 변경 + `Fnv1a64` 기능 통합

---

## 1. 현 구조 진단

| 위치 | 문제 |
|------|------|
| `jc/Hasher.h:24,154` | `Hasher<T>`(32bit)와 `Hasher64<T>`(64bit) 로직 완전 중복. `HashString:70`/`HashString64:201`도 중복 + `i+=step` 버그 + 플랫폼(`_WIN64`) 분기로 결정론성 깨짐 |
| `jc/Hasher.h:41-68,171-198` | `float/double` 특수화가 각자 `Hasher<_u32/_u64>`에 위임 — 알고리즘 교체 불가 |
| `sgf/Graphics/VertexDeclaration.h:107` | `inline Fnv1a64()`가 `jc` 외부에 고립. `VertexDeclaration.cpp:62` `HashAccumulate`, `ShaderProgram.cpp:188` 수동 `Offset/Prime` 하드코딩으로 중복 |
| `jc/Container/HashTable.h:24` | `THasher = Hasher<TKey>` 고정. 알고리즘 주입 불가 |

---

## 2. 목표 아키텍처 (A안: Policy 기반)

```
[HashAlgorithm]  (바이트 해시 순수 함수)
      ↓
[Hasher<T, Algo>] (타입→바이트 어댑터)  — Algo가 Result 타입 결정
      ↓
[HashBuilder<Algo>] (스트리밍 누적) + Alias
```

### 2.1 HashAlgorithm (무상태, constexpr)

```cpp
namespace jc::HashAlgorithm {
  struct Fnv1a32 { using Result=_u32; static constexpr Result kOffset=2166136261u; static constexpr Result kPrime=16777619u;
    static constexpr Result HashBytes(const void*, size_t) noexcept;
    static constexpr Result Combine(Result cur, _u8 byte) noexcept { return (cur ^ byte) * kPrime; }
    static constexpr Result HashIntegral(_u64 v) noexcept { return HashBytes(&v,sizeof(v)); }
  };
  struct Fnv1a64 { using Result=_u64; static constexpr Result kOffset=14695981039346656037ull; static constexpr Result kPrime=1099511628211ull; ... };
  struct SimplePrime32 { using Result=_u32; static constexpr Result kPrime=0x087b840F; static constexpr Result kXor=0x3e4dc77d; static constexpr Result kOffset=kPrime;
    static constexpr Result HashIntegral(_u64 v) noexcept { return ((_u64(v)^kXor)%kPrime)*kPrime; }
    static constexpr Result HashBytes(const void*, size_t) noexcept; // 바이트 루프: (h ^ (b ^ XorLow))*Prime
  };
  struct SimplePrime64 { ... };
}
```

### 2.2 Hasher<T, Algo>

```cpp
template<typename T, typename TAlgo = HashAlgorithm::SimplePrime32>
struct Hasher { using Result = typename TAlgo::Result; constexpr Result operator()(T v) const noexcept; };

template<typename Algo> struct Hasher<float, Algo>  { Result operator()(float) const noexcept; }; // bit cast → HashIntegral
template<typename Algo> struct Hasher<double, Algo> { ... };
template<typename T, typename Algo> struct Hasher<T*, Algo> { ... };
template<typename Algo> struct Hasher<const char*, Algo> { Result operator()(const String&) const; constexpr Result operator()(const char*) const; template<_u32 N> constexpr Result operator()(const char(&)[N]) const; };
template<typename Algo> struct Hasher<String, Algo> { ... };
```

* `Hasher<T>` 기본 Algo=`SimplePrime32` 유지 → 기존 `HashTable` 호환
* `Hasher64<T,Algo=SimplePrime64>` 별도 템플릿 유지 (기존 코드 `Hasher64<float>` 특수화 보존). 내부 로직은 `Hasher`와 동일하되 64bit 기본값.

### 2.3 HashBuilder<Algo> (스트리밍)

```cpp
template<typename TAlgo>
class HashBuilder {
  typename TAlgo::Result cur_ = TAlgo::kOffset;
public:
  void Append(const void* p, size_t n) noexcept { for each byte cur_ = TAlgo::Combine(cur_, byte); }
  template<typename T> void AppendValue(const T& v) noexcept { Append(&v,sizeof(v)); }
  void AppendString(const char* s) noexcept;
  typename TAlgo::Result Digest() const noexcept { return cur_; }
  void Reset() noexcept { cur_=TAlgo::kOffset; }
};
```

기존 `VertexDeclaration.cpp:62 HashAccumulate`와 `ShaderProgram.cpp:188-192` 수동 루프를 `HashBuilder<Fnv1a64>`로 교체.

### 2.4 VertexDeclaration / ShaderProgram 연동

* `VertexDeclaration.h:107 Fnv1a64()` → `jc::HashAlgorithm::Fnv1a64::HashBytes`로 포워딩 (deprecated 인라인 유지, 하위호환)
* `VertexDeclaration.cpp:62 HashAccumulate` 삭제 → `HashBuilder<Fnv1a64>`
* `ShaderProgram.cpp:170-198` → `HashBuilder<Fnv1a64> b; b.AppendString(semantic); b.AppendValue(index); ...`

---

## 3. 수정 대상

| 파일 | 작업 |
|------|------|
| `jc/Hasher.h` | 전체 재작성 (Algorithm + Hasher<T,Algo> + Hasher64<T,Algo> + HashBuilder). `HashString/HashString64`는 deprecated 래퍼로 유지 후 내부 `Algo::HashBytes` 위임 |
| `sgf/Graphics/VertexDeclaration.h` | `#include "jc/Hasher.h"` 추가, `Fnv1a64()`를 `HashAlgorithm::Fnv1a64::HashBytes` 포워딩으로 교체 |
| `sgf/Graphics/VertexDeclaration.cpp` | `HashAccumulate` 제거, `GetOrCreate` 해시 계산을 `HashBuilder<Fnv1a64>`로 교체 |
| `sgf/Graphics/ShaderProgram.cpp` | `_BuildInputSignature` 해시 누적을 `HashBuilder<Fnv1a64>`로 교체 |

손대지 않음: `HashTable.h` (기본 Hasher 유지), `PropertyTypeTraits.h` (기존 `Hasher64<const char*>()` 값은 SimplePrime64 기본값 유지로 호환)

---

## 4. 호환성 전략

* `Hasher<T>` → `Hasher<T, SimplePrime32>` 기본값으로 기존 호출 `Hasher<int>()(x)` 그대로 동작
* `Hasher64<T>` → `Hasher64<T, SimplePrime64>` 기본값 유지
* `Fnv1a64` 사용 시 명시: `Hasher<String, HashAlgorithm::Fnv1a64>()(s)` 또는 `HashBuilder<Fnv1a64>`
* `HashString/HashString64` free 함수는 `SimplePrime` Algo로 포워딩하는 deprecated 래퍼 유지
* `sgf::Fnv1a64()` 인라인은 `jc::HashAlgorithm::Fnv1a64::HashBytes`로 위임 — 기존 호출부 무수정 빌드

---

## 5. 검증

* 정적: `Hasher<int>`, `Hasher<float>`, `Hasher<String>`, `Hasher<const char*>`, `Hasher<T*>`, `Hasher64<...>` , `Hasher<String, Fnv1a64>` 컴파일 확인
* 동적: `jc_gtest HasherTest` 중복률 <50% 유지, `VertexDeclaration::GetOrCreate` 동일 입력 동일 포인터 반환, `VertexShader::InputSignatureHash` 기존 대비 결정론성 확인
* 빌드: `Scripts\BuildProject\jc.bat -P x64`, `sgf.bat -P x64`

---

## 6. 작업 순서

1. `jc/Hasher.h` 재설계
2. `sgf/VertexDeclaration.h/.cpp` 교체
3. `sgf/ShaderProgram.cpp` 교체
4. 빌드 및 테스트
