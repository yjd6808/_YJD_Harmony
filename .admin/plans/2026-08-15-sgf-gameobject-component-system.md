# sgf GameObject/Component 시스템 설계 문서 v3

- 작성일: 2026-08-15 (v3: 전체 클래스 구조 상세 + Camera2D/3D::CreateDefault 팩토리 + 렌더러 인자 규칙)
- 대상: `Projects/sgf` (엔진 라이브러리)
- 상태: **검토 대기** (승인 후 구현)

---

## 1. 클래스 전체 구조도

```
namespace sgf
│
├── Scene 계층 ────────────────────────────────────────────────
│   Scene            (추상: RenderScene 순수 가상, protected 생성자)
│   ├── Scene2D      (추상 기반: 2D 전용 동작 구현, protected 생성자)
│   └── Scene3D      (추상 기반: 3D 전용 동작 구현, protected 생성자)
│         └── 사용자: class MyGameScene : public sgf::Scene3D
│
├── Component 계층 ────────────────────────────────────────────
│   Component               (베이스: 소유자 + 타입 인덱스)
│   ├── Transform           (ctTransform  = 0, 모든 GameObject 기본 보유)
│   ├── Material            (ctMaterial   = 1, GameObject 기본 보유, ResourceBase+Component)
│   └── Camera              (추상 베이스: 화면을 비추는 부품)
│       ├── Camera2D        (ctCamera2D   = 2, 직교 투영)
│       └── Camera3D        (ctCamera3D   = 3, 원근 투영)
│
├── GameObject  (컴포넌트 하이브리드 보관 + 자식 트리 + 메시)
│
├── Director : jc::SingletonStatic<Director>   (씬 관리자, g_cDirector)
│
└── Graphics ──────────────────────────────────────────────────
    SceneRenderer   (즉시 그리기: mesh+material+world)
    Renderer2D : BatchRenderer   (g_cRenderer2D, Begin(Scene*) 추가)
    Renderer3D : BatchRenderer   (g_cRenderer3D, Begin(Scene*) 추가)
```

### 상속/소유 관계 한눈에

```
Scene ──소유──▶ defaultCamera_ (GameObject) ──컴포넌트──▶ Camera2D or Camera3D
  │                  ▲ selectedCamera_ (기본 = defaultCamera_)
  └──소유──▶ gameObjects_ (GameObject 목록)
                └──소유──▶ children_ (자식 GameObject 트리)
                              각 GameObject ──소유──▶ 컴포넌트 보관 (하이브리드)
                                                         Transform(기본 멤버) / Material(기본 멤버)
                                                         + Vector<Component*> (Camera, 커스텀 등)
                              Mesh ──빌림──▶ ResourceMgr (소유)
```

---

## 2. 컴포넌트 상수 대역 (엔진 제공)

### 2-1. 컴포넌트 타입 인덱스 대역

```cpp
enum class ComponentType : _u32
{
    ctTransform = 0,     // 엔진: 위치/회전/크기 (GameObject 기본 멤버)
    ctMaterial  = 1,     // 엔진: 그리는 방법 (GameObject 기본 멤버)
    ctCamera2D  = 2,     // 엔진: 직교 카메라
    ctCamera3D  = 3,     // 엔진: 원근 카메라
    // 4~99: 엔진 예약 (Light, AudioSource 등)
    ctCustomBase = 100,  // 사용자 정의 컴포넌트 시작
};

// 타입 식별자 (GetComponent의 Vector 선형 스캔 비교용 + GetTypeIndex() 구현용)
#define SGF_COMPONENT_TYPE(typeIndex)                                  \
    static constexpr _u32 TypeIndex() { return (_u32)typeIndex; }      \
    virtual _u32 GetTypeIndex() const override { return (_u32)typeIndex; }
```

### 2-2. 컴포넌트 상수버퍼 슬롯 대역 (cbuffer band)

| 슬롯 | 소유 | 내용 |
|---|---|---|
| b0 | SceneRenderer | 프레임 공통 (view/projection/카메라 위치) |
| b1 | SceneRenderer | 오브젝트 (world) |
| b2 | Material 컴포넌트 | baseColor |
| b3 | Camera 컴포넌트 | fov/평면/카메라 파라미터 (v1: 예약만) |
| b4~ | 추후 컴포넌트 | Light 등 |

