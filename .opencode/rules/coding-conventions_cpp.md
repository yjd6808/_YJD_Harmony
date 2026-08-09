---
paths:
  - "Projects/**/*.cpp"
  - "Projects/**/*.h"
  - "Projects/**/*.inl"
---

# Coding Conventions

## 네이밍 규칙

| 대상 | 규칙 |
|------|------|
| private 멤버 변수 (non-static) | suffix `_`, camelCase (예: `health_`, `playerInfo_`) |
| 포인터 파라미터 / 스마트포인터 파라미터 | prefix `_p`, camelCase (예: `_pPlayer`) |
| 일반 파라미터 | prefix `_`, camelCase (예: `_level`) |
| 포인터 로컬 변수 / 스마트포인터 로컬 변수 | prefix `p`, camelCase (예: `pPlayer`) |
| 일반 로컬 변수 | camelCase (예: `playerCount`) |
| constexpr 로컬 변수 | UPPER_CASE (예: `MAX_PLAYER_COUNT`) |

**금지 접두사:** `dw`, `ui`, `i`, `v` — 로컬/파라미터/private 멤버 모두에 사용 금지 (예: `iCount` X → `count` O)

## 포매팅

- **Allman 스타일** 중괄호 사용
- `return`, `continue`, `break` 단일 제어문은 중괄호 생략 가능

```cpp
if (condition)
{
    DoSomething();
}

if (value == 0)
    return;
```

## 가상 함수 규칙

- 가상 함수: 반드시 `virtual` 명시
- 오버라이드 함수: 반드시 `virtual` + `override` 모두 명시

```cpp
virtual void Update();               // 기반 가상 함수
virtual void Update() override;      // 오버라이드 함수
```

## 함수 시각적 구분자 (.cpp 전용)

각 함수 앞에 `/` 문자로 구성된 구분자 삽입. 길이 = `90 - (들여쓰기 레벨 × 4)`

```cpp
// 들여쓰기 레벨 0 (길이 90):
//////////////////////////////////////////////////////////////////////////////////////////
void Function() { }

// 들여쓰기 레벨 1 (길이 86):
namespace NA
{
    //////////////////////////////////////////////////////////////////////////////////////
    void Function() { }
}

// 들여쓰기 레벨 2 (길이 82):
namespace NA
{
    class CA
    {
        //////////////////////////////////////////////////////////////////////////////////
        void Function() { }
    }
}

## JCore(jc) 프로젝트 전용 규칙

jc 계열(Projects/jc) 소스에서 확인된 추가 규칙. 위 일반 규칙과 충돌 시 아래 규칙을 우선한다.

### 기본 자료형

`Type.h`에 정의된 별칭 타입을 사용한다. **원시 타입(int, unsigned int 등) 직접 사용 금지.**

| 타입 | 별칭 | 타입 | 별칭 |
|------|------|------|------|
| long long | `_s64` | unsigned long long | `_u64` |
| int | `_s32` | unsigned int | `_u32` |
| long | `_s32l` | unsigned long | `_u32l` |
| short | `_s16` | unsigned short | `_u16` |
| char | `_s8` | unsigned char | `_u8` |
| float | `_f32` | double | `_f64` |
| long double | `_f64l` | wchar_t | `_s16c` |
| 포인터 크기 정수 | `_ptr`, `_sz` | object id | `object_id` |

- 부호 있음 `_s + 비트수`, 부호 없음 `_u + 비트수`, 실수 `_f + 비트수`
- 핸들 타입은 `_whandle`, `_wmodule`, `_iohandle` (void* alias)

### 파일 헤더 주석

모든 소스/헤더 첫 줄에 작성자 주석을 단다. 두 형식 중 하나 (아무거나 허용).

```cpp
/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 4:04:50 AM
 * =====================
 *
 */
