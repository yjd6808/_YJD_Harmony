/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (게임 오브젝트 시스템)
 * =====================
 * Scene2D — 2D 렌더링 씬 (예약 창구 구현 + 트래버설)
 *
 * [역할]
 * 트리(root_)를 순회하며 노드의 OnRender()가 예약 창구(RenderStatic/RenderDynamic)로
 * 그리기를 예약하면, RenderScene 끝에서 Renderer2D::FlushStatic/FlushDynamic이 한꺼번에 그린다.
 * 기본 카메라는 Camera2D (직교 — 픽셀 단위 좌표).
 *
 * [스태틱 선언]
 * 노드의 OnEnter에서 DeclareStaticXX로 1회 bake → staticId 반환 → OnRender에서 RenderStatic(id) 예약.
 * 선언 시 파라미터(영역/색/채움)가 고정된다. (변하는 것은 RenderDynamic)
 *
 * [드로우 순서]
 * RenderNode(root_) = 트리 순회 순서 (부모 먼저 → 자식 zOrder 오름차순) = 드로우 순서.
 * RenderLayer::Bottom/Default/Top 버킷은 트리와 무관한 씬 전역 최상단/최하단.
 */

#pragma once

#include "sgf/Scene/Scene.h"
#include "sgf/Scene/Camera2D.h"
#include "sgf/Graphics/Fill.h"

NS_SGF_BEGIN

using namespace jc;

class Renderer2D;
class Texture;

class Scene2D : public Scene
{
public:
	virtual ~Scene2D();

	Camera2D* GetCamera2D() const;			// = static_cast<Camera2D*>(GetCamera())

	////////////////////////////////////////////////////////////////////////////////////////
	// 예약 창구 구현 (— Renderer2D로 전달)
	virtual void RenderStatic(_u64 _staticId) override;	// 선언 시 고정 — id만 예약 (버킷은 선언 시 고정)
	virtual void RenderDynamic(const rect& _region, const Fill& _fill,
		const color& _color1, const color& _color2, _u32 _option,
		RenderLayer _layer = RenderLayer::Default) override;		// _layer: Bottom/Default/Top — 씬 전역 순서

	////////////////////////////////////////////////////////////////////////////////////////
	// 스태틱 선언 (— OnEnter에서 1회 bake, 반환 staticId)
	// 2D 도형은 이 6종이 전부 — 어차피 2D에서는 이것들만 있다. (9-패치/게이지 등은 Fill 범용 선언 사용)
	// 6종은 기본 레이어(Default) 전용 — 최상단/최하단 bake는 범용 DeclareStatic(Fill, ..., layer) 사용
	_u64 DeclareStaticImage(Texture* _pTexture, const rect& _region);                          // 텍스처 쿼드
	_u64 DeclareStaticImage(Texture* _pTexture, const rect& _region,                           // 9-패치 (니스슬라이스)
		_f32 _sliceLeft, _f32 _sliceTop, _f32 _sliceRight, _f32 _sliceBottom);
	_u64 DeclareStaticLine(const vec2& _p1, const vec2& _p2, _f32 _thickness, const color& _color);
	_u64 DeclareStaticCircle(const vec2& _center, _f32 _radius, const color& _color, _u32 _segments = 32);
	_u64 DeclareStaticRect(const rect& _region, const color& _color);
	_u64 DeclareStaticPolygon(const vec2* _pPoints, _u32 _count, const color& _color);
	_u64 DeclareStaticTriangle(const vec2& _p1, const vec2& _p2, const vec2& _p3, const color& _color);

	////////////////////////////////////////////////////////////////////////////////////////
	// 스태틱 범용 선언 (— 6종 밖의 Fill bake: 게이지, 커스텀 등)
	// 예: DeclareStatic(Fill::Gauge(0.7f), rect, 배경색, 채움색) — Bake 시점에 FillGauge가 적용됨
	_u64 DeclareStatic(const Fill& _fill, const rect& _region,
		const color& _color1 = color::WHITE, const color& _color2 = color::WHITE,
		_u32 _option = 0, RenderLayer _layer = RenderLayer::Default);

	////////////////////////////////////////////////////////////////////////////////////////
	// 메시 그리기 (— Mesh+Material 자동 드로우)
	// GameObject::RenderSelf → Scene2D::DrawMesh → Renderer2D::DrawMesh (2D 배칭)
	virtual void DrawMesh(Mesh* _pMesh, Material* _pMaterial, const mat4& _world) override;

	virtual void RenderScene() override;

protected:
	Scene2D();	// defaultCamera_ = Camera2D::CreateDefault()
};

NS_SGF_END
