# Mesh StaticLevel 도입 — GameObject::SetMeshStaticLevel 제공

- 작성일: 2026-08-17
- 관련 문서: `.admin/plans/2026-08-17-엔진-코드리뷰-잔여.md` (A-1 / C-1 / C-2, B-4)
- 목적: 2D 프리미티브 메시를 **B(다이나믹 — 매 프레임 월드 변환) 기본**으로 하고,
  **명시적 함수 호출로 A(스태틱 — 1회 bake) 전환**할 수 있게 한다.

---

## 1. 사용자 요구 흐름 (확정)

```
Dynamic 상태로 이동 (Transform 반영) → 특정 위치 도달 → SetMeshStaticLevel(Static) 호출
→ 호출 시점의 현재 월드 행렬로 1회 bake → 이후 트랜스폼 변경은 무시 (위치 고정)
→ SetMeshStaticLevel(Dynamic) 호출 시 복귀 (다시 움직일 수 있음)
```

- 기본값 = Dynamic (B) — 기존 동작 그대로.
- A 전환은 반드시 명시적 호출로만.

## 2. 변경 파일 (5개 + 문서 1개 = 6개)

| # | 파일 | 변경 내용 |
|---|---|---|
| 1 | `Projects/sgf/Sources/sgf/Graphics/Mesh.h` | `StaticLevel` enum 추가 (Dynamic/Static), `BakeStatic(mat4)/UnbakeStatic/IsBaked/GetStaticLevel` 선언, bake 캐시 멤버(`bakedVertices_/bakedIndices_`) + 접근자 |
| 2 | `Projects/sgf/Sources/sgf/Graphics/Mesh.cpp` | BakeStatic/UnbakeStatic 구현 (2D 전용 — 비2D면 false), Finalize에서 bake 캐시 정리 |
| 3 | `Projects/sgf/Sources/sgf/Graphics/Renderer2D.cpp` | `DrawMesh`에 `IsBaked()` 분기 — bake 캐시(월드좌표) 그대로 PushTriangles, 월드 변환 생략 (틴트는 매 프레임 적용) |
| 4 | `Projects/sgf/Sources/sgf/Scene/GameObject.h` | `SetMeshStaticLevel(StaticLevel)`, `GetMeshStaticLevel()` 선언 |
| 5 | `Projects/sgf/Sources/sgf/Scene/GameObject.cpp` | SetMeshStaticLevel 구현 — Static: 현재 `GetWorldMatrix()`로 bake (재호출 시 재구움), Dynamic: Unbake |
| 6 | `Docs/sgf/05_리소스_관리와_머티리얼.md` | StaticLevel 개념 + 사용 예시 추가 |

## 3. 설계 세부

### 3-1. Mesh.h
```cpp
// 2D 메시 렌더 모드 — B 기본, A는 명시적 BakeStatic 전환
enum class StaticLevel : _u8
{
    Dynamic,   // B — 매 프레임 월드 변환 (기본)
    Static     // A — BakeStatic() 1회 구움 후 변환 생략
};

bool BakeStatic(const mat4& _world);   // 2D 전용. CPU 미러를 _world로 변환 → bake 캐시. Static 전환
bool UnbakeStatic();                   // bake 캐시 해제, Dynamic 복귀
bool IsBaked() const;
StaticLevel GetStaticLevel() const;

// bake 캐시 접근자 (Renderer2D용)
const VertexPTC* GetBakedVertices();
_u32 GetBakedVertexCount() const;
const _u16* GetBakedIndices();
_u32 GetBakedIndexCount() const;
```

- bake는 **월드(위치/회전/스케일)만** 구움. 색(틴트)은 매 프레임 재질에서 적용 — 색 변경 유연성 유지.
- `BakeStatic` 재호출 = 현재 전달 행렬 기준 재구움 (Unbake 없이도 새 위치 고정 가능).

### 3-2. Renderer2D::DrawMesh 분기
```cpp
if (_pMesh->IsBaked())
{
    // A — bake 캐시(이미 월드좌표) 그대로 배칭, 변환 계산 0회
    PushTriangles(pTexture, _pMesh->GetBakedVertices(), n, _pMesh->GetBakedIndices(), ic);
    return;
}
// B — 기존 CPU 월드 변환 + 틴트 경로 (변화 없음)
```

### 3-3. GameObject
```cpp
// GameObject.h
void SetMeshStaticLevel(StaticLevel _level);
StaticLevel GetMeshStaticLevel() const;

// GameObject.cpp
void GameObject::SetMeshStaticLevel(StaticLevel _level)
{
    if (pMesh_ == nullptr) return;
    if (_level == StaticLevel::Static)
    {
        pMesh_->BakeStatic(pTransform_->GetWorldMatrix());   // 현재 위치 기준 1회 구움
    }
    else
    {
        pMesh_->UnbakeStatic();
    }
}
```

## 4. 범위 제한

- 2D 메시(vfPTC2D) 전용. 3D 메시는 `BakeStatic` false 반환 (구조상 행렬 전달이 정상 경로).
- Shape2D(DeclareStatic 경로)는 이번 작업 범위 밖 — B-4에서 별도 정리.
- 기존 동작 회귀 없음: BakeStatic 미호출 시 100% 기존 경로.

## 5. 검증

1. `Scripts\BuildProject\sgfr.bat -C Debug -P x64` 빌드 성공
2. 튜토리얼 32 (2D 씬) 회귀 없음 확인 (런타임은 사용자 확인)
3. (선택) 튜토리얼에 StaticLevel 시연 예제 추가 여부는 사용자 결정

## 6. 참고 (향후 연결)

- C-1: baked 메시의 bake 캐시 → GPU IMMUTABLE 버퍼 1회 업로드로 진화 가능
- C-2: Dynamic 메시 → 월드 행렬 b1 상수버퍼 경로로 진화 가능