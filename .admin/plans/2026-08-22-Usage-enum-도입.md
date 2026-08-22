# Usage enum 도입 및 GraphicsEnums D3D11 매핑 주석 보강

- 작성일: 2026-08-22
- 작업 대상: `GraphicsEnums.h` + `Buffers.h/cpp` + `Texture/RenderTarget/DepthStencilSurface`

## 목적
- `D3D11_USAGE`를 엔진 자체 `ResourceUsage` enum으로 래핑하여 `D3D11_*` 직접 사용을 `GraphicsEnums.h` 한 곳으로 모은다
- 기존 `GraphicsEnums.h`의 모든 상수 우측에 D3D11 매핑값을 주석으로 상세히 기술하여 초심자도 대응 관계를 즉시 파악 가능하게 한다

## 분석
- 현재 `D3D11_USAGE` 직접 사용처 7곳 (sgf만, third party 제외):
  - `Buffers.cpp:45,116` `_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT`
  - `Buffers.h:118` `D3D11_USAGE_DYNAMIC` (ConstantBuffer)
  - `Texture.cpp:181` `D3D11_USAGE_IMMUTABLE`
  - `RenderTarget.cpp:61,127` `D3D11_USAGE_DEFAULT`
  - `DepthStencilSurface.cpp:29` `D3D11_USAGE_DEFAULT`
- `Buffers`의 `bool _dynamic`은 2값만 표현, `IMMUTABLE/STAGING` 표현 불가. `true` 사용처는 `Renderer2D.cpp:128,134` 2곳뿐
- 기존 enum별 D3D 매핑:
  - `BlendMode` → `FillBlendDesc` / `RenderStates::GetBlendState` 내 `D3D11_BLEND_*` 조합
  - `DepthMode` → `FillDepthStencilDesc` 내 `D3D11_DEPTH_WRITE_MASK_*`
  - `CullMode/FillMode/FrontFace` → `FillRasterizerDesc`
  - `FilterMode/AddressMode` → `FillSamplerDesc` + `ToD3D11`
  - `PixelFormat` → `DXGI_FORMAT_*` (ToD3D11)
  - `PrimitiveTopology` → `D3D11_PRIMITIVE_TOPOLOGY_*` (ToD3D11)

## 설계
### 1. 신규 enum `ResourceUsage` (`GraphicsEnums.h`)
```cpp
enum class ResourceUsage
{
    ruDefault,    // D3D11_USAGE_DEFAULT
    ruImmutable,  // D3D11_USAGE_IMMUTABLE
    ruDynamic,    // D3D11_USAGE_DYNAMIC
    ruStaging,    // D3D11_USAGE_STAGING
    Max,
};
inline D3D11_USAGE ToD3D11(ResourceUsage _usage) { ... }
```
- 접두어 `ru` (ResourceUsage), 기존 `bm/cm/fm` 규칙과 동일
- 매핑은 `GraphicsEnums.h`에 한 곳에만 둔다는 기존 규칙(127행 주석) 준수

### 2. `Buffers.h/cpp` API 교체
- `VertexBuffer::Create(..., bool _dynamic=false)` → `Create(..., ResourceUsage _usage = ResourceUsage::ruDefault)`
- `IndexBuffer::Create` 동일
- 내부 `bool dynamic_` → `ResourceUsage usage_` 로 교체, `Update`는 `usage_ != ruDynamic` 시 실패
- `ConstantBuffer::Create`는 `ToD3D11(ruDynamic)` 사용
- 기존 `bool` 오버로드는 제거 (사용처 2곳만 있으므로 직접 교체, 호환 오버로드 유지 불필요)

### 3. 나머지 D3D11_USAGE 교체
- `Texture.cpp:181` → `ToD3D11(ResourceUsage::ruImmutable)`
- `RenderTarget.cpp:61,127` → `ToD3D11(ruDefault)`
- `DepthStencilSurface.cpp:29` → `ToD3D11(ruDefault)`

### 4. 호출부 반영
- `Renderer2D.cpp:128,134` `true` → `ResourceUsage::ruDynamic`
- `Renderer3D.cpp:94,98` `true` → `ResourceUsage::ruDynamic` (이미 `true` 아님? 확인 필요 - 실제로는 `Create(..., true)` 맞음)
- `Mesh.cpp:45,53` 기본값 유지 (`ruDefault`)

### 5. 기존 상수 D3D11 매핑 주석 보강 (`GraphicsEnums.h`)
- 각 멤버 우측에 `// D3D11_*값 → 설명` 형태 주석 추가
- 예: `bmAlpha, // D3D11_BLEND_SRC_ALPHA / INV_SRC_ALPHA, BlendOp ADD`
- `BlendMode/DepthMode`는 `Fill*Desc` 구현과 동일 서술로 맞춘다
- `CullMode/FillMode/FilterMode/AddressMode/PixelFormat/PrimitiveTopology`는 이미 `ToD3D11` 함수가 있으므로 주석에 대응 D3D 값 명시
- `ShaderStage/StaticLevel`은 D3D 직접 매핑 없음 → `// D3D 직접 매핑 없음, VSSet*/PSSet* 분기용` 등 엔진 용도 설명

## 검증
- `Scripts\BuildProject\sgfr.bat -C Debug -P x64` 빌드 성공 확인
- `D3D11_USAGE` 문자열이 `GraphicsEnums.h` `ToD3D11` 내부 1곳 외에는 남아있지 않음을 grep으로 확인
- `ResourceUsage::Max` 순회 가능 확인

## 위험
- `Buffers::Create` 시그니처 변경 — 호출부 4곳만 있어 영향 적음
- `ConstantBuffer`는 템플릿 헤더이므로 `GraphicsEnums.h`를 포함해야 함 (`Buffers.h`가 이미 `GraphicDevice.h` 경유로 포함하나, 명시적 include 추가 고려)

## 파일 목록
- `GraphicsEnums.h` — 신규 enum + ToD3D11 + 주석 보강
- `Buffers.h` — Create 시그니처 + 멤버 타입 변경
- `Buffers.cpp` — Usage 로직 교체
- `Texture.cpp` — IMMUTABLE 교체
- `RenderTarget.cpp` — DEFAULT 2곳 교체
- `DepthStencilSurface.cpp` — DEFAULT 교체
- `Renderer2D.cpp` / `Renderer3D.cpp` / `Mesh.cpp` — 호출부 반영 (필요 시)
