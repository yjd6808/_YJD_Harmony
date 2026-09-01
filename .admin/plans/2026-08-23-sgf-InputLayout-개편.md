# sgf InputLayout 구조 개편 — 수정 계획서

> 작성일: 2026-08-23 · 기준: sgf v5 (Projects/sgf 기준 실측)  
> 전제: **하위호환 불필요** — 레거시 경로 즉시 삭제, 전체 수정 허용  
> 목표: `셰이더 ↔ InputLayout 1:1` 제거 → **VertexDeclaration × VS 시그니처 N:M 캐시** (Unity/UE 방식)

---

## 1. 현 구조 진단

### 1.1 문제

| 증상 | 위치 | 원인 |
|---|---|---|
| 정점 포맷 3중 정의 | `Vertex.h` D3D11_INPUT_ELEMENT_DESC 배열 + `Buffers.h` stride + `GraphicDevice` LayoutSlot | 단일 진실 부재 |
| span 주소 키 캐시 | `GraphicDevice.cpp:365` `layout.Data() == span.Data()` | 함수-로컬 static 주소 의존 — 지역 배열이면 조용히 중복 생성 |
| 셰이더 1:1 소유 | `Mesh.h:122` `InputLayout inputLayout_`, `ShaderProgram.h` InputLayout 클래스 | 메시에 VS가 필요 — 같은 메시를 다른 VS로 못 그림 |
| 자동 결합 핸들 전용 | `GraphicContext.h:165` `currentShader_/currentVB` 핸들 추적 | typed 경로(`Material`, `Mesh`)는 캐시 미탑승 → 원시 Bind 오염 |
| 레거시 일체형 Shader | `Shader.h` VS+PS+Layout 한 덩어리, `Bind()`가 raw `IASet*` 직접 호출 | 컨텍스트 캐시 우회 → 튜토리얼 23 통계 오염 |

### 1.2 왜 VS 생성 시점은 유지하나

`CreateVertexShader`는 초기화 시 1회 수행 — 컴파일 비용이 크고 draw 필수이므로 지연할 이유 없음(Unity/UE 동일). 바뀌는 것은 **레이아웃 생성 시점**만: VS 생성 직후 → **최초 draw 직전 (선언×VS) 조합 첫 등장 시** 지연 생성 + 이후 캐시 재사용.

---

## 2. 개편 후 아키텍처

```
[Vertex.h]  VertexPC/PTC/PNT::Decl() → VertexDeclaration* (인터닝, 프로그램 수명)
     ↓
[Buffers.h] VertexBuffer가 Decl* 보관, Stride는 Decl에서 파생 (이중 정의 제거)
     ↓
[ShaderProgram.h] VertexShader가 D3DReflect로 InputSignature + sigHash 보관 (생성 시 1회)
     ↓
[GraphicDevice] GetOrCreateInputLayout(Decl*, VS*) — (declHash × sigHash) 캐시 단일 소유
     ↓
[GraphicContext] desired 상태(pDecl/pVs + dirty) → Draw 진입부 _ResolveInputLayout() 지연 결합
     ↓
[Mesh] 순수 형태 데이터 (VB/IB/Topology만 소유, VS/InputLayout 의존 제거)
[BatchRenderer/Renderer2D/3D] typed VS+PS로 전환, VB가 Decl 자동 전파
```

핵심 원칙:
- **포맷의 진실은 선언 하나** — COLOR가 4B `R8G8B8A8_UNORM`인 정보는 리플렉션으로 알 수 없으므로 Decl이 소유, 리플렉션은 시맨틱 존재 검증에만 사용.
- **레이아웃은 Device 리소스** — Context 상태가 아니므로 Device가 캐시 소유 (deferred 컨텍스트 여러 개여도 중복 없음).
- **해시는 내용 기반** — span 주소 비교 → `declHash × sigHash` 내용 비교로 교체. 지역 배열도 안전.

---

## 3. 수정 대상 전체 지도

