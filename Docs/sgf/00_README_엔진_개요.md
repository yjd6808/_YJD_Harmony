# 00. sgf 엔진 개요 — DX11 2D/3D 게임 엔진의 지도

> 작성자: 윤정도 · 이 문서는 sgf(SteinsGate Framework) 엔진 자체를 설명하는 문서입니다.
> sgfr 튜토리얼(01~31)이 DX11 개념 학습이라면, 이 문서 시리즈는 **그 개념을 포장한 엔진의 구조**입니다.
> 튜토리얼을 마친 뒤, 또는 튜토리얼과 병행하면서 읽으면 좋습니다.

---

## 1. sgf는 무엇인가요?

sgf는 jc 라이브러리를 기반으로 만든 **DX11 2D/3D 게임 엔진(정적 라이브러리)**입니다.

| 항목 | 내용 |
|---|---|
| 위치 | `Projects/sgf/Sources/sgf/` |
| 그래픽 API | Direct3D 11 (DX11) |
| 언어 | C++ (jc 컨벤션, jc 컨테이너 사용) |
| 정적 라이브러리 | 그래서 다른 프로젝트(sgfr, sgcl)가 링크해서 사용 |
| 최종 목표 | sgcl이 쓰는 Cocos2d-x 엔진을 sgf로 교체 |

sgf의 핵심 아이디어는 딱 하나입니다.

> **날것의 DX11(디바이스, 컨텍스트, 버퍼, 셰이더...)을 게임 개발자가 편하게 다루는 개념(씬, 게임 오브젝트, 카메라, 렌더러)으로 포장한다.**

---

## 2. 폴더 구조

```
Projects/sgf/Sources/sgf/
├── PCH/Core.h              ← 미리 컴파일된 헤더 (d3d11, DirectXMath, XAudio2 등)
├── Namespace.h             ← NS_SGF_BEGIN / NS_SGF_END / USING_NS_SGF 매크로
├── SgfHeader.h             ← ★ 통합 헤더. 이거 하나만 include하면 sgf 전부 사용 가능
├── Core/
│   ├── Window.h/.cpp       ← OS 윈도우 + 스왑체인 + 깊이버퍼 소유
│   ├── FrameTimer.h/.cpp   ← 프레임 타이머 (QPC, jc::TimeSpan)
│   └── Application.h/.cpp  ← 게임 앱 진입점 + 메인 루프 (g_cApp)
├── Input/
│   └── InputManager.h/.cpp ← 키보드/마우스 폴링 + 이벤트 (g_cInput)
├── Graphics/
│   ├── GraphicDevice.*     ← DX11 디바이스/컨텍스트 래퍼 (g_cDevice)
│   ├── GraphicContext.*    ← 상태 바인딩 캐시 (중복 D3D 호출 생략)
│   ├── SwapChain.*         ← 화면 교체 (더블 버퍼링)
│   ├── DepthStencilSurface.* ← 깊이/스텐실 버퍼
│   ├── GraphicsEnums.h     ← 열거형 모음 (BlendMode, CullMode, ...)
│   ├── Vertex.h            ← VertexPC / VertexPTC / VertexPNT
│   ├── Buffers.*           ← VertexBuffer / IndexBuffer / ConstantBuffer
│   ├── Shader.*            ← Shader 추상 + VertexShader/PixelShader
│   ├── ShaderProgram.*     ← VS+PS 쌍 묶음
│   ├── DefaultShaders.h    ← 내장 셰이더 소스 (2D/3D)
│   ├── Texture.*           ← 텍스처 (PNG/SVG 로드)
│   ├── RenderTarget.*      ← 화면 밖 렌더 타깃
│   ├── RenderStates.*      ← 상태 객체 플라이웨이트 캐시
│   ├── PipelineState.*     ← 독립 상태 객체 래퍼 (BlendState 등)
│   ├── Mesh.*              ← 정점+인덱스+레이아웃 묶음 + 프리미티브 생성
│   ├── Material.*          ← 셰이더+상태+텍스처 묶음 (그리는 법)
│   ├── ResourceMgr.*       ← 리소스 도서관 (g_cResourceMgr)
│   ├── PrimitiveMeshType.h ← 프리미티브 메시 종류 열거형
│   ├── Fill.*              ← 2D 채움 셰이프 시스템 (게이지/9-패치 등)
│   ├── BatchRenderer.*     ← Renderer2D/3D 공용 배치 베이스 (Begin/End/Flush)
│   ├── PrimitiveBuilder.*  ← 2D 기하 공용 빌더 (채움/메시 팩토리 공유)
│   ├── Renderer2D.*        ← 2D 배치 렌더러 (g_cRenderer2D)
│   ├── Renderer3D.*        ← 3D 메시 렌더러 (g_cRenderer3D)
│   └── IResource.h         ← 리소스 공통 인터페이스/키
├── Scene/
│   ├── Component.h         ← 컴포넌트 인터페이스 (Transform/Material 기본)
│   ├── Transform.*         ← 트랜스폼 (로컬/월드 행렬 캐시)
│   ├── GameObject.*        ← ★ 씬 트리 노드 (GID, zOrder, 훅)
│   ├── Layer.h             ← 렌더 레이어 열거형 (Bottom/Default/Top)
│   ├── Camera.h            ← 카메라 추상 (GameObject 파생)
│   ├── Camera2D.*          ← 직교 카메라 (2D)
│   ├── Camera3D.*          ← 원근 카메라 (3D)
│   ├── Scene.h             ← 씬 추상 베이스 (root_ 트리, 카메라, 예약 창구)
│   ├── Scene2D.*           ← 2D 씬 (스태틱 선언 + 렌더)
│   ├── Scene3D.*           ← 3D 씬 (DrawMesh 창구)
│   ├── Shape2D.*           ← 2D 프리미티브 자동 드로우 편의 노드 (스태틱 bake)
│   ├── Shape3D.*           ← 3D 프리미티브 자동 드로우 편의 노드 (메시+머티리얼 자동 구성)
│   └── Director.*          ← 씬 디렉터 (g_cDirector, 윈도우별 씬 슬롯)
└── Audio/
    └── SoundEngine.*       ← XAudio2 사운드 (g_cSound)
```

