# 06. HLSL — 셰이더 언어와 상수 버퍼로 움직이는 삼각형

> 관련 코드: `Projects/sgfr/Sources/sgfr/06_HLSL/` (`06_HLSL_Main.cpp`, `06_HLSL_Function.cpp`)
> 관련 엔진: `Projects/sgf/Sources/sgf/Graphics/Shader.h`, `Buffers.h` (ConstantBuffer)
> 실행: sgfr 실행 후 콘솔 메뉴에서 **6번** 선택
> 선행 학습: 05. 렌더링 파이프라인

## 1. 이 장에서 배우는 것

GPU에서 실행되는 프로그래밍 언어 **HLSL**의 문법(타입, 시맨틱, 내장 함수)을 배우고, **상수 버퍼(cbuffer)**로 C++에서 GPU로 데이터를 보내는 방법을 익힙니다. 특히 C++ 구조체와 HLSL cbuffer의 **16바이트 정렬 규칙**이 컴파일 타임에 보장되는 메커니즘을 확인합니다. 결과적으로 **정점 데이터는 한 번도 안 바꾸고 GPU가 스스로 움직이는** 시간 기반 애니메이션을 만듭니다.

## 2. 실행하면 보이는 것

콘솔에 HLSL 핵심 요약이 출력된 뒤 창이 뜹니다. 삼각형이 **맥박처럼 커졌다 작아졌다** 하며(0.75~1.25 배율), 동시에 색이 **원래 색과 반전색 사이를 부드럽게 왕복**합니다. 배경은 어두운 회색(0.1, 0.1, 0.12)입니다. ESC로 종료합니다. 삼각형 자체는 05번과 같은 정점 데이터입니다.

## 3. 핵심 개념 — 비유로 먼저 이해하기

### 3-1. 셰이더 = GPU에서 도는 작은 프로그램

CPU 프로그램이 요리사 한 명이라면, GPU 셰이더는 **수천 명의 조리사**입니다. 정점 셰이더(VS)는 정점 하나마다 한 명의 조리사가 맡고, 픽셀 셰이더(PS)는 픽셀 하나마다 한 명의 조리사가 맡습니다. 모두 같은 레시피(셰이더 코드)로 일하지만, 재료(정점/픽셀의 데이터)가 각각 다릅니다. HLSL(High Level Shading Language)은 이 레시피를 쓰는 C언어와 비슷한 언어입니다.

### 3-2. 시맨틱 = 변수의 이름표

셰이더 함수의 입력/출력 변수에는 `POSITION`, `COLOR0`, `SV_POSITION` 같은 **이름표(시맨틱)**가 붙습니다. 이것은 "이 변수의 역할이 무엇인지" GPU에 알려주는 꼬리표입니다. 예를 들어 `SV_`가 붙은 값은 GPU가 특별 취급하는 시스템 값입니다 — `SV_POSITION`은 "이 값이 최종 클립 공간 좌표다"라는 뜻이고, `SV_TARGET`은 "이 함수의 결과가 픽셀 색이다"라는 뜻입니다.

### 3-3. 상수 버퍼 = 정점 모두가 공유하는 메시지 보드

정점 버퍼는 "정점마다 다른 값"을 담습니다(각 정점의 위치/색). 반면 상수 버퍼는 **모든 정점이 공유하는 값**을 담는 통로입니다 — 교실 앞에 걸린 메시지 보드와 같습니다. 교사(C++)가 "지금 시간은 5초!"라고 보드에 적으면 모든 학생(정점/픽셀)이 같은 값을 보고 각자 일합니다. 이 장의 핵심 차별점이 이것입니다: 정점 데이터는 그대로인데, 시간 값만 매 프레임 바뀌므로 삼각형 전체가 한꺼번에 움직입니다.

### 3-4. 16바이트 정렬 = 버스를 타기 위한 좌석 배정

