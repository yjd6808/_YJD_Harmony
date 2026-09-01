# 00. sgf 엔진 전체 개요 — 처음 읽는 사람을 위한 안내서

> **이 문서 하나로**: sgf가 왜 태어났는지, 어떤 부품들로 이루어져 있는지, 코드를 읽을 때 어떤 규약을 알아야 하는지를 잡습니다.
> 나머지 문서(01~09)는 전부 이 문서의 각 챕터를 확대한 것입니다. 여기서 큰 그림을 잡고 넘어가세요.

---

## 1. sgf는 무엇인가요?

**sgf(SteinsGate Framework)** 는 DirectX 11 기반의 2D/3D 게임 엔진 정적 라이브러리입니다.

왜 만들었을까요? 기존 게임 클라이언트(sgcl)는 Cocos2d-x 위에서 돌아갑니다. Cocos2d-x는 훌륭하지만 거대하고, 내부를 우리가 완전히 통제할 수 없습니다. sgf의 최종 목표는 **sgcl에서 Cocos2d-x가 하던 역할을 우리 손으로 만든 엔진으로 교체하는 것**입니다. 그래서 sgf의 API 곳곳에는 Cocos2d-x 사용자가 이질감 없이 넘어올 수 있도록 닮은 구조가 일부러 심어져 있습니다. (`Director`, `Scene`, `AddChild`, `OnEnter`... 어디서 많이 본 이름이죠?)

예를 들어 씬 관리자인 Director의 주석은 대놓고 이렇게 말합니다.

```cpp
// Scene/Director.h
/*
 * 디렉터 (씬 관리자)
 *
 * [디렉터란?]
 * Cocos2d-x의 cocos2d::Director와 같은 역할.
 * "지금 어떤 씬을 보여줄지"를 관리하는 유일한 관리자다.
 * 씬 전환 요청이 오면 프레임 경계에서 안전하게 교체한다.
 * (프레임 도중 씬을 바꾸면 그리던 씬이 사라져 크래시 위험이 있다)
 */
```

프로젝트 관계는 이렇습니다.

| 프로젝트 | 역할 | 관계 |
|---|---|---|
| **jc** | 직접 만든 C++ 기반 라이브러리 (컨테이너/문자열/이벤트/싱글톤/시간/수학) | sgf가 전면 사용. `std::` 대신 jc를 씁니다 |
| **sgf** | 이 문서의 주인공. DX11 2D/3D 엔진 (정적 라이브러리) | jc 위에 서 있음 |
| **sgfr** | sgf를 만들기 위해 DX11을 단계별로 학습/실험한 튜토리얼 프로젝트 | sgf의 "연구 노트". `Docs/sgfr` 문서와 짝 |
| **sgcl** | Cocos2d-x 기반 게임 클라이언트 | 미래의 sgf 고객 |

---

## 2. 5분 컷 — 엔진으로 게임 하나 띄우기

전체 구조를 외우기 전에, 사용자 입장에서 sgf가 얼마나 단순하게 보이는지 먼저 느껴보세요. 필요한 것은 딱 세 가지입니다: **앱(Application) 파생 → 씬(Scene) 파생 → Director에게 씬 실행 요청**.

```cpp
// 1) 씬을 만든다 — 게임 화면 한 장
class MyScene : public sgf::Scene2D
{
	void OnEnter() override
	{
		GetCamera2D()->SetOrthographic2D(1280, 720);

		// 노란 사각형 하나 (정적 도형 노드 — 알아서 매 프레임 그려짐)
		Shape2D* pPanel = dbg_new Shape2D("패널");
		pPanel->SetRect(rect(100, 100, 200, 120), color(255, 200, 0));
		AddChild(pPanel, 0);
	}
};

// 2) 앱을 만든다 — 부팅이 끝나면 씬을 시작
class MyApp : public sgf::Application
{
	void ApplicationDidFinishLaunching() override
	{
		g_cDirector.RunScene(dbg_new MyScene());   // Cocos2d-x와 같은 감각
	}
};
```

창 생성, 디바이스 초기화, 메인 루프, 입력, 사운드, 리소스 — 나머지는 전부 엔진이 해줍니다. 이 "나머지 전부"가 어떻게 돌아가는지가 01~09 문서의 내용입니다.

---

## 3. 등장인물 소개 — 전역 객체들