---

## 3. 전역 객체 (g_c 매크로 — 싱글톤)

| 매크로 | 클래스 | 역할 |
|---|---|---|
| `g_cApp` | Application | 게임 앱 (Initialize/Run) |
| `g_cWindow` | Window | 메인 윈도우 |
| `g_cDevice` | GraphicDevice | DX11 디바이스 |
| `g_cInput` | InputManager | 키보드/마우스 입력 |
| `g_cRenderer2D` | Renderer2D | 2D 배치 렌더러 |
| `g_cRenderer3D` | Renderer3D | 3D 메시 렌더러 |
| `g_cResourceMgr` | ResourceMgr | 리소스 매니저 |
| `g_cDirector` | Director | 씬 디렉터 |
| `g_cSound` | SoundEngine | 사운드 엔진 |

대부분 `SingletonStatic` 기반이라 **초기화/정리 순서가 중요**합니다. (`Application::Initialize`가 전부 해줍니다)

---

## 4. 클래스 다이어그램 (의존 방향)

```
                        [Application] ── 사용 ──▶ [Director] ──▶ [Scene]
                             │                         │            │
                             │ 초기화 순서              │ Render()    │ 소유
                             ▼                         ▼            ▼
   [Window] ──▶ [SwapChain]           [Scene2D] [Scene3D]      [GameObject(root_)]
        │                              └── 사용 ──▶ [Renderer2D/3D]
        ▼                                         │
   [DepthStencilSurface]                          ▼
                                              [GraphicContext] ←── 상태 캐시
        ┌───────────────────────────────────────┘ │
        ▼                                         ▼
   [GraphicDevice] ──▶ [Mesh] ──▶ [VertexBuffer/IndexBuffer/InputLayout]
        │                  │
        │                  └──▶ [Material] ──▶ [ShaderProgram, Texture, 상태객체]
        ▼
   [Renderer2D] ──▶ [Fill] ──▶ [PushQuad/Triangles] ──▶ 배치 버퍼 ──▶ Flush
   [Renderer3D] ──▶ [Mesh+Material] ──▶ [Draw] ──▶ 배치 ──▶ Flush
```

주요 관계:

| 관계 | 설명 |
|---|---|
| Scene ⊃ GameObject | 씬이 `root_` GameObject를 소유하고, 게임 오브젝트가 트리로 붙는다 |
| GameObject ⊃ Camera | 카메라도 GameObject 파생 (씬 트리에 존재하며 `View()`/`Projection()` 제공) |
| Scene → Renderer | 씬의 예약 창구(DeclareStatic/DrawMesh)가 렌더러로 전달된다 |
| GraphicContext → D3D | 모든 상태/버퍼 바인딩이 캐시를 통과한다 (중복 D3D 호출 생략) |
| ResourceMgr ⊃ Mesh/Material/Texture | 리소스는 키(`_u64`)로 참조한다 (소유권 = 매니저) |

