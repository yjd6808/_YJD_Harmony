# 34. GraphicsEnum 한눈에 비교 — 8종 렌더 상태를 좌우에서 바로 비교하기

> 관련 코드: `Projects/sgfr/Sources/sgfr/Tutorial/34_GraphicsEnum_한눈에_비교/` (`34_GraphicsEnumDiff_Main.cpp`, `34_GraphicsEnumDiff_Main.h`)
> 엔진 코드: `Projects/sgf/Sources/sgf/Graphics/GraphicsEnums.h` (열거형 정의), `GraphicDevice.cpp` (`SetBlendMode`/`SetSampler`/`SetCullMode`/`SetWireframe`/`SetDepthTest`), `RenderStates.cpp` (`GetBlendState`/`GetSamplerState`/`GetRasterizerState`/`GetDepthState`), `GraphicContext.cpp` (바인딩 캐시)
> 실행: sgfr 실행 후 콘솔 메뉴에서 **34번** 선택
> 선행 학습: 17번(샘플러와 필터링), 18번(블렌드 스테이트), 19번(래스터라이저 스테이트) — 상태 객체 기초 3종을 먼저 보면 이해가 빠르다

## 1. 이 장에서 배우는 것

`GraphicsEnums.h`에 모인 렌더 상태 8종이 화면에서 **실제로 어떻게 다르게 보이는지** 한 화면에서 비교합니다. 왼쪽(Before)은 sgf 엔진 디폴트로 고정하고, 오른쪽(After)은 방향키 Left/Right로 값을 순회하며 파이프라인에 즉시 반영합니다. 같은 지오메트리를 두고 **상태만 바꿔 끼워** 그려 시각 차이를 확인하는 것이 핵심입니다.

| # | 카테고리 | 값 수 | 엔진 디폴트(Before) | D3D11 대응 |
|---|---|---|---|---|
| 1 | BlendMode | 4 | `bmAlpha` | `D3D11_BLEND_DESC` |
| 2 | FilterMode | 3 | `fmLinear` | `D3D11_SAMPLER_DESC::Filter` |
| 3 | AddressMode | 4 | `amClamp` | `D3D11_SAMPLER_DESC::AddressU/V/W` |
| 4 | CullMode | 3 | `cmBack` | `D3D11_RASTERIZER_DESC::CullMode` |
| 5 | FillMode | 2 | `fmSolid` | `D3D11_RASTERIZER_DESC::FillMode` |
| 6 | DepthMode | 3 | `dmReadWrite` | `D3D11_DEPTH_STENCIL_DESC` |
| 7 | FrontFace | 2 | `ffClockwise` | `D3D11_RASTERIZER_DESC::FrontCounterClockwise` |
| 8 | PrimitiveTopology | 5 | `ptTriangleList` | `IASetPrimitiveTopology` |

상태는 **드로우 단위**입니다. 한 드로우 안의 모든 픽셀은 같은 상태를 공유하고, 다음 드로우 사이에 상태를 교체해 비교합니다.

## 2. 실행하면 보이는 것

화면 중앙 세로 경계선(토폴로지 제외)을 기준으로 좌=Before(디폴트 고정), 우=After(선택값)로 나뉩니다. 1~8 / Up/Down으로 카테고리를 바꾸고, Left/Right로 After 값을 돌립니다. 창 타이틀과 콘솔에 현재 카테고리와 After 이름이 실시간으로 표시됩니다.

| 키 | 동작 | 대상 |
|---|---|---|
| `1`~`8` / `Up`/`Down` | 카테고리 전환 (1 Blend, 2 Filter, 3 Address, 4 Cull, 5 Fill, 6 Depth, 7 FrontFace, 8 Topology) | 전체 |
| `Left`/`Right` | After 값 순회 (파이프라인 즉시 반영) | 선택 카테고리 |
| `ESC` | 종료 | - |

### 카테고리별 관찰 포인트

