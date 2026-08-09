# 05. HLSL — GPU에서 돌아가는 나만의 미니 프로그램

> 관련 코드: `Projects/sgf/Sources/sgf/Graphics/Shader.h/.cpp`, `Buffers.h`(ConstantBuffer), `Projects/sgfr/Sources/sgfr/05_HLSL/`
> 실행: sgfr 메뉴에서 **5번**

---

## 1. HLSL이란?

HLSL(High Level Shading Language)은 **GPU에서 실행되는 코드를 작성하는 언어**입니다.
C와 비슷하게 생겼지만 몇 가지 특징이 있습니다.

- `float2`, `float3`, `float4`, `float4x4` 같은 벡터/행렬 타입이 기본 제공
- `sin`, `cos`, `lerp`, `saturate`, `mul` 같은 수학 함수 내장
- 변수 뒤에 붙는 **시맨틱(Semantic)**: `: POSITION`, `: SV_POSITION` 등 — "이 값의 역할이 무엇인지" GPU에 알려주는 꼬리표

### 자주 쓰는 시맨틱

| 시맨틱 | 의미 |
|---|---|
| `POSITION`, `COLOR`, `TEXCOORD`, `NORMAL` | 입력 레이아웃과 짝을 맞추는 이름표 |
| `SV_POSITION` | VS 출력 중 "이게 최종 화면 위치다" (SV = System Value, GPU가 직접 사용) |
| `SV_TARGET` | PS 출력 중 "이게 백버퍼에 기록할 최종 색이다" |

---

## 2. 상수 버퍼 (Constant Buffer) — CPU에서 GPU로 보내는 편지

04번 삼각형은 정점 데이터가 고정이라 영원히 그 모양 그대로였습니다.
매 프레임 바뀌는 값(시간, 행렬, 빛 방향…)을 셰이더에 전달하려면 **상수 버퍼**를 씁니다.

```hlsl
// HLSL 쪽 — b0 슬롯으로 받겠다는 선언
cbuffer CbTime : register(b0)
{
	float  gTime;     // 경과 시간 (초)
	float3 gPadding;  // 16바이트 정렬을 위한 빈 공간
};
```

```cpp
// C++ 쪽 — 같은 모양의 구조체를 만들어 매 프레임 전송
struct CbTime
{
	_f32 time_;
	_f32 padding_[3]; // HLSL의 float3 gPadding과 짝 맞추기
};

ConstantBuffer<CbTime> cb;
cb.Create(&device);

// 매 프레임:
CbTime data;
data.time_ = 경과시간;
cb.UpdateAndBind(&device, data, 0); // b0 슬롯에 업데이트+바인딩 (VS/PS 모두)
```

### ⚠ 16바이트 규칙 (가장 많이 틀리는 부분!)

상수 버퍼 크기는 반드시 **16바이트의 배수**여야 하고, HLSL의 변수 배치 규칙(float4 경계를 넘어가면 다음 칸으로)도 C++ 구조체와 일치해야 합니다.
그래서 float 하나만 보내더라도 12바이트 padding을 채워 16으로 맞춥니다.
sgf의 `ConstantBuffer<T>`는 `static_assert(sizeof(T) % 16 == 0)`로 실수를 컴파일 단계에서 잡아줍니다.

---

## 3. 이번 튜토리얼의 셰이더 분석

05번은 시간을 상수 버퍼로 보내 삼각형을 **숨쉬듯 커졌다 작아졌다** 하게 하고, 색도 시간에 따라 반전시킵니다.

### VS: 정점 위치를 시간으로 조작

```hlsl
VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	// sin은 -1~+1을 오가는 파도. *0.25 + 1을 하면 0.75~1.25 범위의 배율이 된다.
	float scale = sin(gTime * 2.0f) * 0.25f + 1.0f;
	output.position = float4(_input.position * scale, 1.0f);
	output.color = _input.color;
	return output;
}
```

> **이 패턴을 외우세요**: `sin(시간) * 진폭 + 중심값`
> "어떤 값을 주기적으로 흘들기"의 만능 공식입니다. 깔빡임, 호흡, 부유우는 UI 등 어디에든 쓰입니다.

### PS: 색을 시간으로 조작

```hlsl
float4 PSMain(VSOutput _input) : SV_TARGET
{
	// t가 0~1을 오가면서 원래 색과 반전색 사이를 왛가다 갫다 한다.
	float t = sin(gTime * 3.0f) * 0.5f + 0.5f;
	float3 inverted = 1.0f - _input.color.rgb;             // 보색 (반전색)
	float3 finalColor = lerp(_input.color.rgb, inverted, t); // 선형 보간
	return float4(finalColor, _input.color.a);
}
```

> `lerp(a, b, t)` = `a + (b - a) * t`. t=0이면 a, t=1이면 b, 0.5면 중간. 그래픽스의 밥숙같은 함수입니다.

---

## 4. sgf의 HLSL 규약 복습

- 행렬은 항상 `row_major float4x4`로 선언 (C++과 메모리 배치 통일 → transpose 불필요)
- 벡터×행렬은 `mul(vector, matrix)` 순서 (행 벡터 규약)
- 셰이더 진입점 이름은 `VSMain` / `PSMain`
- 셰이더 소스는 별도 파일 대신 C++ 안의 **raw string literal** `R"(...)"`로 보관 — 파일 경로 문제 없이 학습용으로 간편

```cpp
const char* AnimatedShaderSource(); // 05_HLSL_Function.cpp — 셰이더 소스 반환
shader.CompileFromString(&device, AnimatedShaderSource(), pDescs, layoutCount);
```

---

## 5. 튜토리얼(5번)에서 직접 확인할 것

1. 삼각형이 숨쉬듯 커졌다 작아집니다. (VS에서 위치 조작)
2. 색이 원래 색 ↔ 반전색으로 부드럽게 왛가다 갫다 합니다. (PS에서 색 조작)
3. 콘솔에 HLSL 핵심 개념 요약이 출력됩니다.
4. 실험 제안: 숫자들(진폭, 속도)을 바꿔가며 움직임이 어떻게 변하는지 관찰하세요.

---

## 6. 자주 만나는 오류

| 증상 | 원인 |
|---|---|
| 컴파일 실패 (콘솔에 오류 출력) | HLSL 문법 오류. sgf는 오류 메시지를 콘솔에 그대로 출력해줌 |
| 값이 이상하게 깨진 채 동작 | C++ 구조체와 cbuffer의 메모리 배치 불일치 (16바이트 규칙!) |
| 아무것도 안 그려짐 | 상수 버퍼 슬롯 번호(bN)와 UpdateAndBind의 slot 인자 불일치 |

---

## 7. 정리

| 개념 | 한 줄 요약 |
|---|---|
| HLSL | GPU에서 돌아가는 C 닮은 언어 |
| 시맨틱 | 값의 역할 꼬리표 (SV_POSITION, SV_TARGET) |
| 상수 버퍼 | CPU → GPU로 매 프레임 보내는 작은 데이터 |
| 16바이트 규칙 | cbuffer 크기/배치는 16바이트 단위 |
| sin 패턴 | `sin(시간)*진폭+중심` = 주기적 흘들기 |
| lerp | 두 값 사이를 t로 섮기 |

다음 문서: **06. 정점/인덱스 버퍼** — 사각형을 그리며 중복 정점을 없애는 법을 배웁니다.
