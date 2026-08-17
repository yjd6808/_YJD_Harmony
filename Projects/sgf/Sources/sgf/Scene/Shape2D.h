/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (프리미티브 자동 드로우)
 * =====================
 * Shape2D — 2D 프리미티브 편의 노드 (DeclareStatic/RenderStatic 자동 수행)
 *
 * [사용법]
 * Shape2D* pShape = dbg_new Shape2D("패널");
 * pShape->SetRect(rect(10, 10, 100, 50), color::WHITE);
 * pScene->AddChild(pShape, 0); // AddChild 직후 OnEnter가 1회 bake → 이후 자동 드로우
 *
 * [특징]
 * - 완전 정적: 절대좌표로 1회 bake — 움직이지 않는 UI(테두리/패널/HP바 배경)에 적합
 * - 움직이는 도형은 메시 자동 드로우 경로 (GameObject::SetMesh + Mesh 2D 팩토리) 사용
 * - 드로콜: DeclareStatic 캐시(버텍스 1회 bake) + FlushStatic 텍스처 배칭
 * - OnExit에서 id만 무효화 (staticCache는 순수 증가 — 반납 API 없음)
 */

#pragma once

#include "sgf/Scene/GameObject.h"

NS_SGF_BEGIN

using namespace jc;

class Scene2D;

class Shape2D : public GameObject
{
public:
	explicit Shape2D(const char* _pName = "Shape2D");

	// AddChild 전에 설정 (OnEnter에서 1회 bake — 이후 변경은 무시)
	void SetRect(const rect& _region, const color& _color);
	void SetCircle(const vec2& _center, _f32 _radius, const color& _color, _u32 _segments = 32);
	void SetLine(const vec2& _p1, const vec2& _p2, _f32 _thickness, const color& _color);
	void SetTriangle(const vec2& _p1, const vec2& _p2, const vec2& _p3, const color& _color);

protected:
	virtual void OnEnter() override;   // pScene_->DeclareStaticXX(...) 1회 bake → staticId_
	virtual void OnRender() override;  // pScene_->RenderStatic(staticId_) 예약
	virtual void OnExit() override;    // id만 무효화 — staticCache는 순수 증가 유지

private:
	enum class ShapeKind { None, Rect, Circle, Line, Triangle };

	ShapeKind kind_ = ShapeKind::None;   // AddChild 전에 SetXXX로 결정
	rect   rect_;                        // Rect
	color  color_ = color::WHITE;        // 공통 채움색/틴트
	vec2   circleCenter_;                // Circle
	_f32   circleRadius_ = 1.0f;         // Circle
	_u32   circleSegments_ = 32;         // Circle
	vec2   lineP1_, lineP2_;             // Line
	_f32   lineThickness_ = 1.0f;        // Line
	vec2   triP1_, triP2_, triP3_;       // Triangle

	_u64 staticId_ = 0;                  // 선언된 스태틱 id (0 = 미선언)
};

NS_SGF_END