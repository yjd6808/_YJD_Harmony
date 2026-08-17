/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (게임 오브젝트 시스템)
 * =====================
 * 게임 오브젝트 — Cocos2d-x 스타일 트리 노드 + 컴포넌트 하이브리드
 *
 * [GameObject란?]
 * 씬 트리를 구성하는 노드. 이름/Transform/Material/GID/DataMap을 기본으로 갖고,
 * AddChild로 자식을 둠으로써 "어디에 어떻게 그려질지"를 트리로 표현한다.
 * 사용자는 GameObject를 상속해 OnEnter/OnRender/OnUpdate를 재정의한다.
 *
 * [트리 규약] (핵심)
 * - AddChild(child, zOrder) — 소유권이 부모로 넘어간다. (자식 삭제는 부모 소멸 시 일괄)
 * - zOrder는 _u64 정수, 클수록 위. 같으면 나중에 추가된 것이 위. (Cocos 동작)
 * - 자식 리스트는 삽입 시점에 정렬 유지 → 드로우 시 추가 정렬 비용 0.
 * - 드로우 순서 = 트리 순회 순서 (부모 먼저 → 자식 zOrder 오름차순).
 *
 * [수명주기 훅]
 * - OnEnter: AddChild 직후 1회 (씬 소속 확정) — 스태틱 bake(DeclareStatic*)는 여기서
 * - OnExit: RemoveChild 직후 1회 — (필요 시) 스태틱 정리
 * - OnRender: 매 프레임 트래버설 — pScene_->RenderStatic/RenderDynamic/DrawMesh 호출
 * - OnUpdate: 매 프레임 — 로직 갱신
 */

#pragma once

#include "jc/Math.h"
#include "jc/Time.h"
#include "jc/Container/Vector.h"
#include "jc/Container/DataMap.h"
#include "jc/Primitives/String.h"
#include "jc/Pool/IdProvider.h"
#include "sgf/Scene/Component.h"
#include "sgf/Scene/Transform.h"

NS_SGF_BEGIN

using namespace jc;

class Scene;
class Mesh;
class Material;
class GraphicDevice;

class GameObject
{
public:
	explicit GameObject(const char* _pName = "");
	virtual ~GameObject();						// GID 반환 + 컴포넌트 + 자식 전부 정리

	// === 고유 ID (— 생성 시 발급, 소멸 시 반환, 순수 증가) ===
	_u64 GetGID() const { return gid_; }		// 재사용 없음 (IdProvider) — dangling 감지용

	// === 트리 (핵심) ===
	void AddChild(GameObject* _pChild, _u64 _zOrder);	// 소유권 이전 + zOrder 정렬 삽입
	void RemoveChild(GameObject* _pChild);				// 분리 (delete 안 함)
	void RemoveAllChildren();
	GameObject* GetParent() const { return pParent_; }
	int GetChildCount() const { return children_.Size(); }
	GameObject* GetChildAt(int _index) const;			// zOrder 오름차순, 동일 z는 삽입순
	_u64 GetZOrder() const { return zOrder_; }
	void SetZOrder(_u64 _zOrder);						// 부모 리스트에서 재배치 (동일 z면 그룹 맨 뒤)
	GameObject* FindChildByName(const char* _pName);	// 서브트리 재귀 탐색

	// === 렌더 훅 (— 무인자, pScene_ 멤버로 씬 예약 창구 호출) ===
	// 호출 순서 = 그리기 순서. (Scene::RenderNode가 트래버설하며 호출)
	virtual void OnRender() {}

	// === 메시 자동 드로우 (— Scene::RenderNode가 트래버설 중 호출) ===
	// SetMesh()만 하면 기본 Material(흰색)로 그려진다. (단색 도형 = Material baseColor로 채움색 지정)
	void RenderSelf();

	// === 수명주기 훅 (— 씬 소속 확정/해제 시 1회) ===
	virtual void OnEnter() {}		// AddChild 직후 — DeclareStaticXX로 스태틱 bake
	virtual void OnExit() {}		// RemoveChild 직후 — (필요 시) 스태틱 정리

	// === 갱신 훅 ===
	virtual void OnUpdate(const jc::TimeSpan& _dt) {}