| Phase | 파일 | 작업 | 비고 |
|---|---|---|---|
| **P1 신설** | `Graphics/VertexDeclaration.h/.cpp` | 신규 — VertexSemantic/Element/Declaration + 인터닝 + FNV-1a | 무위험 (추가만) |
| **P2 코어 타입** | `Graphics/Vertex.h` | `VertexLayoutSpan/Layout()` 삭제 → `Decl()` | P2~P6 한 커밋 권장 (연쇄) |
| **P3 셰이더** | `Graphics/ShaderProgram.h/.cpp` | VS에 리플렉션 추가, `InputLayout` 클래스 삭제 | |
| **P4 디바이스** | `Graphics/GraphicDevice.h/.cpp` | `LayoutSlot/layouts_/EnsureInputLayout` 삭제 → `InputLayoutEntry/inputLayoutCache_/GetOrCreateInputLayout` | |
| **P5 컨텍스트** | `Graphics/GraphicContext.h/.cpp` | 핸들 추적 삭제 → `pCurrentDecl_/pCurrentVs_/dirty + _ResolveInputLayout()` | 핵심 |
| **P6 버퍼/메시** | `Graphics/Buffers.h/.cpp`, `Graphics/Mesh.h/.cpp` | VB가 Decl 보관, Mesh에서 VS/InputLayout 제거 | |
| **P7 렌더러** | `Graphics/BatchRenderer.h/.cpp`, `Renderer2D/3D.h/.cpp`, `ResourceMgr.cpp` | 레거시 Shader → typed VS/PS, VB 생성 전환, 프리미티브 헬퍼 VS 인자 삭제 | |
| **P8 청소** | `Shader.h/.cpp` 삭제, `IResource.h`, `SgfHeader.h`, `sgf.vcxproj` | 레거시 잔재 0건 만들기 | 엔진 빌드 복구 지점 |
| **P9 튜토리얼** | `Projects/sgfr` ~20파일 | 호출부 7가지 규칙 일괄 치환 | 파일 단위 분리 가능 |

손대지 않는 것: `Material.h/.cpp`(이미 typed), `PipelineState.*`, `RenderStates.*`, `Texture/RenderTarget/SwapChain` 전체.

---

## 4. 파일별 상세

### P1. VertexDeclaration 신설

**파일**: `Graphics/VertexDeclaration.h` (신설), `Graphics/VertexDeclaration.cpp` (신설)

```cpp
enum class VertexSemantic : _u8 { vsPosition, vsNormal, vsTangent, vsColor, vsTexCoord, ... };
enum class VertexElementFormat : _u8 { vefFloat1..4, vefColor32, vefUByte4, Max };
struct VertexElement { VertexSemantic semantic_; _u8 semanticIndex_; VertexElementFormat format_; _u8 slot_; _u16 offset_ = VERTEX_OFFSET_APPEND; };
class VertexDeclaration {
  static const VertexDeclaration* GetOrCreate(const VertexElement* p, _s32 count);
  _u64 Hash() const; _u32 Stride(_u8 slot=0) const; bool Contains(const char*, _u32) const;
  // 내부: elements_, resolvedOffsets_[16], strides_[4], hash_
};
inline _u64 Fnv1a64(const void*, size_t);
const char* ToSemanticName(VertexSemantic);
_u32 VertexElementSize(VertexElementFormat);
_s32 BuildD3DElementDescs(const VertexDeclaration&, D3D11_INPUT_ELEMENT_DESC*, _s32);
```

구현 포인트:
- `GetOrCreate` 내부: 자동 누적 오프셋 해석 → stride 계산 → **필드 단위** FNV-1a 해시 (패딩 배제) → `jc::Vector<VertexDeclaration*>` 선형 탐색 (해시+내용 비교, std:: 금지 FR-26 준수) → 없으면 `dbg_new` 등록.
- `Contains`: `_stricmp(ToSemanticName(semantic), name)==0` — D3D 시맨틱 대소문자 무구분.
- `BuildD3DElementDescs`: 선언 → D3D11_INPUT_ELEMENT_DESC 배열 채우기 (Device 전용).
- `Mesh.h:44`의 `VertexFormat`과 충돌 방지 위해 새 enum은 `VertexElementFormat` 명명.

