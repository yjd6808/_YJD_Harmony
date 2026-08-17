/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (게임 오브젝트 시스템)
 * =====================
 * Scene3D 구현부 — DrawMesh 창구 + RenderScene (Renderer3D 파이프라인)
 */

#include "Core.h"
#include "sgf/Scene/Scene3D.h"
#include "sgf/Core/Application.h"	// g_cRenderer3D 매크로

NS_SGF_BEGIN

using namespace jc;

// 로그 주기 제한용 프레임 카운터 (60프레임마다 1회만 출력 — A-3)
static _u32 sLogFrame = 0;

//////////////////////////////////////////////////////////////////////////////////////////
Scene3D::Scene3D()
{
	CreateDefaultCamera(Camera3D::CreateDefault());	// 기본 카메라 = 원근 3D (Camera3D)
}

//////////////////////////////////////////////////////////////////////////////////////////
Scene3D::~Scene3D()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Camera3D* Scene3D::GetCamera3D() const
{
	return static_cast<Camera3D*>(GetCamera());
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scene3D::DrawMesh(Mesh* _pMesh, Material* _pMaterial, const mat4& _world)
{
	g_cRenderer3D.Draw(_pMesh, _pMaterial, _world);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scene3D::DrawMesh(const RenderObject& _object)
{
	g_cRenderer3D.Draw(_object);
}

//////////////////////////////////////////////////////////////////////////////////////////
// ★ 3D 한 프레임의 총괄 — 카메라 상수(b0) → 트리 순회 → 수동 그리기 → 배치 종료.
// - FrameConstants(뷰/투영/카메라 위치)는 카메라가 만들고, Renderer3D::BeginScene이 b0에 장착한다.
// - 카메라가 아직 구성되지 않았다면 기본 원근 카메라(60도, 눈(0,3,-8) → 원점)로 자동 구성한다.
void Scene3D::RenderScene()
{
	if (!GetCamera3D()->IsConfigured())
	{
		GetCamera3D()->SetPerspectiveDegrees(60.0f, GetWindow()->AspectRatio(), 0.1f, 1000.0f);
		GetCamera3D()->SetLookAt(vec3(0.0f, 3.0f, -8.0f), vec3::Zero());
	}

	// 프레임 공통 상수 (기존 SceneRenderer::BeginScene 역할 — Renderer3D가 흡수)
	FrameConstants frame;
	frame.view_          = GetCamera()->View();
	frame.projection_    = GetCamera()->Projection();
	frame.cameraPosition_ = vec4(GetCamera3D()->Eye(), 1.0f);
	g_cRenderer3D.BeginScene(frame);           // b0 갱신
	if ((++sLogFrame % 60) == 1)
		_LogDebug_("[sgf] Scene3D::RenderScene Begin (window=%p)", (void*)GetWindow());

	RenderNode(root_);                         // 노드 OnRender() → pScene_ 경유 DrawMesh
	OnRender();                                // 수동 3D/2D 혼합 (씬 자신 — 최상위)

	g_cRenderer3D.EndScene();
	if ((sLogFrame % 60) == 1)
		_LogDebug_("[sgf] Scene3D::RenderScene End");
}

NS_SGF_END