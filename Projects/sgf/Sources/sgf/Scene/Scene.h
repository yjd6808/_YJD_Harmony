/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:34:00 AM
 * 수정일: 8/16/2026 (추상화 + root_ 트리 + 카메라 GameObject 파생 + 예약 창구)
 * =====================
 * 씬(Scene) — 추상 베이스
 *
 * [씬이란?]
 * 게임의 "화면 한 장". 타이틀 화면, 인게임 화면, 결과 화면 등.
 * Cocos2d-x의 cocos2d::Scene과 같은 역할이다.
 * Director가 윈도우별로 씬을 들고 있으며, 매 프레임 Update/RenderScene을 불러준다.
 *
 * [바뀐 점]
 * 1. **추상 클래스** — 직접 생성 금지. Scene2D/Scene3D 파생만 생성한다. (12장 결정)
 * 2. **root_ 트리** — GameObject 파생(Camera/Node/Layer 등)을 AddChild로 트리 구성.
 * 트리 순회 순서 = 드로우 순서 (zOrder 오름차순, 부모 먼저).
 * 3. **카메라 = GameObject 파생** — defaultCamera_/selectedCamera_ 포인터로 보유.
 * SelectCamera(GameObject*)로 전환. (기존 값 멤버 camera_ 제거)
 * 4. **예약 창구 (가상)** — Scene2D(2D)/Scene3D(3D)가 구현. 게임 코드는 씬에만 접근.
 * RenderStatic(id) / RenderDynamic(region, fill, ...) / DrawMesh(mesh, mat, world)
 * 5. **OnRender() 무인자** — 트래버설 후 최상위 수동 그리기. (유지)
 * 6. **Scene도 DataMap 보유** — GameObject와 동일하게 이름 키 → 값.
 *
 * [생성 계약]
 * - 생성자: root_.SetScene(this) + 파생이 CreateDefaultCamera()로 기본 카메라 생성.
 * - Director::RunScene 순서: running 등록 → 씬 OnEnter → (프레임마다 Update/RenderScene).
 * OnEnter 안에서 AddChild가 running 검증을 통과하려면 등록이 OnEnter보다 먼저다.
 */

#pragma once

#include "jc/Math.h"
#include "jc/Time.h"
#include "jc/Container/DataMap.h"
#include "sgf/Scene/GameObject.h"
#include "sgf/Scene/Camera.h"
#include "sgf/Graphics/Fill.h"

NS_SGF_BEGIN

using namespace jc;

class Director;
class Window;
class GraphicDevice;
class Mesh;
class Material;

class Scene
{
public:
	virtual ~Scene();						// root_ 서브트리 + defaultCamera_ 정리

	////////////////////////////////////////////////////////////////////////////////////////
	// 카메라 운용 (— 카메라 = GameObject 파생)
	Camera* GetDefaultCamera() const { return defaultCamera_; }		// 씬이 자동 생성한 기본 카메라
	Camera* GetSelectedCamera() const { return selectedCamera_; }	// 현재 화면을 비추는 카메라
	Camera* GetCamera() const { return selectedCamera_; }			// = selectedCamera_의 Camera*
	void    SelectCamera(GameObject* _pCameraObj);					// 전환 (카메라인 GameObject만)
	void    SelectDefaultCamera();									// = SelectCamera(defaultCamera_)

	////////////////////////////////////////////////////////////////////////////////////////
	// 트리 (— root_ 노드에 위임)
	void AddChild(GameObject* _pChild, _u64 _zOrder);	// 루트 직속 자식 (레이어 추가)
	void RemoveChild(GameObject* _pChild);
	GameObject* FindGameObjectByName(const jc::String& _name);	// root_ 서브트리 재귀
	int  GetGameObjectCount() const;						// root_ 서브트리 노드 수 (재귀)

	////////////////////////////////////////////////////////////////////////////////////////
	// 데이터 보관 (— Scene도 DataMap 보유)
	jc::CDataMap<>& GetDataMap() { return dataMap_; }

	////////////////////////////////////////////////////////////////////////////////////////
	// 예약 창구 (— Scene2D/3D가 구현, 타입 불일치 시 no-op)
	virtual void RenderStatic(_u64 _staticId) {}			// 선언 시 고정 — id만 예약 (3D 씬 = no-op)
	virtual void RenderDynamic(const rect& _region, const Fill& _fill,
		const color& _color1 = color::WHITE, const color& _color2 = color::WHITE,
		_u32 _option = 0, RenderLayer _layer = RenderLayer::Default) {}	// (3D 씬 = no-op)
	virtual void DrawMesh(Mesh* _pMesh, Material* _pMaterial, const mat4& _world) {}	// 3D 창구 (2D 씬 = no-op)

	////////////////////////////////////////////////////////////////////////////////////////
	// 생명주기 (— 씬도 수명주기 훅 보유)
	virtual void OnEnter() {}
	virtual void OnExit() {}
	virtual void OnUpdate(const jc::TimeSpan& _dt) { (void)_dt; }
	virtual void OnRender() {}				// 수동 그리기 — 트래버설 후 최상위
	Window* GetWindow() const { return pWindow_; }

	////////////////////////////////////////////////////////////////////////////////////////
	// 렌더 진입점
	virtual void RenderScene() = 0;			// 파생이 RenderNode + OnRender + Flush 구성

	////////////////////////////////////////////////////////////////////////////////////////
	// 엔진 내부 (GameObject::AddChild가 자식 초기화에 사용)
	GraphicDevice* GetGraphicDevice() const { return pDevice_; }	// 소유하지 않음

protected:
	Scene();								// 직접 생성 금지 — root_.SetScene(this) + 카메라 초기화
	void CreateDefaultCamera(Camera* _pCamera);	// 파생 생성자에서 호출 (Camera2D/3D — 내부에서 dbg_new 소유)
	void RenderNode(GameObject& _node);			// 트리 순회 (파생 RenderScene이 호출)
	GameObject root_;							// 트리 루트 노드 (이름 "Root") — 파생이 RenderNode(root_) 호출

private:
	friend class Director;  friend class GameObject;
	void SetWindow(Window* _pWindow);			// Director만 소속 윈도우를 바꿀 수 있다
	void SetDevice(GraphicDevice* _pDevice);	// Director가 씬 시작 시 디바이스 주입

private:
	Camera* defaultCamera_ = nullptr;			// 소유 (Camera2D/3D — GameObject 파생)
	Camera* selectedCamera_ = nullptr;			// 기본 = defaultCamera_
	jc::CDataMap<> dataMap_;					// 범용 데이터 보관 (이름 키)
	Window* pWindow_ = nullptr;
	GraphicDevice* pDevice_ = nullptr;			// 소유하지 않음 (Director가 주입)
};

NS_SGF_END