**1 BlendMode (4)** — 반투명 3원 겹침. 배경은 중간 회색이라 가산(밝아짐)과 곱셈(어두워짐)이 모두 보인다.
- `bmNone` : 덮어쓰기, 알파 무시 → 사각형 그대로
- `bmAlpha` [디폴트] : `SrcA / InvSrcA` 일반 반투명
- `bmAdd` : `SrcA / ONE` 가산 → 겹칠수록 흰색 (빛)
- `bmMultiply` : `ZERO / SrcColor` 곱셈 → 겹칠수록 검게 (그림자)

**2 FilterMode (3)** — 비스듬한 바닥(밉 있는 체커)으로 확대/축소 보간 비교.
- `fmPoint` : 가장 가까운 1픽셀 (도트, 계단)
- `fmLinear` [디폴트] : 4픽셀 블러 (가까운 곳 선명, 멀리 흐림)
- `fmAnisotropic` : 최대 16샘플 방향성 필터 — 정면은 Linear와 동일, **비스듬한 바닥의 먼 곳까지 선명**. 밉 체인이 있어야 차이가 난다.

**3 AddressMode (4)** — UV 0~3 타일링 체커를 한 장을 `s0(Before)/s1(After)` 두 샘플러로 나눠 읽어 한 번에 비교. 노란 경계선 좌=Before, 우=After.
- `amWrap` : 반복 타일링
- `amMirror` : 거울 뒤집어 반복
- `amClamp` [디폴트] : 가장자리 고정, 1~3 영역은 빨간 테두리가 늘어짐
- `amBorder` : 테두리 색(검정)으로 채움

**4 CullMode (3)** — 색큐브 두 개 나란히, 같은 회전.
- `cmNone` : 양면 모두
- `cmFront` : 앞면 버림 → 속이 보임
- `cmBack` [디폴트] : 뒷면 버림 → 닫힌 물체 기본

**5 FillMode (2)** — 같은 큐브, `Solid`(면) vs `Wireframe`(선만, 토폴로지 디버깅)

**6 DepthMode (3)** — 앞뒤 큐브 한 쌍의 겹침.
- `dmDisabled` : 깊이 OFF, 그리는 순서대로 덮임 (2D 적층)
- `dmReadWrite` [디폴트] : 테스트 ON + 기록 ON (3D 기본, Z가 앞선 것이 이김)
- `dmReadOnly` : 테스트 ON + 기록 OFF (반투명 3D)

**7 FrontFace (2)** — 같은 큐브, 시계=앞(`Clockwise`, D3D 기본) vs 반시계=앞(`CounterClockwise`, OpenGL 스타일). Cull이 `Back`일 때 어느 면이 사라지는지가 뒤바뀐다.

**8 Topology (5)** — 중앙 **단일** 표시 (Before/After 분할 없음). 별 모양 정점으로 점/선/삼각형 연결 차이를 확인.
- `ptPointList` : 점 5개
- `ptLineList` : 독립 선분
- `ptLineStrip` : 연결된 선
- `ptTriangleList` [디폴트] : 독립 삼각형 3개
- `ptTriangleStrip` : 띠

## 3. 핵심 개념 — 비유로 먼저 이해하기

### 3-1. 상태 객체 = "도장 세트"

파이프라인 상태는 도장처럼 미리 만들어 둔다. `BlendMode::bmAdd`면 도장에는 `SrcBlend=SRC_ALPHA, DestBlend=ONE`이 새겨져 있고, 찍는 순간(`SetBlendMode`) 파이프라인 전체가 그 규칙을 따른다. 필터/주소/컬링/깊이 모두 같은 원리다. `RenderStates`가 도장을 캐싱해 처음 요청 때 만들고 재사용하는 이유다.

### 3-2. 왜 Before/After를 좌우에 나눠 비교하나?

상태는 **드로우 단위**다. "왼쪽 픽셀은 Point, 오른쪽 픽셀은 Linear로 섞어"라고 한 드로우 안에서 지시할 수 없다. 그래서 같은 장면을 두 번 그린다. 블렌드/컬링/깊이/토폴로지는 `왼쪽 묶음 SetState(디폴트) → Draw → 오른쪽 묶음 SetState(선택) → Draw`로, 샘플러는 한 장을 두 슬롯(`s0`/`s1`)에 나눠 읽는 **분할 셰이더**로 한 번에 비교한다.

