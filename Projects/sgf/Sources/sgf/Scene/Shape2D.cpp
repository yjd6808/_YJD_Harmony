/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (프리미티브 자동 드로우)
 * =====================
 * Shape2D 구현부 — OnEnter에서 DeclareStaticXX 1회 bake, OnRender에서 RenderStatic 예약
 */

#include "Core.h"
#include "sgf/Scene/Shape2D.h"
#include "sgf/Scene/Scene2D.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
Shape2D::Shape2D(const char* _pName)
	: GameObject(_pName)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void Shape2D::SetRect(const rect& _region, const color& _color)
{
	kind_ = ShapeKind::Rect;
	rect_ = _region;
	color_ = _color;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Shape2D::SetCircle(const vec2& _center, _f32 _radius, const color& _color, _u32 _segments)
{
	kind_ = ShapeKind::Circle;
	circleCenter_ = _center;
	circleRadius_ = _radius;
	circleSegments_ = _segments;
	color_ = _color;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Shape2D::SetLine(const vec2& _p1, const vec2& _p2, _f32 _thickness, const color& _color)
{
	kind_ = ShapeKind::Line;
	lineP1_ = _p1;
	lineP2_ = _p2;
	lineThickness_ = _thickness;
	color_ = _color;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Shape2D::SetTriangle(const vec2& _p1, const vec2& _p2, const vec2& _p3, const color& _color)
{
	kind_ = ShapeKind::Triangle;
	triP1_ = _p1;
	triP2_ = _p2;
	triP3_ = _p3;
	color_ = _color;
}

//////////////////////////////////////////////////////////////////////////////////////////
// AddChild 직후 1회 — 설정된 도형을 절대좌표로 bake하고 staticId_를 받아둔다.
void Shape2D::OnEnter()
{
	Scene2D* pScene2D = dynamic_cast<Scene2D*>(pScene_);
	if (pScene2D == nullptr)
	{
		staticId_ = 0;
		return;
	}

	switch (kind_)
	{
	case ShapeKind::Rect:
		staticId_ = pScene2D->DeclareStaticRect(rect_, color_);
		break;
	case ShapeKind::Circle:
		staticId_ = pScene2D->DeclareStaticCircle(circleCenter_, circleRadius_, color_, circleSegments_);
		break;
	case ShapeKind::Line:
		staticId_ = pScene2D->DeclareStaticLine(lineP1_, lineP2_, lineThickness_, color_);
		break;
	case ShapeKind::Triangle:
		staticId_ = pScene2D->DeclareStaticTriangle(triP1_, triP2_, triP3_, color_);
		break;
	default:
		staticId_ = 0;   // 미설정 — 그리지 않음
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 매 프레임 — bake된 스태틱 캐시를 예약한다. (버텍스 재계산 없음, 배칭에 참여)
void Shape2D::OnRender()
{
	if (staticId_ != 0 && pScene_ != nullptr)
	{
		pScene_->RenderStatic(staticId_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// RemoveChild 직후 1회 — id만 무효화. (staticCache는 순수 증가 — 반납 API 없음)
void Shape2D::OnExit()
{
	staticId_ = 0;
}

NS_SGF_END