/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (게임 오브젝트 시스템)
 * =====================
 * 게임 오브젝트 구현부 — GID/트리/컴포넌트/초기화
 */

#include "sgf/Scene/GameObject.h"
#include "sgf/Scene/Scene.h"
#include "sgf/Scene/Transform.h"
#include "sgf/Graphics/Material.h"
#include "sgf/Graphics/Mesh.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "jc/Assert.h"
#include "jc/Debug/New.h"

NS_SGF_BEGIN

using namespace jc;

// ── GID: 생성 시 발급, 소멸 시 반환 (순수 증가 — 반납 키 재사용 없음) ──
IdProvider<_u64> GameObject::gidProvider_;

//////////////////////////////////////////////////////////////////////////////////////////
GameObject::GameObject(const jc::String& _name)
	: name_(_name)
	, gid_(gidProvider_.Acquire())			// 1, 2, 3, ... 계속 증가 (INVALID_KEY=0은 발급 안 됨)
	, pTransform_(dbg_new Transform(this))	// 기본 컴포넌트
	, pMaterial_(dbg_new Material())
{
}

//////////////////////////////////////////////////////////////////////////////////////////
GameObject::~GameObject()
{
	// GID 반환 — 횟수 감소만 (재할당 없음)
	if (gid_ != 0)
	{
		gidProvider_.Release(gid_);
		gid_ = 0;
	}

	// 자식 전부 정리 (소유권 이전 — 부모 소멸 시 일괄)
	RemoveAllChildren();

	// 그 외 컴포넌트 전부 정리
	for (Component* pComponent : components_)
	{
		JC_DELETE_SAFE(pComponent);
	}
	components_.Clear();

	// 기본 멤버 정리 (Transform/Material)
	JC_DELETE_SAFE(pMaterial_);
	JC_DELETE_SAFE(pTransform_);
}

