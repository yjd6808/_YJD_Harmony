/*
 * 작성자: 윤정도
 * 생성일: 8/17/2026 (프리미티브 자동 드로우 — Shape2D의 3D 버전)
 * =====================
 * Shape3D — 3D 프리미티브 편의 노드 (메시/머티리얼 자동 구성)
 *
 * [사용법]
 * Shape3D* pShape = dbg_new Shape3D("큐브");
 * pShape->SetShape(PrimitiveMesh3DType::Cube, color::RED);
 * pShape->SetPosition(vec3(0.0f, 1.0f, 0.0f));
 * pScene->AddChild(pShape, 0); // AddChild 직후 OnEnter가 메시+머티리얼 1회 구성 → 이후 자동 드로우
 *
 * [특징]
 * - OnEnter에서 ResourceMgr의 3D 프리미티브 메시를 조회해 SetMesh하고,
 *   머티리얼에 디폴트 3D 셰이더 키 + baseColor를 지정해 GPU 초기화까지 마친다.
 * - OnRender에서 RenderSelf() 호출 — 메시 자동 드로우 경로 (Scene3D → Renderer3D 배칭)
 * - Shape2D와 달리 절대좌표 bake가 없다: 위치/크기/회전은 Transform으로 자유롭게 바꾼다.
 *   (3D는 정적인 UI가 아니라 움직이는 오브젝트가 주 용도 — 메시 자동 드로우가 정석)
 * - 메시는 ResourceMgr 소유(빌림) — OnExit에서 반납할 것 없음. 머티리얼은 베이스가 소유.
 */

#pragma once

#include "sgf/Scene/GameObject.h"
#include "sgf/Graphics/PrimitiveMeshType.h"

NS_SGF_BEGIN

using namespace jc;

class Shape3D : public GameObject
{
public:
	explicit Shape3D(const jc::String& _name = "Shape3D");

	// AddChild 전에 설정 (OnEnter에서 1회 구성 — 이후 변경은 무시)
	void SetShape(PrimitiveMesh3DType _type, const color& _color);

	////////////////////////////////////////////////////////////////////////////////////////
	// Transform 편의 래퍼 (— 한 줄 배치용, 내부는 GetTransform() 위임)
	void SetPosition(const vec3& _position) { GetTransform()->SetLocalPosition(_position); }
	void SetScale(const vec3& _scale)       { GetTransform()->SetLocalScale(_scale); }
	void SetScale(_f32 _uniform)            { GetTransform()->SetLocalScale(_uniform); }
	void SetRotationEuler(_f32 _xDeg, _f32 _yDeg, _f32 _zDeg) { GetTransform()->SetLocalRotationEuler(_xDeg, _yDeg, _zDeg); }

protected:
	virtual void OnEnter() override;	// 메시 조회 + 셰이더 키 + baseColor + Material::Initialize 1회
	virtual void OnRender() override;	// RenderSelf() — 메시 자동 드로우 (움직임은 Transform이 반영)

private:
	PrimitiveMesh3DType type_ = PrimitiveMesh3DType::Cube;	// 기본 큐브
	color  color_ = color::WHITE;					// 채움색 (baseColor)
};

NS_SGF_END
