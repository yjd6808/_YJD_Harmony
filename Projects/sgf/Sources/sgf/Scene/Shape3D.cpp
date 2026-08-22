/*
 * 작성자: 윤정도
 * 생성일: 8/17/2026 (프리미티브 자동 드로우 — Shape2D의 3D 버전)
 * =====================
 * Shape3D 구현부 — OnEnter에서 메시+머티리얼 1회 구성, OnRender에서 메시 자동 드로우
 */

#include "Core.h"
#include "sgf/Scene/Shape3D.h"
#include "sgf/Scene/Scene.h"	// pScene_->GetGraphicDevice() (전체 정의)
#include "sgf/Graphics/Mesh.h"
#include "sgf/Graphics/Material.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "sgf/Graphics/GraphicDevice.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
Shape3D::Shape3D(const jc::String& _name)
	: GameObject(_name)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void Shape3D::SetShape(PrimitiveMesh3DType _type, const color& _color)
{
	type_ = _type;
	color_ = _color;
}

//////////////////////////////////////////////////////////////////////////////////////////
// AddChild 직후 1회 — 프리미티브 메시를 꺼내 셰이더/색을 갖춘 머티리얼과 함께 장착한다.
// - 메시는 ResourceMgr 디폴트 리소스(빌림) — Release 없음.
// - 머티리얼은 베이스 GameObject가 소유하며, Renderer3D가 셰이더 키를 해석해 그린다.
void Shape3D::OnEnter()
{
	if (pScene_ == nullptr)
	{
		return;
	}

	Mesh* pMesh = g_cResourceMgr.FindPrimitiveMesh3D(type_);
	if (pMesh == nullptr)
	{
		return;
	}

	SetMesh(pMesh);

	// 3D 렌더러(Renderer3D)는 재질의 셰이더 키를 해석해 파이프라인을 구성한다.
	// GameObject 기본 재질에는 키가 없으므로 디폴트 3D 셰이더를 지정해준다.
	Material* pMaterial = GetMaterial();
	pMaterial->SetVertexShaderKey(g_cResourceMgr.GetDefaultVertexShader3DKey());
	pMaterial->SetPixelShaderKey(g_cResourceMgr.GetDefaultPixelShader3DKey());
	pMaterial->SetBaseColor(color_);
	pMaterial->Initialize(pScene_->GetGraphicDevice());	// GPU 상태(파이프라인/상수버퍼) 초기화
}

//////////////////////////////////////////////////////////////////////////////////////////
// 매 프레임 — 메시 자동 드로우. Transform의 위치/회전/스케일이 월드 행렬로 반영된다.
void Shape3D::OnRender()
{
	RenderSelf();
}

NS_SGF_END