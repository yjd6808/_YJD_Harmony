/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (프리미티브 자동 드로우 시스템)
 * =====================
 * 2D 프리미티브 공용 기하 빌더
 *
 * [역할]
 * 절대좌표/색을 받아 FillResult(정점+인덱스)를 채우는 순수 함수 모음.
 * Scene2D의 엔진 내장 채움 콜백(FillEngine 6종)과 Mesh 2D 팩토리(Rect/Circle/Line/Triangle)가
 * 같은 기하 생성 로직을 공유해 중복을 없앤다.
 *
 * [규약]
 * - 출력 버퍼(_out)는 호출자가 미리 Clear한다. (렌더러 BuildFill이 선처리 보장)
 * - 모든 빌더는 GPU 무접촉 — 정점/인덱스만 생성한다.
 */

#pragma once

#include "jc/Math.h"
#include "sgf/Graphics/Fill.h"

NS_SGF_BEGIN

using namespace jc;

namespace PrimitiveBuilder
{
	// 공용 쿼드 — 네 꼭짓점(좌하단 시작) + UV + 색. (A-7 공용화 — Fill의 PushQuad2D와 동일 순서)
	void PushQuad(FillResult& _out, _f32 _x1, _f32 _y1, _f32 _x2, _f32 _y2,
		const vec2& _uvMin, const vec2& _uvMax, const color& _color);

	// 단색 사각형 — 영역 전체를 채운 쿼드 1개
	void BuildRect(const rect& _region, const color& _color, FillResult& _out);

	// 원 — 중심에서 부채꼴 삼각형들로 근사 (세그먼트 ≥ 3)
	void BuildCircle(const vec2& _center, _f32 _radius, const color& _color, _u32 _segments, FillResult& _out);

	// 두께 있는 선분 — 두 점을 잇는 회전 사각형
	void BuildLine(const vec2& _p1, const vec2& _p2, _f32 _thickness, const color& _color, FillResult& _out);

	// 볼록 다각형 — 첫 점을 중심으로 삼각형 부채꼴 (순서만 지키면 시계/반시계 무관)
	void BuildPolygon(const vec2* _pPoints, _u32 _count, const color& _color, FillResult& _out);

	// 삼각형 1개
	void BuildTriangle(const vec2& _p1, const vec2& _p2, const vec2& _p3, const color& _color, FillResult& _out);
}

NS_SGF_END