### 3-3. Filter 세 형제를 카메라로 비유하면

- **Point** : 모자이크 — 가장 가까운 색을 그대로 쓴다. 도트 게임.
- **Linear** : 부드러운 보정 — 주변 4픽셀을 섞는다. 가까운 곳은 선명하지만 멀리/비스듬하면 흐려진다. 밉맵이 있어도 등방성이라 방향을 구분 못 한다.
- **Anisotropic** : 망원경 — 비스듬한 방향으로 16개 샘플을 길게 모아 멀리까지 선명. 바닥이 멀리 도망가는 장면에서 차이가 난다. 정면에서 보면 Linear와 거의 같아 이 튜토리얼은 일부러 바닥을 눕혀 차이를 만든다.

밉맵이 없으면 Anisotropic도 Linear처럼 보인다. 이 튜토리얼은 256x256 체커에 밉 체인을 자동 생성한 뒤 비교한다.

### 3-4. Address는 "벽지를 창보다 크게 붙이면?"

UV 0~1은 벽지 한 장 크기다. UV 1~3까지 쓰면 벽지 밖이다. `Wrap`은 같은 벽지를 반복, `Mirror`는 뒤집어 반복, `Clamp`는 가장자리 색을 늘리고, `Border`는 지정한 테두리 색을 깐다. 이 튜토리얼 체커의 가장자리 1픽셀을 빨간 테두리로 만든 이유는 `Clamp`에서 빨강이 늘어나는 것을 한눈에 보기 위해서다.

## 4. 초보자 용어 사전

| 용어 | 뜻(쉬운 말) | 비유 |
|---|---|---|
| 열거형(enum) | 고를 수 있는 값 목록 | 자판기 메뉴 |
| 상태 객체 | 렌더 설정을 미리 만든 도장 | 도장 |
| 바인딩(Bind) | 도장을 파이프라인에 찍는 일 | 도장 찍기 |
| 샘플러 | 텍스처에서 색을 꺼내는 규칙 | 국자 |
| 필터 | 확대/축소 시 보간 규칙 | 돋보기 렌즈 |
| 주소 모드 | UV가 0~1 밖일 때 처리 | 벽지 붙이기 |
| 컬링 | 안 보이는 면 버리기 | 뒤집힌 종이 버리기 |
| 와인딩 | 정점이 감긴 순서(시계/반시계) | 화살표 방향 |
| 깊이 버퍼 | 화면 픽셀마다 가장 앞선 Z 기록 | 거리표 |
| 토폴로지 | 정점을 어떤 도형으로 이을지 | 점 연결 게임 |

## 5. 코드 워크스루

`GraphicsEnumDiff_Main()` 흐름을 따라간다.

### 5-1. 카테고리 정의와 디폴트 테이블

```cpp
enum class DiffCategory { dcBlendMode, dcFilterMode, dcAddressMode, dcCullMode, dcFillMode, dcDepthMode, dcFrontFace, dcTopology, Max };

constexpr _s32 DEFAULT_INDEX[] = { 1, 1, 2, 2, 0, 1, 0, 3 }; // 각 카테고리 Before 고정 인덱스
constexpr _s32 COUNT_PER_CATEGORY[] = { 4, 3, 4, 3, 2, 3, 2, 5 };
const wchar_t* BLEND_NAMES[] = { L"bmNone(덮어쓰기)", ... };
const BlendMode BLEND_VALUES[] = { BlendMode::bmNone, ... }; // 인덱스 → 실제 enum
```

Before는 `DEFAULT_INDEX`로 고정하고, After는 `afterIndices[카테고리]`에 기억해 카테고리를 옮겨도 이전 선택이 유지된다. 초기 After는 Before와 확실히 다르도록 `(def+1)%count`로 두어 처음부터 차이가 보인다.

### 5-2. 셰이더 3종

**색 변환**(`ColorTransformShaderSource34`) — 큐브/깊이/토폴로지용. WVP로만 변환하고 색은 그대로.

**블렌드 쿼드**(`BlendQuadShaderSource34`) — 흰 원 텍스처 × 정점 색. 원 밖(알파 0)은 흰색으로 섞어 곱셈에서 검은 사각형을 방지한다.

