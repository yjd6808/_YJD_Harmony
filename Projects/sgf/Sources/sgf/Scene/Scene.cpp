/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (Scene 추상화 + root_ 트리 + 카메라 GameObject 파생)
 * =====================
 * Scene 구현부 — 트리 루트 + 카메라 운용 + 트래버설
 */

#include "Core.h"
#include "sgf/Scene/Scene.h"
#include "sgf/Scene/Director.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
Scene::Scene()
{
	root_.SetName("Root");
	root_.SetScene(this);	// root_가 씬 소속이 되어야 AddChild의 씬 전파·OnEnter가 동작한다.
}

//////////////////////////////////////////////////////////////////////////////////////////
Scene::~Scene()
{
	// defaultCamera_ 정리 (GameObject 파생 — 소유)
	// (root_ 트리의 자식이면 RemoveChild로 분리된 뒤 delete)
	if (defaultCamera_ != nullptr)
	{
		root_.RemoveChild(defaultCamera_);	// 트리에 있으면 분리 (없으면 no-op)
		JC_DELETE_SAFE(defaultCamera_);
	}

	// root_ 서브트리 전부 정리 (소멸자가 재귀 정리)
	root_.RemoveAllChildren();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scene::CreateDefaultCamera(Camera* _pCamera)
{
	jc_assert_msg(_pCamera != nullptr, "기본 카메라가 null입니다.");
	if (_pCamera == nullptr) return;

	defaultCamera_ = _pCamera;
	selectedCamera_ = _pCamera;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scene::SelectCamera(GameObject* _pCameraObj)
{
	// 카메라인 GameObject만 전환 가능하다. (Camera*로 캐스팅 성공 시)
	Camera* pCamera = dynamic_cast<Camera*>(_pCameraObj);
	jc_assert_msg(pCamera != nullptr, "카메라가 아닌 GameObject를 카메라로 선택할 수 없습니다.");
	if (pCamera != nullptr)
	{
		selectedCamera_ = pCamera;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scene::SelectDefaultCamera()
{
	selectedCamera_ = defaultCamera_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scene::AddChild(GameObject* _pChild, _u64 _zOrder)
{
	// running 씬 검증 — AddChild는 running 중인 씬에서만 가능하다.
	Scene* pRunning = g_cDirector.GetRunningScene();
	jc_assert_msg(pRunning != nullptr, "현재 running 중인 씬이 없습니다. AddChild는 running 씬에서만 가능합니다.");
	if (pRunning == nullptr) return;					// 오류 — 처리 거부

	jc_assert_msg(pRunning == this, "이 씬이 running 중이 아닙니다. AddChild 거부.");
	if (pRunning != this) return;						// 오류 — 처리 거부

	root_.AddChild(_pChild, _zOrder);					// → pScene_ 주입 + Initialize + OnEnter
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scene::RemoveChild(GameObject* _pChild)
{
	root_.RemoveChild(_pChild);							// → OnExit + 씬 소속 해제
}

//////////////////////////////////////////////////////////////////////////////////////////
GameObject* Scene::FindGameObjectByName(const char* _pName)
{
	return root_.FindChildByName(_pName);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Scene::GetGameObjectCount() const
{
	// root_ 직속 + 서브트리 재귀 카운트
	return root_.GetChildCount();	// 1차 (서브트리 포함 카운트가 필요하면 확장)
}

//////////////////////////////////////////////////////////////////////////////////////////
// ★ 트리 순회 (DFS) — 모든 게임 오브젝트를 "그리기 순서"대로 방문한다.
// - 자식 리스트는 이미 zOrder 오름차순(안정) 정렬 상태 — 추가 정렬 없이 순회만 한다.
// - 방문 순서 = 그리기 순서. 부모가 자식보다 먼저 그려진다.
// - (용어) DFS: 한 자식의 서브트리를 끝까지 내려간 뒤 다음 형제로 이동하는 탐색 방식.
void Scene::RenderNode(GameObject& _node)
{
	// 자식 리스트는 이미 zOrder 오름차순(안정) 정렬 상태 — 추가 정렬 없이 순회만.
	for (int i = 0; i < _node.GetChildCount(); ++i)
	{
		GameObject* pChild = _node.GetChildAt(i);
		if (!pChild->IsVisible()) continue;

		pChild->RenderSelf();				// 메시 자동 드로우 (SetMesh 노드 — Mesh+Material)
		pChild->OnRender();					// 무인자 — 내부에서 pScene_->RenderStatic/RenderDynamic/DrawMesh
		RenderNode(*pChild);				// 자식 서브트리 (DFS)
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scene::SetWindow(Window* _pWindow)
{
	pWindow_ = _pWindow;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scene::SetDevice(GraphicDevice* _pDevice)
{
	pDevice_ = _pDevice;
}

NS_SGF_END