---

## 3. 클래스 상세 정의

### 3-1. Component (`Scene/Component.h`)

```cpp
class GameObject;   // 전방 선언

class Component
{
public:
    virtual ~Component() = default;

    // 소속 게임오브젝트 (GameObject::AddComponent가 설정)
    GameObject* GetOwner() const { return pOwner_; }

    // 부착 시 1회 (OnAttach 후 GameObject가 컴포넌트 OnUpdate를 매 프레임 호출)
    virtual void OnAttach(GameObject* _pOwner) { pOwner_ = _pOwner; }
    virtual void OnUpdate(const jc::TimeSpan& _dt) { (void)_dt; }

    // 타입 인덱스 (SGF_COMPONENT_TYPE 매크로가 구현)
    virtual _u32 GetTypeIndex() const = 0;

private:
    friend class GameObject;
    GameObject* pOwner_ = nullptr;
};
```

### 3-2. Transform (`Scene/Transform.h` / `.cpp`) — `SGF_COMPONENT_TYPE(ctTransform)`

```cpp
class Transform : public Component
{
    SGF_COMPONENT_TYPE(ComponentType::ctTransform)

public:
    // === 로컬 상태 (설정 즉시 dirty → 자식 트리로 전파) ===
    void SetPosition(const vec3& _pos);        const vec3& GetPosition() const;
    void Translate(const vec3& _delta);        // 상대 이동
    void SetRotation(const vec3& _eulerDeg);   const vec3& GetRotation() const;   // 오일러 각도(도)
    void Rotate(const vec3& _eulerDeltaDeg);   // 상대 회전
    void SetScale(const vec3& _scale);         const vec3& GetScale() const;

    // === 행렬 (요청 시점에 1회 재계산, 캐시) ===
    const mat4& GetLocalMatrix() const;   // Scale * Rx * Ry * Rz * Translation
    const mat4& GetWorldMatrix() const;   // (부모) 부모World * local / (없음) local
    vec3 GetWorldPosition() const;        // world.m[3][0..2]

private:
    void MarkDirty();                     // 설정 변경 시 호출, 자식 Transform에 재귀 전파

private:
    vec3 position_ = vec3::Zero();        // 로컬 위치
    vec3 rotation_ = vec3::Zero();        // 로컬 회전 (오일러, 도)
    vec3 scale_    = vec3::One();         // 로컬 크기
    mat4 localMatrix_;                    // 캐시
    mat4 worldMatrix_;                    // 캐시
    bool localDirty_  = true;
    bool worldDirty_  = true;
};
```

### 3-3. Camera (`Scene/Camera.h`) — 추상 베이스

```cpp
class Camera : public Component
{
public:
    virtual ~Camera() = default;

    virtual mat4 View() const = 0;         // 월드 → 카메라 기준
    virtual mat4 Projection() const = 0;   // 카메라 기준 → NDC
    mat4 ViewProjection() const { return View() * Projection(); }

    bool enabled_ = true;                  // false면 렌더에서 제외 (미래: 다중 카메라 분할 화면)
};
```

### 3-4. Camera2D (`Scene/Camera2D.h` / `.cpp`) — `SGF_COMPONENT_TYPE(ctCamera2D)`