**텍스처 변환**(`TextureTransformShaderSource34`) — Filter 바닥용. WVP + 텍스처 샘플.

**분할 비교**(`SamplerSplitShaderSource34`) — Address용. `s0(Before)`/`s1(After)` 두 샘플러, `gSplitPixelX` 경계로 좌/우를 나눠 샘플링한다.

```hlsl
cbuffer CbSplit : register(b0) { float gSplitPixelX; float3 gPad; };
float4 PSMain(VSOutput i) : SV_TARGET {
    if (abs(i.position.x - gSplitPixelX) < 1.5f) return float4(1,0.85,0.2,1); // 노란 경계선
    if (i.position.x < gSplitPixelX) return gTexture.Sample(gSamplerBefore, i.uv) * i.color;
    return gTexture.Sample(gSamplerAfter, i.uv) * i.color;
}
```

### 5-3. 텍스처 2종

```cpp
FillSoftCirclePixels(s_SoftPixels, 128); // 블렌드용 부드러운 원
FillCheckerPixels(s_Checker, 32, 32, 4); // Address용 32x32 체커 (4px 셀, 빨간 테두리)
FillCheckerPixels(s_FilterChecker, 256, 256, 8); // Filter용 256x256 고주파 체커 + 밉 생성
```

Filter용은 256x256에 `MipLevels=0`+`GENERATE_MIPS`로 밉 체인을 만들어 `pFilterSRV`로 바인딩한다. Anisotropic 차이는 밉이 있어야 보인다.

### 5-4. 버퍼와 셰이더 생성

```cpp
vbQuad.Create(..., ResourceUsage::ruDynamic); // 블렌드용 매 프레임 갱신
vbUv.Create(..., uv 0~3);                     // Address용 전체 화면 쿼드
vbGround.Create(..., 2x2m 평면, uv 0~4);       // Filter용 바닥
vbCube.Create(... FillColorCube ...);         // 큐브
vbTopo.Create(... 별 모양 5점+중심 ...);
shaderBlend.CompileFromString(... BlendQuad ...);
shaderSplit.CompileFromString(... SamplerSplit ...);
shaderTextureTransform.CompileFromString(... TextureTransform ...);
shaderColor.CompileFromString(... ColorTransform ...);
```

### 5-5. 입력과 타이틀

```cpp
for (i 0..Max) if (IsKeyPressed('1'+i)) category = i;
if (IsKeyPressed(Up/Down)) category 순회;
if (IsKeyPressed(Left/Right)) afterIdx = (afterIdx ±1 + count)%count;
UpdateTitle(); PrintGuide(category, afterIdx);
```

### 5-6. 렌더 분기 — 카테고리별로 다른 비교법

**Blend** — `DrawCircleGroup(Before)` → `DrawCircleGroup(After)` 사이에 `SetBlendMode` 교체. 동적 쿼드 `vbQuad.Update`로 원 위치 갱신.

**Filter** — 기울인 바닥을 좌/우에 각각 `SetSampler(선택, Wrap, 0)`로 바인딩해 `viewFilter(0,1.4,-1.8 → 0,-0.8,3.5)`로 비스듬히 그리기.

```cpp
device.SetSampler(filter, AddressMode::amWrap, 0);
cb.wvp = Translation(offsetX,-0.8,0) * viewFilter * proj;
```

**Address** — 한 장을 `s0(Linear+Clamp)`/`s1(Linear+선택)` 두 샘플러로 나눠 `shaderSplit` 한 번에 그리기.

**Cull/Fill/FrontFace** — 같은 큐브를 좌/우에 `SetWireframe`/`SetCullMode` 또는 `GetRasterizerState(cull,fill,front)`로 나눠 그리기.

**Depth** — 앞뒤 큐브 한 쌍(`Z 0.5` 뒤, `Z 0.0` 앞)을 좌/우에 `GetDepthState(mode)`로 나눠 그리기.

**Topology** — 중앙 단일. 별 정점을 `SetPrimitiveTopology(선택)`으로만 바꿔 `Draw/DrawIndexed` 분기.