GPU 상수 버퍼는 데이터를 16바이트 단위로 묶어 처리합니다. `float` 하나(4바이트)만 보내도 나머지 12바이트는 빈 좌석(패딩)으로 채워 총 16바이트로 맞춰야 합니다. 버스의 좌석이 4명씩 한 줄로 고정된 것과 같습니다 — 1명이 예약해도 4인석 한 줄을 씁니다. 이 규칙을 지키지 않으면 C++과 HLSL의 메모리 배치가 어긋나고, 셰이더가 잘못된 값을 읽습니다. sgf의 `ConstantBuffer<T>`는 `static_assert`로 C++ 구조체가 16바이트 배수인지 컴파일 타임에 강제합니다.

## 4. 초보자 용어 사전

| 용어 | 뜻(쉬운 말) | 비유 |
|---|---|---|
| HLSL | GPU용 C유사 언어 (High Level Shading Language) | GPU 요리 레시피 언어 |
| 셰이더 (Shader) | GPU에서 실행되는 작은 프로그램 | 조리사 여러 명의 레시피 |
| 시맨틱 (Semantic) | 변수의 역할을 나타내는 꼬리표 | 이름표 |
| SV_POSITION / SV_TARGET | GPU가 특별 취급하는 시스템 시맨틱 | 특수 출입증 |
| cbuffer (Constant Buffer) | 셰이더 공유 전역 변수 통로 | 교실 메시지 보드 |
| register(b0) | 상수 버퍼를 꽂는 슬롯 번호 | 보드 걸이 번호 0번 |
| lerp | a와 b 사이를 t 비율로 선형 보간 | 물감 섞기 비율 |
| 패딩 (Padding) | 정렬 맞추기 위한 빈 공간 | 버스 빈 좌석 |
| static_assert | 컴파일 시점 검사 | 출고 전 자동 검사기 |
| DYNAMIC 버퍼 | CPU가 매 프레임 갱신 가능한 버퍼 | 매일 내용 바뀌는 보드 |

## 5. 코드 워크스루

`06_HLSL_Main.cpp`의 `HLSL_Main()`과 Function.cpp의 `AnimatedShaderSource()`를 따라갑니다.

### 5-1. C++ 쪽 상수 버퍼 구조체 (코드 31~38행)

```cpp
// 셰이더 CbTime(b0)과 메모리 배치 일치 (16바이트)
// ConstantBuffer<T>는 sizeof(T)가 16의 배수가 아니면 컴파일 에러를 낸다.
struct CbTime
{
	_f32 time_;			// 경과 시간(초)
	_f32 padding_[3];	// 16바이트 정렬용 (HLSL의 gPadding과 대응)
};
```

`time_(4바이트) + padding_[3](12바이트) = 16바이트`로 정확히 16의 배수입니다. 만약 `padding_`을 빼고 4바이트짜리 구조체를 만들면 `ConstantBuffer<CbTime>::Create`의 `static_assert(sizeof(T) % 16 == 0)`이 컴파일을 막습니다.

### 5-2. 셰이더 소스: HLSL 쪽 cbuffer (Function.cpp)

```hlsl
cbuffer CbTime : register(b0)
{
	float  gTime;		// 경과 시간(초). C++에서 매 프레임 갱신한다.
	float3 gPadding;	// 16바이트 정렬용 빈 공간 (사용하지 않음)
};
```

- `register(b0)`의 **0이 슬롯 번호**입니다. C++의 `cbTime.UpdateAndBind(&device, cb, 0)`에서 마지막 인자 `0`이 같은 슬롯을 가리켜 둘이 연결됩니다.
- `gTime`과 `gPadding`은 C++의 `time_`/`padding_[3]`과 메모리 배치가 1:1로 일치합니다.

### 5-3. 정점 셰이더: 시간 기반 맥박 (Function.cpp)

```hlsl
VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	float scale = sin(gTime * 2.0f) * 0.25f + 1.0f;
	output.position = float4(_input.position * scale, 1.0f);
	output.color = _input.color;
	return output;
}
```