```cpp
class Camera2D : public Camera
{
    SGF_COMPONENT_TYPE(ComponentType::ctCamera2D)

public:
    // === [팩토리] 디폴트 카메라 게임오브젝트 생성 ===
    // 이름 "DefaultCamera2D"의 GameObject를 만들어 Camera2D 컴포넌트를 부착하고 반환.
    // (DefaultCamera2D라는 별도 클래스는 없다 — GameObejct + Camera2D 조합일 뿐)
    static GameObject* CreateDefault();

    // === 2D 직교 설정 ===
    void SetOrthographic2D(_f32 _width, _f32 _height);   // 왼쪽 아래 (0,0), Y 위+
    void SetPosition2D(const vec2& _position);           // 카메라 중심
    const vec2& Position2D() const;
    void Move2D(const vec2& _delta);
    void SetZoom(_f32 _zoom);  _f32 Zoom() const;
    void ZoomBy(_f32 _scale);

    // === 화면좌표 변환 ===
    vec2 ScreenToWorld2D(_f32 _screenX, _f32 _screenY) const;   // 마우스 → 월드
    vec2 WorldToScreen2D(const vec2& _world) const;             // 월드 → 픽셀

    // === 표준 조작 (방향키 이동 + 휠 줌) ===
    void DriveDefault2D(InputManager& _input, const jc::TimeSpan& _dt);

    // === lazy 초기화 지원 ===
    bool IsConfigured() const { return configured_; }

private:
    void Rebuild();   // 상태 → view/projection 행렬 갱신

private:
    bool configured_ = false;
    _f32 width2D_ = 0.0f;  _f32 height2D_ = 0.0f;   // 가상 화면 크기
    vec2 position2D_ = vec2::Zero();                 // 카메라 중심
    _f32 zoom_ = 1.0f;                               // 줌 배율
    mat4 view_;  mat4 projection_;
};
```

### 3-5. Camera3D (`Scene/Camera3D.h` / `.cpp`) — `SGF_COMPONENT_TYPE(ctCamera3D)`

```cpp
class Camera3D : public Camera
{
    SGF_COMPONENT_TYPE(ComponentType::ctCamera3D)

public:
    // === [팩토리] 디폴트 카메라 게임오브젝트 생성 (이름 "DefaultCamera3D") ===
    static GameObject* CreateDefault();

    // === 3D 원근 설정 ===
    void SetPerspectiveDegrees(_f32 _fovYDeg, _f32 _aspect, _f32 _nearZ, _f32 _farZ);
    void SetLookAt(const vec3& _eye, const vec3& _target, const vec3& _up = vec3::Up());
    const vec3& Eye() const;  const vec3& Target() const;

    // === 궤도/이동 조작 ===
    void Orbit(_f32 _yawDelta, _f32 _pitchDelta);   // Target 중심 공전
    void Dolly(_f32 _distance);                     // 전진/후퇴
    void Pan(_f32 _rightDelta, _f32 _upDelta);      // 평행 이동
    void MoveForward(_f32 _d);  void MoveRight(_f32 _d);  void MoveUp(_f32 _d);

    // === 표준 조작 (방향키 Orbit + WASD + 휠 Dolly) ===
    void DriveDefault3D(InputManager& _input, const jc::TimeSpan& _dt);

    bool IsConfigured() const { return configured_; }

private:
    void Rebuild();   // Eye/Target/Up → view, 파라미터 → projection 갱신

private:
    bool configured_ = false;
    _f32 fovY_ = 0.0f;  _f32 aspect_ = 1.0f;  _f32 nearZ_ = 0.1f;  _f32 farZ_ = 1000.0f;
    vec3 eye_ = vec3::Zero();  vec3 target_ = vec3::Zero();  vec3 up_ = vec3::Up();
    mat4 view_;  mat4 projection_;
};
```

### 3-6. GameObject (`Scene/GameObject.h` / `.cpp`)