### P2. Vertex.h

```cpp
// Before
using VertexLayoutSpan = Span<const D3D11_INPUT_ELEMENT_DESC>;
struct VertexPC { static VertexLayoutSpan Layout() { static D3D11_INPUT_ELEMENT_DESC descs[] = {...}; return {descs,2}; } };

// After
struct VertexPC {
  vec3 position_; color color_;
  static const VertexDeclaration* Decl() {
    static const VertexElement ELEMENTS[] = {
      {VertexSemantic::vsPosition,0,VertexElementFormat::vefFloat3},
      {VertexSemantic::vsColor,0,VertexElementFormat::vefColor32},
    };
    static const VertexDeclaration* s = VertexDeclaration::GetOrCreate(ELEMENTS);
    return s;
  }
};
// VertexPTC(3요소), VertexPNT(3요소) 동일 패턴
// 오프셋 하드코딩(0/12/20/24) 완전 제거 — VERTEX_OFFSET_APPEND 자동 누적
```

### P3. ShaderProgram.h/.cpp

```cpp
// VertexShader에 추가
struct SignatureElement { char semanticName_[20]; _u32 semanticIndex_; };
jc::Vector<SignatureElement> inputSignature_; _u64 signatureHash_;
const auto& InputSignature() const; _u64 InputSignatureHash() const;
bool _BuildInputSignature(); // InitializeFromSource 성공 직후 1회 호출

// _BuildInputSignature 구현
// D3DReflect(pBytecode) → GetDesc → InputParameters 루프 → GetInputParameterDesc
// SystemValueType != D3D_NAME_UNDEFINED 필터 → SignatureElement 저장 → Fnv1a 해시
// InputLayout 클래스(82-102) 전체 삭제
```

### P4. GraphicDevice

```cpp
// 삭제: class InputLayout 전방선언, struct LayoutSlot, EnsureInputLayout, layouts_
// 추가:
struct InputLayoutEntry { _u64 declHash; _u64 sigHash; SgfComPtr<ID3D11InputLayout> pLayout; };
jc::Vector<InputLayoutEntry> inputLayoutCache_;
ID3D11InputLayout* GetOrCreateInputLayout(const VertexDeclaration* decl, VertexShader* vs);

// CreateVertexBuffer 템플릿 변경
template<typename TVertex>
_u32 CreateVertexBuffer(const TVertex* p, _u32 count, ResourceUsage usage=ruDefault) {
  return CreateVertexBufferInternal(p, count, usage, TVertex::Decl());
}
_u32 CreateVertexBufferInternal(const void* p, _u32 count, ResourceUsage usage, const VertexDeclaration* decl);

// GetOrCreateInputLayout 구현: declHash/sigHash 캐시 조회 → DEBUG에서 Contains 검증 → BuildD3DElementDescs → Device::CreateInputLayout → 캐시 등록
// Finalize: layouts_.Clear() → inputLayoutCache_.Clear() (ClearState 이후, SgfComPtr Release)
```

### P5. GraphicContext — 핵심

