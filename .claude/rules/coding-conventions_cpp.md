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
```