```cpp
class GameObject
{
public:
    explicit GameObject(const char* _szName = "");
    virtual ~GameObject();   // 컴포넌트 전부 + 자식 전부 정리

    // === 컴포넌트 (하이브리드: 기본 컴포넌트 = 멤버 O(1), 나머지 = Vector 선형 스캔) ===
    template <typename T>
    T* GetComponent()
    {
        // 1) 기본 컴포넌트 (모든 GameObject가 항상 보유) — 멤버 즉시 반환 (Unity의 transform fast-path)
        if constexpr (std::is_same_v<T, Transform>) return pTransform_;
        if constexpr (std::is_same_v<T, Material>)  return pMaterial_;

        // 2) 그 외 컴포넌트 (Camera, 커스텀) — Vector 선형 스캔 (n은 보통 5~10 이하)
        for (Component* pComponent : components_)
        {
            if (pComponent->GetTypeIndex() == T::TypeIndex())
                return static_cast<T*>(pComponent);
        }
        return nullptr;
    }

    template <typename T>
    T* AddComponent();               // 새로 생성해 벡터에 부착 (같은 타입 다중 부착 가능)

    template <typename T>
    void RemoveComponent();          // Transform/Material은 기본 보유라 제거 불가 (assert)

    Transform* GetTransform() const { return pTransform_; }   // O(1) 멤버 접근
    Material*  GetMaterial()  const { return pMaterial_; }    // O(1) 멤버 접근
    Camera*    GetCameraComponent() const;        // Camera2D → Camera3D 순으로 탐색

    // === ChildElement (자식 GameObject 트리) ===
    void AddChildElement(GameObject* _pChild);    // 소유권 이전 + 부모 연결
    bool RemoveChildElement(GameObject* _pChild); // 부모에서 분리 (delete 안 함)
    GameObject* GetParent() const;
    int  GetChildCount() const;
    GameObject* GetChild(int _index) const;

    // === 메시 / 렌더 ===
    void SetMesh(Mesh* _pMesh);      Mesh* GetMesh() const;   // 빌림 (ResourceMgr 소유)
    void SetVisible(bool _visible);  bool IsVisible() const;

    // === 이름 ===
    void SetName(const jc::String& _name);  const jc::String& GetName() const;

    // === 엔진 내부 (Scene이 호출) ===
    void Initialize(GraphicDevice* _pDevice);      // 기본 Material GPU 초기화, 자식 재귀
    void Update(const jc::TimeSpan& _dt);          // 컴포넌트 OnUpdate 순회 + 자식 재귀

private:
    friend class Scene;  friend class Transform;
    void SetScene(Scene* _pScene);                 // 소속 씬 설정 (자식 재귀)

private:
    jc::String name_;
    Transform* pTransform_ = nullptr;            // 기본 멤버 (모든 GameObject가 보유, 생성 시 new)
    Material*  pMaterial_  = nullptr;            // 기본 멤버 (모든 GameObject가 보유, 생성 시 new)
    jc::Vector<Component*> components_;          // 그 외 컴포넌트 (Camera, 커스텀 — 소유)
    GameObject* pParent_ = nullptr;
    jc::Vector<GameObject*> children_;           // 소유
    Scene* pScene_ = nullptr;
    Mesh* pMesh_ = nullptr;                      // 빌림
    bool visible_ = true;
    bool initialized_ = false;
};
```

### 3-7. Material (`Graphics/Material.h` / `.cpp`) — `SGF_COMPONENT_TYPE(ctMaterial)`

```cpp
class Material : public ResourceBase, public Component
{
    SGF_COMPONENT_TYPE(ComponentType::ctMaterial)

public:
    Material();  ~Material();

    // === 기존 API 전부 유지 ===
    bool Initialize(GraphicDevice* _pDevice);
    void Finalize();
    void SetVertexShaderKey(_u64);  void SetPixelShaderKey(_u64);
    void SetTextureKey(_u32 _slot, _u64 _key);  _u64 GetTextureKey(_u32 _slot) const;
    bool SetRasterizer(CullMode, FillMode = ..., FrontFace = ...);
    bool SetBlend(BlendMode);  bool SetDepth(DepthMode);  bool SetSampler(...);
    void SetBaseColor(const color&);  const color& GetBaseColor() const;
    bool Bind(GraphicContext&);

    // === [신규] 게임오브젝트 기본 머티리얼 ===
    bool InitializeWithDefault3D(GraphicDevice* _pDevice);   // 상태 + 3D 디폴트 셰이더 키

    // 기존 멤버: pDevice_, shader 키들, textureKeys_, 파이프라인 상태들,
    //           MaterialConstants constants_, ConstantBuffer<MaterialConstants> ...
};
```

### 3-8. Scene (`Scene/Scene.h` / `.cpp`) — 추상