```cpp
// 삭제: SetInputLayout(InputLayout*), currentShader_/currentVertexBuffer_/ApplyPendingInputLayout()
// 추가:
const VertexDeclaration* pCurrentDecl_ = nullptr;
VertexShader* pCurrentVs_ = nullptr;
bool inputLayoutDirty_ = false;
void SetVertexDeclaration(const VertexDeclaration* decl);
bool _ResolveInputLayout();

// SetVertexBuffer(VertexBuffer*) — 선언 자동 전파
void SetVertexBuffer(VertexBuffer* b) {
  if(b) SetVertexDeclaration(b->Decl());
  // 기존 캐시 비교 + IASetVertexBuffers (stride는 b->Stride())
}

// SetVertexShader — desired 기록 + dirty
void SetVertexShader(VertexShader* s) {
  if(s->Raw()==pCachedVs_) { pCurrentVs_=s; skipped++; return; }
  pCachedVs_=s->Raw(); pCurrentVs_=s; inputLayoutDirty_=true; VSSetShader
}

// _ResolveInputLayout — Draw 직전
bool _ResolveInputLayout() {
  if(!dirty) return true;
  if(!pDevice_||!pCurrentVs_||!pCurrentDecl_) { assert(false); return false; }
  auto* raw = pDevice_->GetOrCreateInputLayout(pCurrentDecl_, pCurrentVs_);
  if(!raw) return false;
  if(raw!=pCachedInputLayout_) { IASetInputLayout(raw); apiCount++; } else skipped++;
  dirty=false; return true;
}
void Draw(...) { if(!_ResolveInputLayout()) return; Draw(...); }
void InvalidateCache() { /* 기존 캐시 무효화 */ inputLayoutDirty_=true; }
void ClearState() { ClearState(); pCurrentDecl_=nullptr; pCurrentVs_=nullptr; dirty=false; }
```

### P6. Buffers / Mesh

```cpp
// Buffers.h
bool Create(GraphicDevice* d, const void* data, UINT count, const VertexDeclaration* decl, ResourceUsage usage=ruDefault);
const VertexDeclaration* Decl() const { return pDecl_; }
// 삭제: 레이아웃 없는 Create 오버로드, VertexLayoutSpan layout_ → const VertexDeclaration* pDecl_
// Create 내부: stride_ = decl->Stride(0); pDecl_=decl;

// Mesh.h
bool Initialize(GraphicDevice* d, const void* verts, UINT vCount, const VertexDeclaration* decl, const _u32* idx=nullptr, UINT iCount=0, PrimitiveTopology topo=ptTriangleList);
bool InitializeAsRect2D(GraphicDevice* d); // VS 인자 전부 삭제 (2D 4개 + 3D 6개 팩토리)
bool InitializeAsCube(GraphicDevice* d); // ...
// 삭제: #include ShaderProgram.h, InputLayout inputLayout_
// Bind: SetVertexBuffer만 (선언 자동 전파), SetInputLayout 삭제
// 팩토리 내부: Initialize(..., VertexPNT::Decl(), indices...) 형태로 전환
```

### P7. BatchRenderer / Renderer2D/3D / ResourceMgr

```cpp
// BatchRenderer.h: #include Shader.h → ShaderProgram.h
virtual const VertexDeclaration* VertexDecl() const = 0; // VertexLayoutSpan 대체
VertexShader vs_; PixelShader ps_; // Shader shader_ 대체
// BatchRenderer.cpp: vs_.InitializeFromSource + ps_.InitializeFromSource / ApplyFrameStates에서 SetVertexShader(&vs_)+SetPixelShader(&ps_)

// Renderer2D.h/.cpp, Renderer3D.h/.cpp
const VertexDeclaration* VertexDecl() const override { return VertexPTC::Decl(); } // or VertexPC
vertexBuffer_.Create(device, nullptr, MAX_VERTICES, VertexPTC::Decl(), ruDynamic) // sizeof 제거

// ResourceMgr.cpp: CreatePrimitiveMesh2D/3D 헬퍼에서 VertexShader* 인자 삭제, 호출부 pVs2D/pVs3D 인자 삭제
```

### P8. 삭제/정리

- 삭제: `Graphics/Shader.h`, `Graphics/Shader.cpp`
- `IResource.h`: `ResourceType::rtInputLayout` + `RESOURCE_TYPE_NAMES` "InputLayout" 삭제 (enum 시프트 안전 — 직렬화 없음)
- `SgfHeader.h`: `#include Shader.h` 삭제, `#include VertexDeclaration.h` 추가 (Vertex.h보다 앞)
- `sgf.vcxproj`: `VertexDeclaration.h/.cpp` 추가, `Shader.h/.cpp` 제거
- 검증: `grep -rn "VertexLayoutSpan|::Layout()|CompileFromString|SetInputLayout|EnsureInputLayout|LayoutSlot|rtInputLayout" Projects/sgf` → 0건