`sin(gTime * 2.0f)`은 -1~+1을 오가므로 `* 0.25 + 1`로 **0.75~1.25 배율**이 됩니다. 이 배율을 정점 위치에 곱하면 삼각형이 맥박치듯 커졌다 작아집니다. 정점 배열 자체는 건드리지 않습니다 — 위치 변환이 전부 GPU에서 일어납니다.

### 5-4. 픽셀 셰이더: 색 반전 왕복 (Function.cpp)

```hlsl
float4 PSMain(VSOutput _input) : SV_TARGET
{
	float t = sin(gTime * 3.0f) * 0.5f + 0.5f;		// 0~1 왕복
	float3 inverted = 1.0f - _input.color.rgb;		// 반전색
	float3 mixed = lerp(_input.color.rgb, inverted, t);
	return float4(mixed, _input.color.a);
}
```

- `lerp(a, b, t)`: t=0이면 a, t=1이면 b, 중간이면 그 사이 값 (선형 보간 내장 함수).
- `1.0f - rgb`는 원래 색의 반전색(빨강 → 청록 등)입니다.
- 시간에 따라 t가 0↔1을 오가며 원색과 반전색 사이를 부드럽게 섞습니다.

### 5-5. 렌더 루프에서의 상수 버퍼 갱신 (코드 111~121행)

```cpp
device.BeginFrame(color(0.1f, 0.1f, 0.12f, 1.0f));

CbTime cb = {};
cb.time_ = elapsed;
cbTime.UpdateAndBind(&device, cb, 0);	// register(b0)에 연결

vb.Bind(&device);
shader.Bind(&device);
device.Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
device.Context()->Draw(3, 0);
```

`UpdateAndBind` 내부(Buffers.h)는 `Map(D3D11_MAP_WRITE_DISCARD)`로 GPU 메모리를 CPU 주소 공간에 연결해 `memcpy`로 새 값을 넣고 `Unmap`한 뒤, `VSSetConstantBuffers(b0)`와 `PSSetConstantBuffers(b0)`로 VS/PS 양쪽 슬롯에 장착합니다. `WRITE_DISCARD`는 "이전 내용은 버릴 테니 새 메모리를 달라"는 의미라, GPU가 이전 데이터를 읽는 중이어도 CPU가 멈추지 않고 진행됩니다(빠름). 한 번에 두 호출을 하는 이유는 어느 스테이지에서든 `gTime`을 읽을 수 있게 하기 위해서입니다.

### 5-6. 정리 (코드 127~128행)

`device.Finalize()` → `window.Destroy()`.

### 사용된 sgf API 정리

| API | 역할 |
|---|---|
| `ConstantBuffer<T>::Create(&device)` | DYNAMIC 상수 버퍼 생성 (16바이트 배수 static_assert) |
| `ConstantBuffer<T>::UpdateAndBind(&device, data, slot)` | Map/Unmap으로 내용 갱신 + VS/PS 양쪽 바인딩 |
| `Shader::CompileFromString(&device, hlsl, layoutDescs, count)` | HLSL 런타임 컴파일 |
| `Shader::Bind(&device)` | VS/PS/입력레이아웃 장착 |
| `VertexBuffer::Create/Bind` | 정점 버퍼 생성/장착 |
| `FrameTimer::Tick/DeltaTime` | 경과 시간 측정 |
| `device.Context()->Draw(3, 0)` | 삼각형 그리기 |

## 6. GPU와 DX11, 하드웨어 깊이 보기

### 6-1. 상수 버퍼의 하드웨어 실체: GPU 상수 메모리와 register 파일

D3D11의 상수 버퍼는 GPU에서 **상수 레지스터(Constant Register) 파일**에 매핑되는 특별한 메모리입니다. 셰이더 유닛은 명령 실행 중 `cbuffer` 값을 읽을 때 일반 VRAM 접근(수백 사이클, L2 캐시 경유)보다 훨씬 빠른 **상수 캐시/상수 레지스터 파일**을 사용합니다. 즉 `gTime`을 읽는 것은 셰이더 명령의 즉시 상수(immediate constant)에 가까운 속도로 처리됩니다. 이것이 "정점마다 다른 값은 버퍼, 모두가 공유하는 값은 상수 버퍼"로 나누는 이유입니다 — 하드웨어 접근 경로가 완전히 다릅니다.