```cpp
class Scene
{
public:
    virtual ~Scene();   // defaultCamera_ + gameObjects_ 전부 정리

    // === 카메라 운용 (여러 카메라 중 화면을 비출 카메라 선택) ===
    GameObject* GetDefaultCamera() const;    // 씬이 자동 생성한 기본 카메라 오브젝트
    GameObject* GetSelectedCamera() const;   // 현재 화면을 비추는 카메라 오브젝트
    Camera*     GetCamera() const;           // = selectedCamera_의 카메라 컴포넌트
    void        SelectCamera(GameObject* _pCameraObj);  // 전환 (nullptr 금지)
    void        SelectDefaultCamera();                   // = SelectCamera(defaultCamera_)

    // === GameObject 관리 ===
    void AddGameObject(GameObject* _pObject);     // 소유권 이전 + 즉시 GPU 초기화 (자식 재귀)
    void RemoveGameObject(GameObject* _pObject);  // delete (자식 포함)
    GameObject* FindGameObjectByName(const char* _szName);   // 자식까지 재귀
    int  GetGameObjectCount() const;
    GameObject* GetGameObject(int _index) const;

    // === 생명주기 (사용자 훅) ===
    virtual void OnEnter() {}                    // 씬 시작 시 1회
    virtual void OnExit() {}                     // 씬 종료 시 1회
    virtual void OnUpdate(const jc::TimeSpan& _dt) {}   // 매 프레임
    virtual void OnRender() {}                   // 매 프레임 (렌더러 인자 규칙: 4장)
    Window* GetWindow() const { return pWindow_; }

    // === 렌더 진입점 (2D/3D 동작 분기 — 추상) ===
    virtual void RenderScene() = 0;

protected:
    Scene();                                     // 직접 생성 금지 (추상)
    void CreateDefaultCamera(GameObject* _pCameraObj);  // 파생 생성자에서 호출

private:
    friend class Director;
    void SetWindow(Window* _pWindow);

    // 엔진 내부 (Director가 호출)
    void UpdateGameObjects(const jc::TimeSpan& _dt);   // 루트 오브젝트 Update 재귀
    void RenderGameObjects();                          // mesh+material+visible 오브젝트 자동 렌더

private:
    GameObject* defaultCamera_ = nullptr;    // 소유 (Camera2D::CreateDefault / Camera3D::CreateDefault)
    GameObject* selectedCamera_ = nullptr;   // 기본 = defaultCamera_
    jc::Vector<GameObject*> gameObjects_;    // 루트 오브젝트 (소유)
    Window* pWindow_ = nullptr;
};
```

### 3-9. Scene2D (`Scene/Scene2D.h` / `.cpp`) — 추상 기반

```cpp
class Scene2D : public Scene
{
public:
    virtual ~Scene2D();

    Camera2D* GetCamera2D() const;   // = static_cast<Camera2D*>(GetCamera())

    virtual void RenderScene() override;

protected:
    Scene2D();   // 직접 생성 금지 (상속 전용): defaultCamera_ = Camera2D::CreateDefault()

    // (필요 시 사용자 2D 씬 공통 로직용 훅 추가 가능)
};
```

```cpp
Scene2D::Scene2D()
{
    CreateDefaultCamera(Camera2D::CreateDefault());   // "DefaultCamera2D" 오브젝트 + Camera2D
}

void Scene2D::RenderScene()
{
    if (!GetCamera2D()->IsConfigured())          // 첫 렌더: 창 크기로 자동 구성
        GetCamera2D()->SetOrthographic2D((_f32)GetWindow()->Width(), (_f32)GetWindow()->Height());

    g_cRenderer2D.Begin(this);                   // ← 렌더러가 씬을 인자로 받음
    RenderGameObjects();                         // 자동 렌더 (2D 오브젝트)
    OnRender();                                  // 수동 그리기 (2D 배치만 열려 있음)
    g_cRenderer2D.End();
}
```

### 3-10. Scene3D (`Scene/Scene3D.h` / `.cpp`) — 추상 기반

```cpp
class Scene3D : public Scene
{
public:
    virtual ~Scene3D();

    Camera3D* GetCamera3D() const;   // = static_cast<Camera3D*>(GetCamera())

    virtual void RenderScene() override;

protected:
    Scene3D();   // 직접 생성 금지 (상속 전용): defaultCamera_ = Camera3D::CreateDefault()
};
```

