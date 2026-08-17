/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (프리미티브 자동 드로우 시스템)
 * =====================
 * 2D 프리미티브 공용 기하 빌더 구현부
 *
 * [내부 규약]
 * - 정점 감기(winding)와 좌표계는 Renderer2D 기존 6종(Line/Circle/Polygon/Triangle)과 동일.
 * - 정점 인덱스는 FillResult 인덱스(_u16) 규약을 따른다. (65535 정점 미만 전제)
 */

#include "Core.h"
#include "sgf/Graphics/PrimitiveBuilder.h"

#include <cmath>

NS_SGF_BEGIN

using namespace jc;

namespace PrimitiveBuilder
{
	//////////////////////////////////////////////////////////////////////////////////////
	// 공용 쿼드 — 네 꼭짓점(좌하단 시작) + UV + 색. Fill::PushQuad2D와 동일 정점/인덱스 순서. (A-7)
	void PushQuad(FillResult& _out, _f32 _x1, _f32 _y1, _f32 _x2, _f32 _y2,
		const vec2& _uvMin, const vec2& _uvMax, const color& _color)
	{
		const _s32 base = _out.vertices_.Size();
		_out.vertices_.PushBack(VertexPTC{ vec3(_x1, _y1, 0.0f), vec2(_uvMin.x, _uvMin.y), _color });
		_out.vertices_.PushBack(VertexPTC{ vec3(_x2, _y1, 0.0f), vec2(_uvMax.x, _uvMin.y), _color });
		_out.vertices_.PushBack(VertexPTC{ vec3(_x2, _y2, 0.0f), vec2(_uvMax.x, _uvMax.y), _color });
		_out.vertices_.PushBack(VertexPTC{ vec3(_x1, _y2, 0.0f), vec2(_uvMin.x, _uvMax.y), _color });
		_out.indices_.PushBack((_u16)(base + 0)); _out.indices_.PushBack((_u16)(base + 1)); _out.indices_.PushBack((_u16)(base + 2));
		_out.indices_.PushBack((_u16)(base + 0)); _out.indices_.PushBack((_u16)(base + 2)); _out.indices_.PushBack((_u16)(base + 3));
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// 단색 사각형 — 영역 전체를 채운 쿼드 1개. (Fill::Solid 콜백과 동일 로직)
	void BuildRect(const rect& _region, const color& _color, FillResult& _out)
	{
		PushQuad(_out, _region.Left(), _region.Bottom(), _region.Right(), _region.Top(),
			vec2(0.0f, 0.0f), vec2(1.0f, 1.0f), _color);
		_out.pTexture_ = nullptr;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// 원 — 중심에서 부채꼴 삼각형들로 근사. (Renderer2D::DrawCircle과 동일 로직)
	void BuildCircle(const vec2& _center, _f32 _radius, const color& _color, _u32 _segments, FillResult& _out)
	{
		_s32 segments = (_s32)_segments;
		if (segments < 3) segments = 3;

		const _s32 base = _out.vertices_.Size();
		_out.vertices_.PushBack(VertexPTC{ vec3(_center.x, _center.y, 0.0f), vec2(0.0f, 0.0f), _color });
		for (_s32 i = 0; i <= segments; ++i)
		{
			const _f32 a = jc_math_pi2 * _f32(i) / _f32(segments);
			_out.vertices_.PushBack(VertexPTC{
				vec3(_center.x + cosf(a) * _radius, _center.y + sinf(a) * _radius, 0.0f),
				vec2(0.0f, 0.0f), _color
			});
		}
		for (_s32 i = 0; i < segments; ++i)
		{
			_out.indices_.PushBack((_u16)(base + 0));
			_out.indices_.PushBack((_u16)(base + 1 + i));
			_out.indices_.PushBack((_u16)(base + 2 + i));
		}
		_out.pTexture_ = nullptr;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// 두께 있는 선분 — 두 점을 잇는 회전 사각형(2삼각형). (Renderer2D::DrawLine과 동일 로직)
	void BuildLine(const vec2& _p1, const vec2& _p2, _f32 _thickness, const color& _color, FillResult& _out)
	{
		const _f32 dx = _p2.x - _p1.x;
		const _f32 dy = _p2.y - _p1.y;
		const _f32 length = sqrtf(dx * dx + dy * dy);
		if (length <= Epsilon_v) return;

		const vec2 center((_p1.x + _p2.x) * 0.5f, (_p1.y + _p2.y) * 0.5f);
		const _f32 radian = atan2f(dy, dx);
		const _f32 halfW = length * 0.5f;
		const _f32 halfH = _thickness * 0.5f;
		const _f32 c = cosf(radian);
		const _f32 s = sinf(radian);

		const vec2 locals[4] = {
			vec2(-halfW, -halfH), vec2(+halfW, -halfH),
			vec2(+halfW, +halfH), vec2(-halfW, +halfH),
		};
		const _s32 base = _out.vertices_.Size();
		for (_s32 i = 0; i < 4; ++i)
		{
			const _f32 x = locals[i].x * c - locals[i].y * s + center.x;
			const _f32 y = locals[i].x * s + locals[i].y * c + center.y;
			_out.vertices_.PushBack(VertexPTC{ vec3(x, y, 0.0f), vec2(0.0f, 0.0f), _color });
		}
		_out.indices_.PushBack((_u16)(base + 0)); _out.indices_.PushBack((_u16)(base + 1)); _out.indices_.PushBack((_u16)(base + 2));
		_out.indices_.PushBack((_u16)(base + 0)); _out.indices_.PushBack((_u16)(base + 2)); _out.indices_.PushBack((_u16)(base + 3));
		_out.pTexture_ = nullptr;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// 볼록 다각형 — 첫 점을 중심으로 삼각형 부채꼴. (사용자는 시계/반시계 무관, 순서만 지킴)
	void BuildPolygon(const vec2* _pPoints, _u32 _count, const color& _color, FillResult& _out)
	{
		if (_pPoints == nullptr || _count < 3) return;

		const _s32 base = _out.vertices_.Size();
		for (_u32 i = 0; i < _count; ++i)
		{
			_out.vertices_.PushBack(VertexPTC{ vec3(_pPoints[i].x, _pPoints[i].y, 0.0f), vec2(0.0f, 0.0f), _color });
		}
		for (_u32 i = 1; i + 1 < _count; ++i)
		{
			_out.indices_.PushBack((_u16)(base + 0));
			_out.indices_.PushBack((_u16)(base + i));
			_out.indices_.PushBack((_u16)(base + i + 1));
		}
		_out.pTexture_ = nullptr;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// 삼각형 1개.
	void BuildTriangle(const vec2& _p1, const vec2& _p2, const vec2& _p3, const color& _color, FillResult& _out)
	{
		const _s32 base = _out.vertices_.Size();
		_out.vertices_.PushBack(VertexPTC{ vec3(_p1.x, _p1.y, 0.0f), vec2(0.0f, 0.0f), _color });
		_out.vertices_.PushBack(VertexPTC{ vec3(_p2.x, _p2.y, 0.0f), vec2(0.0f, 0.0f), _color });
		_out.vertices_.PushBack(VertexPTC{ vec3(_p3.x, _p3.y, 0.0f), vec2(0.0f, 0.0f), _color });
		_out.indices_.PushBack((_u16)(base + 0)); _out.indices_.PushBack((_u16)(base + 1)); _out.indices_.PushBack((_u16)(base + 2));
		_out.pTexture_ = nullptr;
	}
}

NS_SGF_END
