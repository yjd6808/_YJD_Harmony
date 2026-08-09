# 25. 엔진 v2 아키텍처 - 최종버전 게임엔진의 구조

> 이 문서는 sgf 엔진이 v1(튜토리얼용 뼈대)에서 v2(최종버전 게임엔진)로 어떻게, 왜 바뀌었는지 설명합니다.
> 코드를 읽기 전에 이 문서를 먼저 읽으면 전체 그림이 잡힙니다.

---

## 1. v2에서 바뀐 것 한눈에 보기

| 항목 | v1 (기존) | v2 (최종) | 이유 |
|---|---|---|---|
| 카메라 | 행렬을 직접 만들어야 함 | `SetOrthographic2D` / `DriveDefault2D` 등 고수준 API | 일반 프로그래머도 한 줄로 사용 |
| D3D 개체 | `GraphicDevice` 한 클래스에 전부 | `SwapChain` / `DepthStencilSurface` / `RenderStates`로 분리 | 역할별 분리, 멀티스레드 개선 대비 |
| 사운드 | 없음 | `SoundEngine` (XAudio2, Cocos2d-x AudioEngine 구조) | `g_cSound.Play2d(...)` 한 줄 재생 |
| 애플리케이션 | 챕터마다 루프 직접 작성 | `Application` 클래스 + 생명주기 훅 | Cocos2d-x AppDelegate 방식 |
| DeltaTime | `float` 초 | `jc::TimeSpan` | 정밀도/의미 명확성, jc 규칙 준수 |
| 씬 그래프 | `Node`/`Sprite` + Child 계층 | **제거**. `Scene`이 `Renderer2D`로 직접 그림 | 불필요한 복잡도 제거 |
| 전역 접근 | `Director::Get()->...` | `g_cDirector`, `g_cSound`, `g_cInput` 등 매크로 | sgcl 코딩 규칙과 통일 |
| 콘솔/파일 IO | `printf`, `fgets` | `jc::Console`, `jc::FileStream` | jc 라이브러리 전면 사용 |

---

## 2. 전체 계층 구조

```
┌──────────────────────────────────────────────┐
│ Application (게임 루프, 생명주기 훅)          │
│  ├─ Window        : 창 + 메시지 펌프          │
│  │   └─ onActivated 이벤트 → 백/포그라운드 훅 │
│  ├─ InputManager  : 키보드/마우스 (폴링+이벤트)│
│  ├─ GraphicDevice : DX11 총괄 (파사드)        │
│  │   ├─ SwapChain           : 화면 표시       │
│  │   ├─ DepthStencilSurface : 깊이 버퍼       │
│  │   └─ RenderStates        : 상태 객체 모음   │
│  ├─ Renderer2D    : 2D 배치 렌더러            │
│  ├─ FrameTimer    : TimeSpan 기반 시간 측정    │
│  ├─ SoundEngine   : XAudio2 사운드 (싱글톤)   │
│  └─ Director      : 씬 관리 (싱글톤)          │
│      └─ Scene     : 게임 로직 + Camera 소유   │
└──────────────────────────────────────────────┘
```

---

## 3. 개선 항목별 상세

### 3-1. 카메라 사용성 (요구사항 1)

v1에서는 `Mat4::OrthographicOffCenterLH(...)`를 직접 계산해 `Renderer2D::Begin`에 넘겨야 했습니다.
v2 `Camera`는 **의도 단위의 API**를 제공합니다.

```cpp
// 2D: 창 크기만 알려주면 끝. 좌하단 (0,0) ~ 우상단 (w,h) 월드가 만들어진다.
GetCamera().SetOrthographic2D((_f32)g_cWindow.Width(), (_f32)g_cWindow.Height());

// 매 프레임 한 줄: WASD/방향키 이동 + 휠 줌
GetCamera().DriveDefault2D(g_cInput, _dt);

// 마우스 픽셀 좌표 → 월드 좌표 (클릭 판정에 바로 사용)
Vec2 world = GetCamera().ScreenToWorld2D((_f32)g_cInput.MouseX(), (_f32)g_cInput.MouseY());
```

3D도 마찬가지로 `SetPerspectiveDegrees`(라디안 대신 도 단위), `Orbit`/`Dolly`/`Pan`(마야 카메라 스타일),
`DriveDefault3D`를 제공해 행렬 지식 없이 조작할 수 있습니다.

### 3-2. D3D 개체 분리 (요구사항 2)