```cpp
Scene3D::Scene3D()
{
    CreateDefaultCamera(Camera3D::CreateDefault());   // "DefaultCamera3D" 오브젝트 + Camera3D
}

void Scene3D::RenderScene()
{
    if (!GetCamera3D()->IsConfigured())
    {
        GetCamera3D()->SetPerspectiveDegrees(60.0f, GetWindow()->AspectRatio(), 0.1f, 1000.0f);
        GetCamera3D()->SetLookAt(vec3(0.0f, 3.0f, -8.0f), vec3::Zero());
    }

    g_cRenderer3D.Begin(this);       // 3D 배치 (깊이 켬, 바닥)
    g_cRenderer2D.Begin(this);       // 2D 배치 (깊이 끔, UI 위)
    RenderGameObjects();             // 자동 렌더 (3D 오브젝트, selectedCamera 기준)
    OnRender();                      // 수동: 3D/2D 혼합 그리기 가능
    g_cRenderer3D.End();             // 3D 먼저 플러시
    g_cRenderer2D.End();             // 2D 나중 플러시 (위에 얹힘)
}
```

### 3-11. Director (`Scene/Director.h` / `.cpp`) — 기존 + 자동 갱신/렌더 연결

```cpp
class Director : public jc::SingletonStatic<Director>
{
    // 기존 API 유지: RunScene/ReplaceScene/CurrentScene/DetachWindow/Cleanup

    void Update(const jc::TimeSpan& _dt);   // [변경]
    //   씬 전환 처리 → 각 씬:
    //     pScene->OnUpdate(_dt);            (수동 훅)
    //     pScene->UpdateGameObjects(_dt);   (자동: 컴포넌트+자식 재귀)

    void Render(Window* _pWindow);          // [변경]
    //   pScene->RenderScene();              (가상: Scene2D/3D 분기)
};
```

---

## 4. 렌더러 인자 규칙 (OnRender에서의 그리기 규약)

### 4-1. 렌더러 시그니처

```cpp
// Renderer2D.h (BatchRenderer 상속)
class Renderer2D : public BatchRenderer
{
public:
    void Begin(Scene* _pScene);          // [신규] 씬의 selectedCamera ViewProjection으로 배치 시작
    void Begin(const mat4& _viewProj);   // [유지] 저수준 직접 지정 (씬 밖 코드용)
    // DrawXxx(...)는 기존 그대로 — 배치가 이미 씬 카메라로 열려 있음
};

// Renderer3D.h — 동일 구조
class Renderer3D : public BatchRenderer
{
public:
    void Begin(Scene* _pScene);
    void Begin(const mat4& _viewProj);
    // DrawCube/DrawGrid/... 기존 그대로
};
```

### 4-2. 인자 전달 규칙 (3단계)

| 단계 | 누가 | 호출 | 씬 인자 |
|---|---|---|---|
| 1. 배치 열기 | 엔진 (Scene2D/3D::RenderScene) | `g_cRenderer2D.Begin(this)` | ✅ 씬 포인터 — selectedCamera의 ViewProjection을 렌더러가 기억 |
| 2. 그리기 | 사용자 (OnRender 안) | `g_cRenderer2D.DrawRect(...)` | ❌ 불필요 — 1단계에서 받은 씬 카메라로 자동 적용 |
| 3. 배치 닫기 | 엔진 | `g_cRenderer2D.End()` | ❌ |

- **OnRender 내부**에서는 절대 씬/카메라를 인자로 넘기지 않는다.
  배치가 열린 시점에 렌더러가 씬(선택 카메라)을 기억하고 있기 때문.
- **씬 밖**에서 즉시 사용 시: `g_cRenderer2D.Begin(pScene); ...Draw...; End();` — 씬을 직접 전달.
- **저수준** 사용 시: `Begin(mat4)` — 카메라 행렬을 직접 전달.

### 4-3. 씬 타입별 사용 가능 렌더러 (RenderScene에서 열리는 배치)

| 씬 | OnRender에서 쓸 수 있는 것 | 금지 |
|---|---|---|
| Scene2D | `g_cRenderer2D.*` | `g_cRenderer3D.*` (배치가 안 열려 있음 — assert/무시) |
| Scene3D | `g_cRenderer3D.*` + `g_cRenderer2D.*` | 없음 |

### 4-4. 즉시 렌더 (게임오브젝트 자동 렌더 경로)

```cpp
// Scene::RenderGameObjects (엔진 내부)
for (obj : 루트 오브젝트 + 자식 재귀)
{
    if (obj->GetMesh() && obj->IsVisible())
        g_cSceneRenderer.Draw(obj->GetMesh(), obj->GetMaterial(), obj->GetTransform()->GetWorldMatrix());
}
```