	// === 컴포넌트 (그대로 — 하이브리드 보관, 카메라는 컴포넌트 아님) ===
	template <typename T> T* GetComponent();	// Transform/Material = O(1) 멤버, 그 외 = 선형 스캔
	template <typename T> T* AddComponent();	// 그 외 컴포넌트 생성+추가 (소유)
	Transform* GetTransform() const { return pTransform_; }
	Material*  GetMaterial()  const { return pMaterial_; }

	// === 메시 / 표시 ===
	void SetMesh(Mesh* _pMesh);  Mesh* GetMesh() const { return pMesh_; }	// 빌림 (ResourceMgr 소유)
	void SetVisible(bool _visible);  bool IsVisible() const { return visible_; }

	// === 이름 ===
	void SetName(const jc::String& _name) { name_ = _name; }
	const jc::String& GetName() const { return name_; }

	// === 데이터 보관 (— 모든 GameObject가 보유) ===
	jc::CDataMap<>& GetDataMap() { return dataMap_; }	// String 키 → 다양한 타입 값

	// === 엔진 내부 (Scene/Transform이 호출) ===
	void Initialize(GraphicDevice* _pDevice);	// 기본 Material GPU 초기화, 자식 재귀
	void Update(const jc::TimeSpan& _dt);		// OnUpdate + 컴포넌트 OnUpdate + 자식 재귀
	void PropagateTransformDirty();				// Transform::SetDirty가 자식들에게 전파

private:
	friend class Scene;
	friend class Transform;
	void SetScene(Scene* _pScene);					// 소속 씬 (자식 재귀)
	void InsertChildSorted(GameObject* _pChild, _u64 _zOrder);	// 정렬 안정 삽입
	void PropagateTransformDirtyRecursive();		// 이 노드 아래 자식 Transform 전부 worldDirty

protected:
	Scene* pScene_ = nullptr;						// 소속 씬 (OnRender()에서 사용, AddChild 시 주입)

private:
	jc::String name_;
	_u64 gid_ = 0;									// 고유 ID (생성자에서 발급, 소멸자에서 반환)
	jc::CDataMap<> dataMap_;						// 범용 데이터 보관 (이름 키)
	Transform* pTransform_ = nullptr;				// 기본 멤버 (생성 시 new)
	Material*  pMaterial_  = nullptr;				// 기본 멤버 (생성 시 new)
	jc::Vector<Component*> components_;				// 그 외 컴포넌트 (소유)

	GameObject* pParent_ = nullptr;
	struct ChildEntry { GameObject* pObject_; _u64 zOrder_; };
	jc::Vector<ChildEntry> children_;				// zOrder 오름차순 유지 (안정)

	Mesh* pMesh_ = nullptr;							// 빌림
	_u64 zOrder_ = 0;								// 부모 리스트 기준 (자기 자신의 깊이)
	bool visible_ = true;
	bool initialized_ = false;

	static IdProvider<_u64> gidProvider_;			// 전역 GID 발급기 (freeList 없음 — 순수 증가)
};

//////////////////////////////////////////////////////////////////////////////////////////
// 템플릿 구현 — GameObject.cpp와 분리 (헤더 인클루드 시점에 인스턴스화)
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
T* GameObject::GetComponent()
{
	// 기본 멤버 (O(1))
	if constexpr (std::is_same_v<T, Transform>) return pTransform_;
	if constexpr (std::is_same_v<T, Material>)  return pMaterial_;

	// 그 외: components_ 선형 스캔 (커스텀 컴포넌트)
	for (Component* pComponent : components_)
	{
		if (pComponent->GetComponentType() == T::COMPONENT_TYPE)
		{
			return static_cast<T*>(pComponent);
		}
	}
	return nullptr;
}

template <typename T>
T* GameObject::AddComponent()
{
	// 기본 멤버는 이미 존재하므로 그대로 반환
	if constexpr (std::is_same_v<T, Transform>) return pTransform_;
	if constexpr (std::is_same_v<T, Material>)  return pMaterial_;

	T* pComponent = dbg_new T(this);
	components_.PushBack(pComponent);
	return pComponent;
}

NS_SGF_END