v1의 `GraphicDevice`는 디바이스/스와프체인/깊이버퍼/상태객체를 전부 들고 있는 '신(God) 클래스'였습니다.
v2는 **파사드(Facade) 패턴**으로 재구성했습니다.

- `SwapChain` : 백버퍼/화면 표시(Present) 담당
- `DepthStencilSurface` : 깊이/스텐실 텍스처와 뷰 담당
- `RenderStates` : 블렌드/래스터라이저/샘플러/깊이 상태 사전 생성본 보관
- `GraphicDevice` : 위 셋을 소유하고 기존과 **동일한 API**를 유지 (기존 챕터 코드 무수정)

이 분리는 겉보기 정리를 넘어, RnD 문서에서 다루는 **멀티스레드 렌더링**으로 가는 사전 작업입니다.
(스레드마다 만질 수 있는 객체의 경계가 명확해야 안전하게 분리할 수 있습니다.)

### 3-3. Cocos2d-x 구조 이식 (요구사항 3)

- **사운드**: `AudioEngine::play2d`와 같은 사용감의 `SoundEngine::Play2d` (audioId 반환, Stop/Pause/Resume/SetVolume/Preload/Uncache).
  WAV 로딩은 `jc::FileStream`, 캐시는 `jc::HashMap`, 오류 출력은 `jc::Console`로 처리해 std::를 쓰지 않습니다.
- **애플리케이션**: `ApplicationProtocol`의 생명주기 훅을 이식했습니다.
  `ApplicationDidFinishLaunching`(첫 씬 열기) / `DidEnterBackground`(자동 PauseAll) / `WillEnterForeground`(자동 ResumeAll) / `ApplicationDidExit`.
  창 활성/비활성은 `Window::onActivated`(`jc::Event<bool>`, WM_ACTIVATEAPP)로 감지합니다.
- **입력**: 기존 InputManager가 이미 cocos EventListener와 유사한 `jc::Event` 구독 + 폴링 병행 구조라 유지하고, Application이 `NextFrame()` 호출을 대신해주도록 통합했습니다.

### 3-4. TimeSpan DeltaTime (요구사항 4)

`FrameTimer`가 QueryPerformanceCounter 값을 마이크로초 틱의 `jc::TimeSpan`으로 변환합니다.
모든 갱신 함수는 `OnUpdate(const jc::TimeSpan& _dt)` 형태이며, 실수 초가 필요한 곳만 `GetTotalSeconds()`로 꺼냅니다.
(비정상적으로 긴 프레임은 250ms로 잘라 물리 폭주를 막습니다.)

### 3-5. Node/Sprite 제거 (요구사항 5)

`Scene::OnRender(Renderer2D*)`에서 `DrawSprite/DrawRect/DrawCircle/DrawLine`을 직접 호출합니다.
addChild를 해야만 그려지는 구조가 사라졌고, 부모-자식 움직임이 필요하면 14번 튜토리얼(태양계)처럼 좌표를 직접 계산합니다.

### 3-6. g_c 전역 접근 (요구사항 6)

sgcl의 `g_c` 접두사 레퍼런스 매크로 방식을 그대로 적용했습니다.

```cpp
g_cApp / g_cWindow / g_cDevice / g_cInput / g_cRenderer2D / g_cTimer  // Application.h
g_cDirector                                                            // Director.h
g_cSound                                                               // SoundEngine.h
```

---

## 4. 한 프레임의 흐름 (Application::Run)

```
PumpMessage → Tick(TimeSpan 측정)
 → g_cDirector.Update(dt) → OnUpdate(dt)            // 로직
 → BeginFrame(clear) → Director.Render → OnRender → EndFrame(vsync)   // 그리기
 → input.NextFrame() → g_cSound.Update()            // 프레임 마무리
```

## 5. sgcl 포팅 가이드

1. sgcl의 cocos `AudioEngine::play2d` 호출을 `g_cSound.Play2d`로 치환 (시그니처 동일 사고방식).
2. `AppDelegate`의 코드 → `Application` 파생 클래스의 같은 이름 훅으로 이동.
3. cocos `Director::getInstance()` → `g_cDirector` 매크로.
4. `Scene`은 `OnUpdate(TimeSpan)/OnRender(Renderer2D*)` 두 함수만 구현하면 됩니다.

## 6. 멀티스레드 렌더링 연구

`RnD/MultithreadRendering/` 폴더의 문서 6편을 참고하세요. (00_Glossary부터 순서대로)