sgf는 "게임 전체에 딱 하나면 되는 것"을 전역 싱글톤으로 제공하고, `g_c` 접두어 매크로로 접근합니다(sgcl 스타일). 코드 어디서든 아래 이름들이 보이면 이 표를 떠올리세요.

| 전역 이름 | 클래스 | 비유하자면 | 하는 일 |
|---|---|---|---|
| `g_cApp` | `Application` | **극장 지배인** | 부팅/종료, 메인 루프, 서브 윈도우 관리 |
| `g_cWindow` | `Window` | **무대(창)** | Win32 창 + 자신의 그리기 표면(스왑체인/깊이버퍼) 소유 |
| `g_cDevice` | `GraphicDevice` | **공장** | D3D11 리소스 생성/수명, 프레임 괄호(BeginFrame) |
| (디바이스 내부) | `GraphicContext` | **작업반장** | 바인딩/상태/드로우 명령 + 상태 캐시 |
| `g_cResourceMgr` | `ResourceMgr` | **도서관 사서** | 텍스처/메시/머티리얼/셰이더를 키(_u64)로 보관·대여 |
| `g_cRenderer2D` | `Renderer2D` | **2D 그림반** | 스프라이트/도형 배치 렌더링 (static/dynamic/mesh 3경로) |
| `g_cRenderer3D` | `Renderer3D` | **3D 그림반** | 메시 파이프라인 + 디버그 도형 배치 |
| `g_cDirector` | `Director` | **무대 감독** | 윈도우별 씬 슬롯, 씬 전환/Update/Render 지휘 |
| `g_cInput` | `InputManager` | **접수원** | 키보드/마우스 폴링 + 이벤트 |
| `g_cSound` | `SoundEngine` | **음향팀** | XAudio2 기반 재생 (`Play2d` — Cocos AudioEngine 대응) |

> **왜 전역 1개인가?** 창/디바이스/입력은 물리적으로 하나뿐인 자원입니다. 어디서나 접근해야 하는데 매번 포인터를 물려주면 코드만 번잡해집니다. 대신 "생성/소멸 시점은 Application이 통제한다"는 규칙으로 전역의 부작용을 억제합니다.

---

## 4. 폴더 지도 — 어디에 무엇이 있나

```
Projects/sgf/Sources/sgf/
├── Core/        생존에 필요한 뼈대
│   ├── Application.h/.cpp   부팅 → 메인 루프 → 종료 (총감독)
│   ├── Window.h/.cpp        Win32 창 + 메시지 펌프 + 표면 소유
│   └── FrameTimer.h/.cpp    시간 측정 (jc::TimeSpan, 250ms 클램프)
├── Input/
│   └── InputManager.h/.cpp  폴링 + 이벤트 입력
├── Graphics/    엔진의 심장 (가장 파일이 많음)
│   ├── GraphicDevice / GraphicContext   공장과 작업반장 (생성 vs 명령)
│   ├── SwapChain / DepthStencilSurface  창마다 하나씩 갖는 그리기 표면
│   ├── Vertex / VertexDeclaration       정점 구조와 "정점 포맷의 단일 진실"
│   ├── Buffers / ShaderProgram          VB/IB/상수버퍼, VS/PS 컴파일
│   ├── DefaultShaders.h                 코드에 내장된 기본 2D/3D 셰이더
│   ├── Texture / RenderTarget           이미지, 화면 밖 렌더링
│   ├── RenderStates / PipelineState     블렌드/래스터/깊이/샘플러 상태
│   ├── Mesh / Material                  "무엇을" / "어떻게" 그릴지
│   ├── ResourceMgr                      리소스 도서관 (+ 디폴트 리소스)
│   ├── Fill / PrimitiveBuilder          2D 채움(단색/텍스처/9패치/게이지)
│   ├── BatchRenderer                    2D/3D 렌더러의 공통 베이스
│   └── Renderer2D / Renderer3D          실제 그림을 그리는 두 일꾼
├── Scene/       게임 구조물
│   ├── Component / Transform            부품 시스템, SRT+dirty 전파
│   ├── GameObject / Layer               트리 노드 (zOrder, 수명 훅)
│   ├── Camera / Camera2D / Camera3D     뷰·투영 행렬 공급자
│   ├── Scene / Scene2D / Scene3D        화면 한 장 (root 트리 + 예약 창구)
│   ├── Shape2D / Shape3D                "도형 하나 그려줘" 편의 노드
│   └── Director                         씬 전환 관리자
└── Audio/
    └── SoundEngine.h/.cpp               XAudio2 (MAX_VOICES=32)
```