### P9. 튜토리얼 일괄 전환 (sgfr)

| # | Before | After |
|---|---|---|
| ① | `VertexLayoutSpan descs = VertexPC::Layout();` | **삭제** |
| ② | `Shader shader;` | `VertexShader vs; PixelShader ps;` |
| ③ | `shader.CompileFromString(&device, src, descs)` | `vs.InitializeFromSource(&device, src) && ps.InitializeFromSource(&device, src)` |
| ④ | `shader.Bind(gc);` | `gc.SetVertexShader(&vs); gc.SetPixelShader(&ps);` |
| ⑤ | `vb.Create(&device, verts, sizeof(VertexPC), 3)` | `vb.Create(&device, verts, 3, VertexPC::Decl())` |
| ⑥ | `mesh.Initialize(&device, verts, sizeof(VertexPTC), 6, descs, &vs)` | `mesh.Initialize(&device, verts, 6, VertexPTC::Decl())` |
| ⑦ | `InitializeAsCube(&device, &vs)` | `InitializeAsCube(&device)` |

대상: 05/05_Simple/06/07/10/11/13/14/15/16/17/18/19/24/25/34(3쌍) + 08/09/12/20 등 typed 경로 + 26/27/28/30/Temp  
05 핸들 경로(`hShader/hVb` + `SetShader/SetVertexBuffer`)는 무수정 — 내부적으로 typed 위임되므로 새 resolve 자동 탑승.

---

## 5. 검증 체크리스트

**정적**
- [ ] `VertexLayoutSpan`/`::Layout()`/`CompileFromString`/`SetInputLayout`/`EnsureInputLayout`/`LayoutSlot`/`rtInputLayout` 0건
- [ ] `Shader.h` include 0건, `class Shader` 정의 0건
- [ ] `D3D11_INPUT_ELEMENT_DESC`는 `VertexDeclaration.cpp`에만 존재

**동적**
- [ ] 05(핸들+레거시), 07(정점/인덱스), 08(메시), 20(Material), 23(캐시 통계) 정상 렌더
- [ ] 23번 `SkippedCallCount` 개편 전 대비 증가/동일 (raw Bind 제거 효과)
- [ ] RenderDoc 캡처: InputLayout 객체 수 == (Decl × Sig 조합 수) (기존: 메시 수)
- [ ] `_DEBUG`에서 `VertexPC 메시 + PTC용 VS` 조합으로 draw → 시맨틱 이름 포함 assert 발생

---

## 6. 커밋 전략 (레거시 호환 없이 한 번에)

| 커밋 | 내용 | 빌드 |
|---|---|---|
| 1 | P1+P2 (VertexDeclaration 신설 + Vertex.h Decl 도입) | — (연쇄) |
| 2 | P3+P4 (VS 리플렉션 + Device 캐시) | — |
| 3 | P5+P6+P7 (Context resolve + Buffers/Mesh/Renderer/ResourceMgr) + P8 청소 | ✅ 엔진 빌드 복구 |
| 4 | P9 튜토리얼 일괄 전환 + 검증 | ✅ 전체 빌드 |

중간 커밋 1·2는 튜토리얼 깨진 상태 — 브랜치에서 1→4 연속 작업 후 합치기 권장.  
하위호환 불필요이므로 Layout() 공존 기간 없이 즉시 교체한다.

---

## 7. 참고 — 이번 범위 밖 (후속 이슈)

- `RenderTarget.cpp:206` raw `PSSetShaderResources` 캐시 우회
- `INVALID_HANDLE` 이중 정의 (`GraphicDevice.h` NS 안 / `GraphicContext.h` NS 밖)

---

*원문: `sgf_InputLayout_개편_수정순서.md` (2026-08-23, v5 실측 1246줄) — 본 문서는 하위호환 제거 전제에서 전체 수정 허용 방향으로 재구성한 계획서.*
