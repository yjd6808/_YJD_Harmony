/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (게임 오브젝트 시스템)
 * =====================
 * Scene3D — 3D 렌더링 씬 (DrawMesh 창구 + RenderScene)
 *
 * [역할]
 * 트리(root_)를 순회하며 노드의 OnRender()가 DrawMesh로 그리기를 요청하면,
 * Renderer3D가 즉시 그린다 (기존 SceneRenderer 파이프라인 흡수).
 * 기본 카메라는 Camera3D (원근 투영).
 *
 * [RenderScene]
 * 카메라 자동 구성(1회) → Renderer3D::BeginScene(frame) → 트래버설 + OnRender → EndScene.
 * FrameConstants(뷰/투영/카메라 위치)는 Renderer3D::BeginScene으로 넘어간다. (6-6)
 */

#pragma once

#include "sgf/Scene/Scene.h"
#include "sgf/Scene/Camera3D.h"
#include "sgf/Graphics/Renderer3D.h"	// FrameConstants / RenderObject (SceneRenderer에서 이동)

NS_SGF_BEGIN

using namespace jc;

class Mesh;
class Material;

class Scene3D : public Scene
{
public:
	virtual ~Scene3D();

	Camera3D* GetCamera3D() const;				// = static_cast<Camera3D*>(GetCamera())

	////////////////////////////////////////////////////////////////////////////////////////
	// 3D 즉시 그리기 창구 (— Renderer3D로 전달)
	virtual void DrawMesh(Mesh* _pMesh, Material* _pMaterial, const mat4& _world) override;
	virtual void DrawMesh(const RenderObject& _object);

	virtual void RenderScene() override;

protected:
	Scene3D();	// defaultCamera_ = Camera3D::CreateDefault()
};

NS_SGF_END