- 게임오브젝트는 `SceneRenderer`(즉시 그리기, mesh+material+월드행렬 3개 인자)로 그려진다.
- OnRender에서 즉시 3D 디버그 도형을 그리려면 `g_cRenderer3D.DrawCube(...)` (배치 경유).

---

## 5. 수도 코드 (최종 사용 예시)

### 예시 A. 3D 씬 — 기본 + 카메라 2대 전환

```cpp
// [커스텀 컴포넌트] — 상수 대역 100부터
class Rotator : public sgf::Component
{
    SGF_COMPONENT_TYPE(ComponentType::ctCustomBase)
public:
    virtual void OnUpdate(const jc::TimeSpan& _dt) override
    {
        GetOwner()->GetTransform()->Rotate(vec3(0.0f, 60.0f * (float)_dt.GetTotalSeconds(), 0.0f));
    }
};

// [3D 씬] — Scene3D 상속 (추상이므로 반드시 사용자 씬 정의)
class GameScene : public sgf::Scene3D
{
public:
    virtual void OnEnter() override
    {
        // 1. 기본 카메라는 이미 있음 (DefaultCamera3D + Camera3D, 첫 렌더 시 자동 구성)
        //    원하면 직접 재설정:
        GetCamera3D()->SetLookAt(vec3(0.0f, 2.0f, -5.0f), vec3::Zero());

        // 2. 메시 (ResourceMgr 등록)
        Mesh* pCubeMesh = dbg_new Mesh;
        pCubeMesh->InitializeAsCube(&g_cDevice, g_cResourceMgr.GetDefaultVertexShader3D());
        g_cResourceMgr.Add(pCubeMesh);

        // 3. 게임오브젝트
        GameObject* pCube = new GameObject("큐브");
        pCube->GetTransform()->SetPosition(vec3(0.0f, 0.5f, 0.0f));
        pCube->SetMesh(pCubeMesh);
        pCube->GetMaterial()->SetBaseColor(color::Red());
        pCube->AddComponent<Rotator>();
        AddGameObject(pCube);

        // 4. 감시 카메라 — Camera3D 컴포넌트를 단 GameObject
        GameObject* pWatchCam = new GameObject("감시카메라");
        pWatchCam->GetTransform()->SetPosition(vec3(10.0f, 10.0f, 0.0f));  // Transform으로 위치 제어
        Camera3D* pCam3D = pWatchCam->AddComponent<Camera3D>();
        pCam3D->SetLookAt(vec3(10.0f, 10.0f, 0.0f), vec3::Zero());
        AddGameObject(pWatchCam);

        SelectCamera(pWatchCam);      // 감시 카메라로 전환 → 화면이 바뀜
        SelectDefaultCamera();        // 다시 기본 카메라로
    }

    virtual void OnUpdate(const jc::TimeSpan& _dt) override
    {
        if (g_cInput.IsKeyPressed(VK_SPACE))
        {
            const bool watching = (GetSelectedCamera() == GetDefaultCamera());
            SelectCamera(watching ? FindGameObjectByName("감시카메라") : GetDefaultCamera());
        }
    }

    virtual void OnRender() override
    {
        // [인자 규칙 2단계] 배치는 엔진이 이미 열어둠 → 씬 인자 없이 그리기만
        g_cRenderer3D.DrawCube(vec3(0.0f, 3.0f, 0.0f), vec3(0.2f), color::Yellow());  // 디버그 표시
        g_cRenderer2D.DrawRect(10.0f, 10.0f, 200.0f, 30.0f, color::White());          // UI
    }
};

g_cDirector.RunScene(new GameScene());
```

### 예시 B. 2D 씬

```cpp
class TitleScene : public sgf::Scene2D
{
public:
    virtual void OnEnter() override
    {
        // 기본 카메라(DefaultCamera2D)는 첫 렌더 시 창 크기로 자동 직교 구성됨
        GetCamera2D()->SetZoom(2.0f);   // 원하면 조정

        Mesh* pQuad = dbg_new Mesh;
        pQuad->InitializeAsQuad2D(&g_cDevice, g_cResourceMgr.GetDefaultVertexShader2D());
        g_cResourceMgr.Add(pQuad);

        GameObject* pSprite = new GameObject("타이틀");
        pSprite->GetTransform()->SetPosition(vec3(100.0f, 200.0f, 0.0f));
        pSprite->GetTransform()->SetScale(vec3(128.0f, 64.0f, 1.0f));
        pSprite->SetMesh(pQuad);
        pSprite->GetMaterial()->SetTextureKey(0, g_cResourceMgr.LoadTextureFromFile("Resources/title.png"));
        AddGameObject(pSprite);
    }
};
```

