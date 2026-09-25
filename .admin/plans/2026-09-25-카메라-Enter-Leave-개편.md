# 카메라 View/Projection 비가상화 + Enter/Leave 도입

## 컨펌 결과 (사용자 확정)
1. 카메라: A안 — base에 `view_`/`projection_` 보관, `View()`/`Projection()`을 비상속 getter로 유지. 호출부 무변경.
2. 범위: Scene + GameObject까지 적용.
3. 별도 OnEntering 갈고리 없음 — `Enter()`/`Leave()` 자체를 virtual로 두고 Scene2D/3D가 override한다.

## 변경 1 — 카메라 (5개 파일)
- `Scene/Camera.h`: 순수가상 `View()`/`Projection()` 제거, 비상속 getter로 전환. `view_`/`projection_` 멤버를 base에 추가(Identity 초기화). `ViewProjection()` 캐시와 `InvalidateViewProjection()` 유지.
- `Scene/Camera2D.h`, `Scene/Camera3D.h`: `View()`/`Projection()` override와 private `view_`/`projection_` 제거. `Rebuild2D()`/`Rebuild3D()`는 상속 멤버에 기록한다.
- `Scene/Camera2D.cpp`, `Scene/Camera3D.cpp`: 생성자 초기화 목록에서 `view_`/`projection_` 초기화 제거.
- 호출부(`Scene3D.cpp`, `Renderer2D.cpp`) 무변경.

## 변경 2 — Enter/Leave (6개 파일)
- `Scene/Scene.h`: `virtual void Enter() { OnEnter(); }`, `virtual void Leave() { OnExit(); }` 추가. 관련 주석 갱신.
- `Scene/Scene2D.h/.cpp`, `Scene/Scene3D.h/.cpp`: `Enter()` override 선언/구현. `RenderScene()`에 있던 `IsConfigured()` 자동 구성 분기를 `Enter()`로 이동. `RenderScene()` 분기 제거.
- `Scene/Director.cpp`: `OnEnter()` 호출 3곳 → `Enter()`, `OnExit()` 호출 3곳 → `Leave()`. 관련 주석 갱신.
- `Scene/GameObject.h`: `virtual void Enter() { OnEnter(); }`, `virtual void Leave() { OnExit(); }` 추가.
- `Scene/GameObject.cpp`: `AddChild`의 `OnEnter()` → `Enter()`, `RemoveChild`의 `OnExit()` → `Leave()`. 관련 주석 갱신.
- 기존 파생(Shape2D/Shape3D, sgfr 튜토리얼)의 `OnEnter/OnExit` 재정의는 그대로 동작한다.

## 검증
- `Scripts\BuildProject\sgfr.bat -C Debug -P x64` 통과
- `Scripts\BuildProject-Multibyte\sgfr.bat -C Debug -P x64` 통과