---

## 5. 네이밍/코딩 규약 (jc 컨벤션)

| 구분 | 규칙 | 예 |
|---|---|---|
| 기본 타입 | `_s32` `_u32` `_f32` `_u64` `_s8` 등 | `_u32 _vertexCount` |
| 파라미터 | `_camelCase` (밑줄 접두사) | `_pDevice`, `_staticId` |
| 멤버 | `m_PascalCase` 또는 `name_` (구조체 공개 멤버) | `m_pContext`, `camera_` |
| 포인터 | `p` 접두사 | `_pChild`, `m_pDevice` |
| 출력 파라미터 | `_pOut` | `FillResult& _out` |
| 열거형 값 | 접두어 + Pascal | `bmAlpha`, `cmBack`, `fmSolid`, `dmReadWrite` |
| 열거형 종단 | 반드시 `Max`로 끝남 | `BlendMode::Max` |
| 싱글톤 | `SingletonStatic` + `g_c` 매크로 | `g_cResourceMgr` |
| 매크로 | `JC_DELETE_SAFE` `JC_RELEASE_SAFE` `jc_assert` 등 | — |

열거형 접두어 규칙 (`GraphicsEnums.h`):

| 열거형 | 접두어 | 예 |
|---|---|---|
| BlendMode | `bm` | `bmNone`, `bmAlpha`, `bmAdd`, `bmMultiply` |
| CullMode | `cm` | `cmNone`, `cmFront`, `cmBack` |
| FillMode | `fm` | `fmSolid`, `fmWireframe` |
| FrontFace | `ff` | `ffClockwise`, `ffCounterClockwise` |
| DepthMode | `dm` | `dmDisabled`, `dmReadWrite`, `dmReadOnly` |
| FilterMode | `fm` | `fmPoint`, `fmLinear`, `fmAnisotropic` |
| AddressMode | `am` | `amClamp`, `amWrap`, `amMirror` |
| RenderLayer | `rl` | `rlBottom`, `rlDefault`, `rlTop` |
| PrimitiveTopology | `pt` | `ptTriangleList` 등 |

---

## 6. 수학/셰이더 규약 (전체 공통 — 헷갈리면 여기로)

| 항목 | 규칙 |
|---|---|
| 행렬 저장 | row-major (행 우선) |
| 벡터 곱 | 행벡터 `v' = v * M` |
| 계층 변환 | 자식 월드 = 자식 로컬 × 부모 월드 |
| 전체 파이프라인 | `world * view * projection` |
| HLSL | `row_major float4x4`, `mul(v, M)` — 전치 불필요 |
| 좌표계 | 왼손(LH), +Z가 화면 안쪽 |
| NDC 깊이 | z ∈ [0, 1] |
| 각도 | `jc_math_deg2rad()` 변환 후 사용 |

상수버퍼 슬롯 규약 (`DefaultShaders.h`):

| 슬롯 | 이름 | 내용 | 갱신 빈도 |
|---|---|---|---|
| b0 | FrameConstants | `view_`, `projection_`, `cameraPosition_` | 프레임당 1회 |
| b1 | ObjectConstants | `world_` | 오브젝트당 1회 |
| b2 | MaterialConstants | `baseColor` 등 | 머티리얼 변경 시 |

---

## 7. 문서 시리즈 목록

| 문서 | 내용 |
|---|---|
| `01_부팅과_메인_루프.md` | Application::Initialize → Run, 한 프레임의 흐름 |
| `02_2D_렌더링_파이프라인.md` | Renderer2D: 스태틱/다이나믹, Fill, 배칭, Flush |
| `03_3D_렌더링_파이프라인.md` | Renderer3D: 메시, 프리미티브, 프레임 상수 |
| `04_씬_시스템과_카메라.md` | Scene/GameObject/Component/Transform/Director/Camera |
| `05_리소스_관리와_머티리얼.md` | ResourceMgr, Mesh, Material, 상태 객체, 렌더타깃 |
| `06_예제_코드_모음.md` | 최소 2D 씬, 3D 큐브, 씬 전환 등 실전 예제 |
| `07_학습_로드맵.md` | 어떤 순서로 코드를 읽을지 (sgfr 튜토리얼 매핑) |
| `08_입력_시스템.md` | InputManager: 폴링/이벤트, NextFrame 원리 |