모든 분기 뒤 `EndFrame(true)`로 Present. 종료 시 상태를 디폴트로 복원해 다음 튜토리얼을 보호한다.

## 6. GPU와 DX11, 하드웨어 깊이 보기

### 6-1. 열거형 → D3D11 값 매핑 (GraphicsEnums.h 한 곳에만 둔다)

| 엔진 enum | D3D11 값 | 생성 위치 |
|---|---|---|
| `bmNone` | `BlendEnable FALSE, ONE/ZERO` | `FillBlendDesc` / `GetBlendState` |
| `bmAlpha` | `SRC_ALPHA / INV_SRC_ALPHA, OP_ADD` | 동일 |
| `bmAdd` | `SRC_ALPHA / ONE` | 동일 |
| `bmMultiply` | `ZERO / SRC_COLOR` (또는 `DEST_COLOR/ZERO`) | 동일 |
| `fmPoint` | `MIN_MAG_MIP_POINT` | `GetSamplerState` |
| `fmLinear` | `MIN_MAG_MIP_LINEAR` | 동일 |
| `fmAnisotropic` | `ANISOTROPIC, MaxAnisotropy 16` | 동일 |
| `amWrap/Mirror/Clamp/Border` | `TEXTURE_ADDRESS_WRAP / MIRROR / CLAMP / BORDER` | 동일 |
| `cmNone/Front/Back` | `CULL_NONE/FRONT/BACK` | `GetRasterizerState` |
| `fmSolid/Wireframe` | `FILL_SOLID/WIREFRAME` | 동일 |
| `ffClockwise` | `FrontCounterClockwise FALSE` | 동일 |
| `ffCounterClockwise` | `TRUE` | 동일 |
| `dmDisabled` | `DepthEnable FALSE, Write ZERO` | `GetDepthState` |
| `dmReadWrite` | `Enable TRUE, Write ALL, LESS_EQUAL` | 동일 |
| `dmReadOnly` | `Enable TRUE, Write ZERO` | 동일 |
| `ptPointList` 등 | `PRIMITIVE_TOPOLOGY_POINTLIST` 등 | `ToD3D11()` |

사용처는 `ToD3D11()` 인라인 함수로만 변환한다. `D3D11_` 상수를 직접 쓰지 않는다.

### 6-2. 하드웨어: 상태는 왜 드로우 단위인가?

블렌드는 ROP(OM), 샘플러는 텍스처 유닛, 래스터라이저는 클리퍼/컬러, 깊이는 Z 유닛, 토폴로지는 IA(정점 조립) 고정 기능 하드웨어의 설정이다. 하드웨어 레지스터는 파이프라인 전체에 적용되므로 픽셀마다 바꿀 수 없고, 드로우 사이에 레지스터를 다시 쓰는 형태로 교체한다. `RenderStates`가 상태 객체를 캐싱하는 이유가 여기에 있다.

### 6-3. Filter — 밉과 Anisotropic의 하드웨어 동작

텍스처가 멀리/비스듬하면 화면 픽셀 하나가 텍셀 여러 개를 덮는다. **밉맵**은 텍스처를 절반씩 축소한 피라미드로, 멀리서는 작은 밉을 샘플링해 에일리어싱을 줄인다. **Point**는 가장 가까운 밉의 가장 가까운 텍셀 1개, **Linear**는 밉 2개에서 각각 4텍셀을 섞은 뒤 밉 사이를 선형 보간, **Anisotropic**는 시야 방향으로 길게 찌그러진 픽셀 발자국에 맞춰 최대 16개 샘플을 방향성 있게 모아 멀리까지 선명도를 유지한다. 밉이 없거나 정면에서 보면 Linear≒Anisotropic이라 이 튜토리얼은 밉 있는 고주파 체커와 비스듬한 바닥으로 차이를 극대화한다.

### 6-4. Address — 텍스처 좌표가 밖일 때

`Clamp`는 가장자리 텍셀을 늘리고, `Border`는 `BorderColor`(현재 검정)를 쓴다. 그림자 맵처럼 범위 밖을 구분해야 할 때 `Border`가 유용하다. `Wrap/Mirror`는 타일링에 필수다.