```

```cpp
/*
	작성자 : 윤정도
	클래스 설명 (선택)
*/
```

### 네임스페이스 처리

`Namespace.h`의 매크로를 사용한다. **namespace 중괄호 내부를 들여쓰지 않는다.**

```cpp
NS_JC_BEGIN          // namespace jc {
NS_DETAIL_BEGIN      // namespace detail {   (중첩 네임스페이스용)
NS_END               // }
```

```cpp
USING_NS_JC          // using namespace jc
USING_NS_STD         // using namespace std
USING_NS_STD_CHRONO  // using namespace std::chrono
```

- 파일 안 코드는 `NS_JC_BEGIN` 블록 안에 넣고, 파일 끝에 `NS_END`로 닫는다.
- 내부 구현 전용은 `jc::detail` 네임스페이스를 쓴다.

### 들여쓰기

- **탭(Tab) 사용** (탭 1개 = 공백 4). `.editorconfig` 기준 `indent_style = tab, tab_width = 4`
- 매크로 함수 정의의 연속 라인 정렬은 탭 정렬을 사용한다.

### 클래스/구조체/인터페이스 네이밍

| 대상 | 규칙 | 예시 |
|------|------|------|
| 클래스 | PascalCase | `Vector`, `Memory`, `Stream` |
| 레거시/유틸성 클래스, 비템플릿 클래스 | `C` 접두사 | `CMessage`, `CDataMap`, `CDefaultAllocator`, `CVoidOwner` |
| 인터페이스 (가상 전용) | `I` 접두사 + `JC_NOVTABLE` | `struct JC_NOVTABLE IPropertyValueBinaryOperator` |
| 구조체 | PascalCase, `struct` 사용 | `Pair`, `BucketNode`, `TreeNode` |
| 다중 상속/가상화 안 하는 클래스 | `final` 명시 | `class Memory final` |
| 템플릿 타입 파라미터 | `T` 접두사 PascalCase | `T`, `TKey`, `TValue`, `TAllocator`, `TPredicate` |
| 클래스 내부 타입 별칭 | `T` 접두사 + 대상 이름 | `using TCollection`, `using TVector`, `using TEnumerator` |

### enum 규칙

- 일반 enum은 `enum class`를 사용한다. (멤버는 PascalCase, `ContainerType::Vector`)
- **enum 멤버 네이밍**:
  - `enum class` 멤버: PascalCase (`BlendState::Opaque`)
  - 일반 `enum` 멤버: `bl` 접두사 + PascalCase (`BlendState`이면 `blOpaque`)
- enum class + 비트플래그 연산은 `JC_ENUM_CLASS_BIT_OPERATION_OVERLOADING(enum_name)` 매크로로 연산자 오버로딩을 생성한다.
- 타입명이 겹치는 것을 피하기 위해 `JC_SENUM_*` 매크로로 enum을 struct 안에 감싸 쓰기도 한다.
  - 결과 타입 별칭은 항상 `enum명##_t`로 정의됨 (`JC_SENUM_MIDDLE/END` 매크로) → 사용처는 `PropertyType_t`처럼 `_t` 접미.

### 상수

- 클래스/함수 내 상수: `static constexpr` + `UPPER_SNAKE` (`DEFAULT_CAPACITY`, `INVALID_OFFSET`)
- 전역 상수/사용자 지정 상수: `#define` + `UPPER_SNAKE` (`READ_BUFFER_SIZE`, `MAX_PLAYER_COUNT`)
- 로컬 const: 기존 라이선스대로 `const` 앞에 붙이며, 굳이 `k` 접두사 채택은 유지하되 신규 코드는 `UPPER_SNAKE` 권장 (`kiDataTypeSize` 레거시)

### 매크로 규칙

- 공개 매크로는 반드시 UPPER_CASE
- 어노테이션 매크로: `IN`, `OUT`, `IN_OUT`, `IN_OPT`, `OUT_OPT`, `NOT_NULL`, `NULLABLE`, `UNUSED` (Define.h 정의, 코딩 계약 표기에만 사용)

### 함수 네이밍

- 함수/메서드: **PascalCase** (`ReadInt32`, `PushBack`, `CalcU32_LEB128`)
- 게터/세터: `Get##이름()`, `Set##이름()` 패턴 (Define.h의 `JC_GETTER`/`JC_SETTER` 매크로 활용 가능)
- 오버라이드 `virtual ... override` (일반 규칙과 동일)
- .cpp 구현 함수 앞에는 일반 규칙의 `/` 구분자.

### assert / 예외

- 어서션: `jc_assert(조건)`, 메시지 포함 `jc_assert_msg(조건, "메시지", ...)` — Debug 빌드에서만 동작
- 예외는 jc 예외 타입 사용: `RuntimeException`, `InvalidArgumentException`, `InvalidOperationException`
- 예외 메시지는 한국어로 작성하는 것을 기본으로 한다.

### 포함(include) 규칙

- 헤더 가드는 `#pragma once` 사용. (Define.h 등 일부 매크로 헤더만 `#if !defined(...)` 스타일)
- 경로는 프로젝트 루트 기준 상대 경로: `"jc/Type.h"`, `"jc/Container/Vector.h"`
- 프로젝트 헤더 우선, 그 다음 서브시스템 헤더, 마지막으로 표준 라이브러리.
- PCH는 `jc/PCH/Core.h`를 경유한다.

### 주석 스타일

- 주석은 기본 **한국어**로 작성
- `//` 한 줄 주석, `/* ... */` 블록 주석 허용
- doxygen 스타일: `/// <summary>` 또는 `/** \brief ... */` 허용 (재사용성 언급 주석에 사용)
- 기존 라이선 주석은 남기되, 신규 코드는 위 형식을 준수한다.