**읽는 요령**: 모든 헤더 파일 상단에는 `[역할]`, `[왜 이렇게 만들었나]` 식의 한국어 주석 블록이 있습니다. 클래스가 낯설면 무조건 헤더 주석부터 읽으세요. 이 문서 시리즈도 그 주석들을 발췌해가며 설명합니다.

---

## 5. 한 프레임의 여행 — 모든 문서를 관통하는 뼈대

sgf의 심장 박동은 `Application::Run()`의 while 루프입니다. 실제 코드가 곧 설계도이므로 그대로 발췌합니다. (전체 흐름은 01 문서에서 한 줄씩 해부합니다)

```cpp
// Core/Application.cpp — Run() 메인 루프 (요약 발췌)
while (true)
{
	// 1. 윈도우 메시지 처리. WM_QUIT이면 루프 종료.
	if (!window_.PumpMessage())
		break;

	// 2. 직전 프레임으로부터 경과 시간 측정 (jc::TimeSpan)
	timer_.Tick();
	const jc::TimeSpan& dt = timer_.Delta();

	// 3. 로직 갱신: 모든 윈도우의 씬 교체/갱신 -> 앱 훅
	g_cDirector.Update(dt);
	OnUpdate(dt);

	// 5. 메인 윈도우 그리기: 화면 지우기 -> 씬 그리기 -> 앱 훅 -> 화면 표시
	device_.BeginFrame(window_.GetSwapChain(), &window_.GetDepthSurface(), clearColor_);
	g_cDirector.Render(&window_);
	OnRender();
	window_.GetSwapChain().Present(vsync_);

	// 7. 입력 상태를 다음 프레임으로 넘긴다.
	input_.NextFrame();

	// 8. 사운드 엔진 정리 (재생이 끝난 소리의 재생기 회수)
	g_cSound.Update();
}
```

한 문장으로: **"메시지 받고 → 시간 재고 → 씬을 갱신하고 → 씬을 그리고 → 화면에 내보낸다"** 를 초당 수십~수백 번 반복하는 것이 게임입니다. 각 단계가 아래 문서들의 주제입니다.

```
PumpMessage ──────────────▶ 01 (부팅/루프), 08 (입력)
Tick(dt) ─────────────────▶ 01 (FrameTimer)
Director.Update ──────────▶ 04 (씬 시스템)
BeginFrame~Present ───────▶ 02 (2D), 03 (3D), 05 (리소스), 09 (정점 선언)
```

---

## 6. 코드 읽는 법 — sgf만의 규약

처음 코드를 열면 낯선 표기가 많습니다. 아래만 알면 대부분 해석됩니다.

### 6-1. 타입과 네이밍

| 표기 | 의미 |
|---|---|
| `_s32`, `_u32`, `_f32`, `_u64` | jc의 고정 크기 정수/실수 타입 (int, unsigned, float...) |
| `멤버변수_` (뒤 언더스코어) | 멤버 변수. 예: `width_`, `pDevice_` |
| `p` 접두어 | 포인터. 예: `pScene_`, `pTex` |
| `_인자` (앞 언더스코어) | 함수 매개변수. 예: `_width` |
| `dbg_new` | 메모리 누수 추적이 붙은 new |
| `JC_DELETE_SAFE(p)` | delete 후 nullptr 대입 |

### 6-2. enum은 접두어로 소속을 표시

`bmAlpha`(BlendMode), `cmBack`(CullMode), `fmSolid`(FillMode), `dmReadWrite`(DepthMode), `amClamp`(AddressMode), `rlDefault`(RenderLayer)... 처럼 **enum 이름의 약자를 소문자 접두어**로 붙입니다. 마지막 원소는 항상 `Max`(개수 세기용)입니다.

### 6-3. 수학 규약 — 행우선(row-major) + 왼손 좌표계

- 행렬은 **행우선**으로 저장하고, 벡터를 **행벡터**로 두고 `v * M` 순서로 곱합니다.
- 왼손 좌표계(LH), 깊이는 z ∈ [0, 1].
- 이 규약은 HLSL 셰이더까지 일관됩니다. 기본 셰이더 소스에서 직접 확인할 수 있습니다:

```cpp
// Graphics/DefaultShaders.h
/*
 * [행렬 규약]
 * 엔진은 행우선(row-major)이므로 HLSL도 row_major로 선언하고
 * mul(벡터, 행렬) 순서를 쓴다. (전치 없이 그대로 복사)
 */
cbuffer ConstantBufferFrame : register(b0)
{
	row_major float4x4 view_;
	row_major float4x4 projection_;
	float4 cameraPosition_;
};
```

### 6-4. 정점 구조체는 "자기 소개서"를 갖고 다닌다

정점 타입마다 자신의 메모리 배치를 설명하는 `Decl()`을 내장합니다. 이 선언이 정점 포맷의 유일한 진실이며(09 문서 주제), 하드코딩된 오프셋이 없습니다.

```cpp
// Graphics/Vertex.h
// 위치 + 색상 정점 (단순 도형, 선 그리기용)
struct VertexPC
{
	vec3 position_;		// 로컬 좌표계 위치
	color color_;		// 정점 색상 (삼각형 내부는 자동 보간됨)

	// 메모리 배치 선언 — 오프셋은 자동 누적 (하드코딩 제거)
	static const VertexDeclaration* Decl()
	{
		static const VertexElement ELEMENTS[] =
		{
			{ VertexSemantic::vsPosition, 0, VertexElementFormat::vefFloat3 },
			{ VertexSemantic::vsColor, 0, VertexElementFormat::vefColor32 },
		};
		static const VertexDeclaration* s_pDecl = VertexDeclaration::GetOrCreate(ELEMENTS);
		return s_pDecl;
	}
};
```

정점 종류는 세 가지만 기억하면 됩니다. **VertexPC**(위치+색 — 선/디버그), **VertexPTC**(위치+UV+색 — 2D 스프라이트), **VertexPNT**(위치+법선+UV — 3D 조명).

---

## 7. 문서 읽는 순서

| 순서 | 문서 | 한 줄 요약 |
|---|---|---|
| 1 | `01_부팅과_메인_루프` | 전원 버튼부터 심장 박동까지 |
| 2 | `02_2D_렌더링_파이프라인` | 2D를 그리는 세 가지 길 (static/dynamic/mesh) |
| 3 | `03_3D_렌더링_파이프라인` | 메시+머티리얼+월드행렬, b0/b1/b2 규약 |
| 4 | `04_씬_시스템과_카메라` | Scene 트리, GameObject, Transform, Director |
| 5 | `05_리소스_관리와_머티리얼` | 도서관(ResourceMgr)과 대여 키(_u64) |
| 6 | `06_예제_코드_모음` | 복붙해서 시작하는 실전 레시피 |
| 7 | `07_학습_로드맵` | sgfr 튜토리얼 35편과 엔진 소스의 연결 지도 |
| 8 | `08_입력_시스템` | 폴링 vs 이벤트, 프레임 경계의 비밀 |
| 9 | `09_정점_선언과_InputLayout_해시_캐시` | 정점 포맷 재설계 (2026-08-23) |

더 깊이 가고 싶다면:
- `Docs/sgf/RnD/EngineImprovement/` — 엔진을 상용 엔진급으로 키우기 위한 진단과 설계 (8편)
- `Docs/sgf/RnD/MultithreadRendering/` — 멀티스레드 렌더링 선행 연구 (6편)
- `Docs/sgfr/` — DX11을 0부터 배우는 튜토리얼 문서 (35편, sgfr 프로젝트와 짝)

---

## 8. 요약 — 기억할 다섯 문장

1. sgf는 **sgcl의 Cocos2d-x를 대체하기 위한 자체 DX11 엔진**이고, 그래서 Director/Scene/AddChild 같은 Cocos 스타일 API를 일부러 유지한다.
2. 사용자는 `Application` 파생 + `Scene` 파생 + `g_cDirector.RunScene()` 세 가지만 알면 게임을 시작할 수 있다.
3. 엔진 전역 객체는 `g_cApp`(지배인), `g_cDevice`(공장), `g_cResourceMgr`(사서), `g_cRenderer2D/3D`(그림반), `g_cDirector`(감독), `g_cInput`(접수원), `g_cSound`(음향팀)으로 역할이 분리되어 있다.
4. 심장은 `Application::Run()`의 **메시지 → 시간 → 갱신 → 그리기 → 표시** 루프다.
5. 코드 규약(행우선 `v*M`, LH 좌표, `g_c` 전역, enum 접두어, jc 타입)만 알면 어떤 파일을 열어도 읽힌다 — 그리고 모든 헤더 상단에 친절한 한국어 설명 주석이 있다.