//////////////////////////////////////////////////////////////////////////////////////////
// ★ 트리에 자식 추가 — 씬 트리의 골격을 만드는 핵심 함수.
// 하는 일: ① 부모 연결 ② 씬 소속 전파(자식 재귀) ③ zOrder 정렬 삽입 ④ OnEnter 1회 호출.
// - OnEnter에서 스태틱 bake(버텍스 1회 생성)가 일어나므로, 리소스 준비는 여기서 하면 된다.
// - (용어) zOrder: 그리기 순서. 작을수록 아래(먼저)에 그려진다.
void GameObject::AddChild(GameObject* _pChild, _u64 _zOrder)
{
	jc_assert_msg(_pChild != nullptr, "null 자식은 추가할 수 없습니다.");
	if (_pChild == nullptr) return;                                  // 오류 — 처리 거부

	jc_assert_msg(_pChild->pParent_ == nullptr, "이미 부모가 있는 객체");
	if (_pChild->pParent_ != nullptr) return;                       // 오류 — 처리 거부

	_pChild->pParent_ = this;
	_pChild->zOrder_  = _zOrder;
	_pChild->SetScene(pScene_);              // 씬 소속 전파 (자식 재귀)

	if (initialized_ && pScene_ != nullptr && pScene_->GetWindow() != nullptr)
		_pChild->Initialize(pScene_->GetGraphicDevice());   // 씬 초기화 이후 추가 → 즉시 GPU 초기화

	InsertChildSorted(_pChild, _zOrder);
	_pChild->OnEnter();                      // AddChild 직후 1회 — 스태틱 bake
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameObject::RemoveChild(GameObject* _pChild)
{
	jc_assert_msg(_pChild != nullptr, "null 자식은 제거할 수 없습니다.");
	if (_pChild == nullptr) return;

	_pChild->OnExit();                       // RemoveChild 시 1회 — (필요 시) 스태틱 정리
	for (int i = 0; i < children_.Size(); ++i)
	{
		if (children_[i].pObject_ == _pChild)
		{
			children_.RemoveAt(i);           // 분리만 (delete 안 함)
			break;
		}
	}
	_pChild->pParent_ = nullptr;
	_pChild->SetScene(nullptr);              // 씬 소속 해제 (자식 재귀)
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameObject::RemoveAllChildren()
{
	// 역순 제거 — RemoveChild가 리스트를 수정하므로 뒤에서부터
	while (children_.Size() > 0)
	{
		RemoveChild(children_.Last()->pObject_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
GameObject* GameObject::GetChildAt(int _index) const
{
	jc_assert_msg(_index >= 0 && _index < children_.Size(), "자식 인덱스 범위 초과");
	if (_index < 0 || _index >= children_.Size()) return nullptr;
	return children_[_index].pObject_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameObject::SetZOrder(_u64 _zOrder)
{
	if (pParent_ == nullptr || zOrder_ == _zOrder) return;
	zOrder_ = _zOrder;

	// 리스트에서 빼서 새 위치에 재삽입 (OnExit/OnEnter 없이 — SetZOrder는 수명주기 변경이 아님)
	jc::Vector<ChildEntry>& list = pParent_->children_;
	int from = 0;
	for (; from < list.Size(); ++from)
		if (list[from].pObject_ == this) break;
	if (from >= list.Size()) return;         // 자기 자신이 목록에 없음 (오류)

	ChildEntry entry = list[from];
	list.RemoveAt(from);

	int to = 0;
	for (; to < list.Size(); ++to)
		if (list[to].zOrder_ > _zOrder) break;   // 같은 z면 그룹 맨 뒤 (계약 3)
	list.Insert(to, entry);
}

//////////////////////////////////////////////////////////////////////////////////////////
GameObject* GameObject::FindChildByName(const jc::String& _name)
{
	if (name_ == _name) return this;

	for (int i = 0; i < children_.Size(); ++i)
	{
		GameObject* pFound = children_[i].pObject_->FindChildByName(_name);
		if (pFound != nullptr) return pFound;
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameObject::SetMesh(Mesh* _pMesh)
{
	pMesh_ = _pMesh;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 메시 스태틱 레벨 전환 — Static: 현재 월드 행렬을 고정 보관 (재호출 = 새 위치로 재고정)
// Dynamic: 고정 해제 → 매 프레임 현재 월드 행렬 사용.
// 정점은 어느 쪽이든 GPU(b1)로 변환된다 — 레벨은 "월드 행렬 출처"만 바꾼다.
void GameObject::SetMeshStaticLevel(StaticLevel _level)
{
	staticLevel_ = _level;
	if (_level == StaticLevel::slStatic)
	{
		staticWorld_ = pTransform_->GetWorldMatrix();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameObject::SetVisible(bool _visible)
{
	visible_ = _visible;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 메시 자동 드로우 — Mesh+Material만 설정하면 트래버설 중 자동으로 그려진다.
// (Scene::RenderNode → RenderSelf → pScene_->DrawMesh → Renderer2D::DrawMesh GPU 변환)
// ★ 왜 편리한가: AddChild + SetMesh만 하면 매 프레임 여기서 자동으로 그려진다. 그리기 코드가 필요 없다.
// - 월드 행렬은 Transform이 계산해 준다 (부모 변환 포함). slStatic이면 고정 행렬을 전달한다.
// - (용어) 월드 행렬: 오브젝트를 "게임 세계" 어디에 놓을지 알려주는 4x4 변환.
void GameObject::RenderSelf()
{
	if (pMesh_ == nullptr || pScene_ == nullptr)
	{
		return;
	}
	// slStatic: 고정 행렬 / 그 외: 현재 월드 행렬 (정점은 불변 — 변환은 GPU가 수행)
	const mat4& world = (staticLevel_ == StaticLevel::slStatic)
		? staticWorld_
		: pTransform_->GetWorldMatrix();
	pScene_->DrawMesh(pMesh_, pMaterial_, world);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameObject::Initialize(GraphicDevice* _pDevice)
{
	// 기본 Material GPU 초기화 (이미 초기화됐으면 스킵 — Material 내부에서도 재초기화 방지)
	if (pMaterial_ != nullptr && !initialized_ && _pDevice != nullptr)
	{
		pMaterial_->Initialize(_pDevice);
	}

	initialized_ = true;

	// 자식 재귀
	for (int i = 0; i < children_.Size(); ++i)
	{
		children_[i].pObject_->Initialize(_pDevice);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameObject::Update(const jc::TimeSpan& _dt)
{
	OnUpdate(_dt);

	for (int i = 0; i < children_.Size(); ++i)
	{
		children_[i].pObject_->Update(_dt);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameObject::SetScene(Scene* _pScene)
{
	pScene_ = _pScene;

	// 자식 재귀 — 씬 소속 전파
	for (int i = 0; i < children_.Size(); ++i)
	{
		children_[i].pObject_->SetScene(_pScene);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameObject::InsertChildSorted(GameObject* _pChild, _u64 _zOrder)
{
	// 같은 zOrder는 계속 지나침 → "나중에 추가된 것이 리스트 뒤(위)" — 안정
	int index = 0;
	for (; index < children_.Size(); ++index)
	{
		if (children_[index].zOrder_ > _zOrder)   // '>' → 같은 z는 뒤로 밀리지 않음
			break;
	}
	children_.Insert(index, ChildEntry{ _pChild, _zOrder });
}

NS_SGF_END