### 6-2. 16바이트 정렬의 하드웨어 근거: 버스 폭과 벡터 레지스터

상수 버퍼가 16바이트 단위로 정렬되어야 하는 이유는 GPU 하드웨어의 메모리 버스와 벡터 연산 유닛이 **16바이트(128비트) 폭**을 기본 단위로 하기 때문입니다. GPU는 float4(128비트) 로드 명령을 표준으로 사용하며, 상수 버퍼의 데이터는 이 128비트 그리드에 정렬되어 있어야 효율적인 단일 로드 명령으로 읽힙니다. 16바이트에 걸치지 않는 float 하나를 읽으려면 하드웨어가 두 번 나눠 읽는 수가 있습니다. 그래서 `float gTime; float3 gPadding;`이 정확히 16바이트로 맞춰지는 것이며, `static_assert(sizeof(T) % 16 == 0)`은 이 하드웨어 요구를 컴파일 타임에 강제하는 안전장치입니다.

### 6-3. Map/Unmap과 WRITE_DISCARD의 메모리 경로: GPU 가시 메모리

`UpdateAndBind`의 Map은 CPU가 GPU 메모리를 쓸 수 있게 하는 API입니다. D3D11의 DYNAMIC 상수 버퍼는 내부적으로 CPU와 GPU 양쪽이 접근 가능한 메모리(통합 메모리 시스템 또는 PCI Express 매핑 영역)를 사용합니다. `D3D11_MAP_WRITE_DISCARD`는 매 프레임 **새 메모리 조각을 할당**해 기존 데이터와의 동기화 대기를 없애는 기법입니다. GPU가 지난 프레임의 버퍼를 아직 읽는 중이어도, CPU는 다른 조각에 새 값을 쓸 수 있으므로 파이프라인이 멈추지 않습니다. 이 "쓰기 버리기" 패턴은 모든 실전 렌더러에서 상수 갱신의 표준 관용구입니다.

### 6-4. 셰이더 컴파일의 실제: 바이트코드 → 마이크로코드 변환

`CompileFromString`은 HLSL 소스를 런타임에 컴파일해 DXBC(바이트코드)로 만들고, `ID3D11VertexShader`/`ID3D11PixelShader` 객체를 생성합니다. 여기서 한 단계 더 들어가면, GPU 드라이버는 DXBC를 받아 자신의 GPU 아키텍처에 맞는 **마이크로코드(마이크로연산 시퀀스)**로 변환합니다. `sin` 같은 함수는 GPU의 SFU(Special Function Unit) 명령으로, `lerp`는 FMA 두 개로, `1.0f - color.rgb`는 단순 감산 명령으로 최적화됩니다. 즉 이 장에서 쓴 다섯 줄짜리 셰이더는 실제로는 하드웨어 명령 몇 개로 탈바꿈해 수천 개의 코어에서 병렬 실행됩니다.

### 6-5. VS/PS 양쪽 바인딩의 이유: 셰이더 스테이지별 상수 슬롯의 분리

`UpdateAndBind`가 `VSSetConstantBuffers`와 `PSSetConstantBuffers` 둘 다를 호출하는 이유는, D3D11에서 VS와 PS가 **서로 다른 상수 슬롯 공간**을 갖기 때문입니다. 슬롯 번호 0은 VS의 b0와 PS의 b0가 별개입니다. 실전에서는 VS 전용(변환 행렬), PS 전용(조명 색) 상수 버퍼를 각각 다른 슬롯에 배치해 리소스 갱신 횟수를 줄이는 최적화를 합니다. 이 슬롯 배치 규약이 12번 "셰이더 스테이지와 상수버퍼" 튜토리얼의 핵심 주제입니다.

### 6-6. 시간 기반 애니메이션의 하드웨어 흐름