### 예시 C. ChildElement + 월드 행렬 자동 합성

```cpp
GameObject* pPlayer = new GameObject("플레이어");
pPlayer->SetMesh(pCubeMesh);
AddGameObject(pPlayer);

GameObject* pGun = new GameObject("총");
pGun->GetTransform()->SetPosition(vec3(0.5f, 0.8f, 0.0f));   // 부모 기준 로컬
pPlayer->AddChildElement(pGun);

pPlayer->GetTransform()->Translate(vec3(3.0f, 0.0f, 0.0f));  // 부모 이동 → 총도 자동으로 따라감
```

---

## 6. 파일 목록

| 파일 | 작업 | 내용 |
|---|---|---|
| `Scene/Component.h` | 신규 | Component 베이스 + ComponentType 상수 대역 |
| `Scene/Transform.h` | 신규 | SRT + 행렬 캐시 + dirty 전파 |
| `Scene/Transform.cpp` | 신규 | 행렬 재계산 |
| `Scene/Camera.h` | **대체** | Camera 추상 베이스 (기존 Camera.h 교체) |
| `Scene/Camera2D.h` | 신규 | 기존 2D 기능 이관 + `CreateDefault()` |
| `Scene/Camera2D.cpp` | 신규 | 구현 |
| `Scene/Camera3D.h` | 신규 | 기존 3D 기능 이관 + `CreateDefault()` |
| `Scene/Camera3D.cpp` | 신규 | 구현 |
| `Scene/GameObject.h` | 신규 | 컴포넌트 하이브리드 보관(멤버+Vector)/자식/메시 |
| `Scene/GameObject.cpp` | 신규 | 구현 |
| `Scene/Scene.h` | 수정 | defaultCamera_/selectedCamera_/RenderScene/카메라 운용 API |
| `Scene/Scene.cpp` | 신규 | GameObject 관리 + Update/RenderGameObjects |
| `Scene/Scene2D.h` | 신규 | 2D 씬 기반 (추상) |
| `Scene/Scene2D.cpp` | 신규 | 구현 |
| `Scene/Scene3D.h` | 신규 | 3D 씬 기반 (추상) |
| `Scene/Scene3D.cpp` | 신규 | 구현 |
| `Scene/Director.cpp` | 수정 | Render → RenderScene, Update → UpdateGameObjects |
| `Graphics/Renderer2D.h/.cpp` | 수정 | `Begin(Scene*)` 추가 |
| `Graphics/Renderer3D.h/.cpp` | 수정 | `Begin(Scene*)` 추가 |
| `Graphics/Material.h/.cpp` | 수정 | Component 상속 + InitializeWithDefault3D |
| `SgfHeader.h` | 수정 | 신규 헤더 include |
| `Project/sgf.vcxproj` + `.filters` | 수정 | 신규 파일 등록 |

## 7. 작업 순서 / 검증

1. Component.h(상수 대역) → Transform → Camera/Camera2D/Camera3D → GameObject → Scene/Scene2D/Scene3D → Material → Renderer → Director → SgfHeader → vcxproj
2. 빌드: `Scripts\BuildProject\sgf.bat -P x64 -C Debug`
3. 호환: Scene 훅(OnEnter/OnUpdate/OnRender), `g_c*` 매크로 유지. `GetCamera()`가 `Camera*`로 변경 → 2D/3D 전용은 `GetCamera2D()/GetCamera3D()` (sgcl 수정 가능성 안내)

## 8. 범위 밖

- 쿼터니언 회전 / Camera cbuffer b3 실제 사용 / Light·AudioSource 등 추가 디폴트 컴포넌트 / GetComponents(동일 타입 전체 반환)·직렬화·프리팹 / 멀티 카메라 분할화면(enabled_ 예약만)