### 6-5. Cull/FrontFace — 와인딩과 컬링

GPU는 화면에 투영된 삼각형의 정점 순서가 `FrontFace`와 일치하면 앞면, 아니면 뒷면으로 판정한다. 우리 엔진은 시계 방향이 앞면(D3D 기본)이라 인덱스 순서가 중요하다. `cmNone`은 종이처럼 얇은 물체에, `cmFront`는 큐브 속을 보는 특수 연출에 쓴다.

### 6-6. Depth — Z 버퍼의 읽기/쓰기 분리

`dmReadOnly`는 테스트는 하되 기록을 안 한다. 반투명 3D를 그릴 때 깊이에 가려지는지는 판정하되, 반투명 자신이 뒤의 반투명을 가리지 않게 하기 위해서다. 이 튜토리얼의 `dmDisabled`는 2D 적층처럼 그리는 순서가 결과를 결정함을 보여준다.

## 7. 핵심 규칙 요약

- [ ] 상태는 **드로우 단위**다. 픽셀마다 바꿀 수 없고 드로우 사이에 교체한다.
- [ ] Before는 엔진 디폴트 고정, After는 Left/Right로 순회해 파이프라인에 즉시 반영된다.
- [ ] Blend: `bmNone/bmAlpha/bmAdd/bmMultiply` — 공식과 용도(불투명/반투명/가산/곱셈)를 구분한다.
- [ ] Filter: `Point(1픽셀)/Linear(4픽셀)/Anisotropic(16샘플 방향성)` — 밉과 비스듬한 면에서 차이가 난다.
- [ ] Address: `Wrap/Mirror/Clamp/Border` — UV 밖 처리, 타일링과 가장자리/테두리.
- [ ] Cull/FrontFace/Fill: 와인딩이 앞면을 결정하고 컬링/채우기가 이를 이용한다.
- [ ] Depth: `Disabled(그리는 순서)/ReadWrite(3D 기본)/ReadOnly(반투명 3D)` — 읽기/쓰기 분리.
- [ ] Topology: 점/선/삼각형 연결 방식, `IASetPrimitiveTopology`로 정한다. 34번은 중앙 단일 표시.
- [ ] Anisotropic는 밉 체인이 있어야 의미가 있다. 정면에서는 Linear와 같다.

## 8. 직접 해보기 — 실험으로 확인하기

1. **Blend**: Additive로 원 20개를 겹쳐 하얗게 타는 것을 만들고, Multiply로 같은 원을 겹쳐 검게 어두워지는 것을 확인하세요.
2. **Filter**: Filter를 `Point`로 두고 같은 체커를 1번(원거리)과 8번(근거리) 위치에서 관찰해 계단 현상과 흐림을 비교하세요. `Anisotropic`로 바꿔 비스듬한 바닥의 먼 곳이 선명해지는 것을 확인하세요.
3. **Address**: Address를 `Border`로 두고 체커 밖(UV 1~3)이 검정으로 채워지는 것을 확인하세요. `Wrap`으로 타일링이 반복되는 것도 비교하세요.
4. **Cull**: `cmFront`로 큐브 속이 보이는 것을 확인하고, `FrontFace`를 `CounterClockwise`로 바꿔 컬링 결과가 뒤바뀌는 것을 확인하세요.
5. **Depth**: `dmDisabled`에서 앞뒤 큐브의 그리는 순서를 바꿔 결과가 달라지는 것을, `dmReadWrite`에서는 Z가 항상 이기는 것을 확인하세요.
6. **Topology**: `ptPointList`에서 정점 5개가 점으로만 찍히고, `ptLineStrip`에서 이어진 선이 되는 것을 확인하세요.

## 9. 다음 단계로

상태 8종을 한눈에 비교했으니 이제 각 상태를 **머티리얼**(20번)처럼 한 묶음으로 관리하는 방법을 떠올리면 된다. `BlendState/RasterizerState/DepthStencilState/SamplerState` 객체를 만들어 머티리얼에 담으면 `SetBlendMode` 같은 개별 호출 대신 한 번에 바인딩할 수 있다. `PipelineState`와 31번 파이프라인 여행으로 이어진다.