`elapsed`(CPU) → `UpdateAndBind`(PCIe/통합 메모리를 통한 GPU 메모리 복사) → 셰이더가 상수 캐시에서 `gTime` 로드 → VS에서 `sin(gTime*2)` 계산(SFU 명령) → 정점 위치 스케일 → RS 보간 → PS에서 반전색과 lerp 혼합 → 백버퍼 기록. 이 전체 흐름에서 **정점 버퍼는 한 번도 변경되지 않습니다.** 이것이 "데이터는 불변이고, 매개변수만 바뀐다"는 GPU 렌더링의 핵심 패턴이며, 실전 게임에서도 캐릭터 모델은 그대로 두고 월드 변환 행렬만 매 프레임 갱신하는 식으로 확장됩니다.

### 6-7. 상수 버퍼 슬롯의 한계와 성능 규율

D3D11의 상수 버퍼 슬롯은 스테이지당 **최대 15개(b0~b14)**이며, sgf의 `GraphicContext`는 `MAX_CBUFFER_SLOTS = 8`로 제한합니다. 슬롯이 부족해지지 않도록 실전 렌더러는 "변환 행렬은 b0, 조명 파라미터는 b1, 머티리얼 색은 b2"처럼 **슬롯 할당 규약**을 정합니다 — 12번 튜토리얼의 주제입니다. 또 갱신 비용도 생각해야 합니다. 매 프레임 갱신되는 버퍼와 거의 안 바뀌는 버퍼를 구분해서, 불변 데이터(예: 화면 크기)는 프레임마다 다시 Map하지 않는 것이 규율입니다. 상수 버퍼는 "작지만 자주 바뀌는" 통로라는 특성이 하드웨어 접근 경로와 함께 이 모든 규칙을 만들었습니다.

## 7. 핵심 규칙 요약

- [ ] HLSL 기본 타입: float / float2 / float3 / float4 / float4x4.
- [ ] 시맨틱: `POSITION`, `COLOR0` 등은 입력/출력 변수의 역할 꼬리표, `SV_` 접두사는 GPU 특별 취급.
- [ ] `SV_POSITION` = 클립 공간 좌표 출력, `SV_TARGET` = 픽셀 색 출력.
- [ ] cbuffer는 모든 정점/픽셀이 공유하는 값, `register(b0)`의 0 = 슬롯 번호.
- [ ] **16바이트 배수 규칙**: `float gTime` 뒤에 `float3 gPadding` 필요. C++ 측도 동일 배치.
- [ ] `ConstantBuffer<T>::Create`의 static_assert로 컴파일 타임 강제.
- [ ] `sin * 0.25 + 1` = 0.75~1.25 배율, `sin * 0.5 + 0.5` = 0~1 (lerp 계수).
- [ ] `UpdateAndBind(&device, cb, slot)` = Map(WRITE_DISCARD) + memcpy + Unmap + VS/PS 바인딩.
- [ ] 정점 데이터는 불변, 상수 버퍼만 갱신 → GPU가 애니메이션을 수행.

## 8. 직접 해보기 — 실험으로 확인하기


1. PSMain에서 `return float4(_in.color.rgb * 0.5f, 1);`로 바꿔 전체를 어둡게 해보세요.
2. VSMain에서 `o.position.x += sin(gTime) * 0.3f;`를 넣어 삼각형을 흔들어보세요.
3. 일부러 문법 에러를 내고 콘솔의 컴파일 에러 메시지를 읽어보세요. 줄 번호가 어디를 가리키나요?

## 9. 다음 단계로

07번에서 **인덱스 버퍼**를 배워 사각형을 그립니다. 상수 버퍼의 16바이트 규칙은 이후 모든 튜토리얼(12번에서 슬롯 규약으로, 10·11번에서 변환 행렬 전송으로)에서 반복됩니다. 미리 알면 좋은 것: 상수 버퍼에 나중에 `mat4` 변환 행렬을 넣을 텐데, sgf는 row-major이므로 HLSL에서 `row_major float4x4`로 선언해 전치 없이 복사합니다.
