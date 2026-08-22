/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026
 * 수정일: 8/17/2026 (2D/3D 엄격 분리 — 슈퍼셋 구조 제거)
 * =====================
 * 프리미티브 메시 타입 (2D/3D 분리)
 *
 * [무엇인가?]
 * ResourceMgr가 디폴트 리소스로 보관하는 프리미티브 메시의 종류를 나타내는 enum.
 * 기본 도형 메시를 한 번 만들어 두고 FindPrimitiveMesh2D/3D로 키를 꺼내 쓴다.
 *
 * [왜 2D/3D를 분리했나?]
 * - PrimitiveMesh2DType: 순수 2D만 (Rect/Circle/Triangle/Line — VertexPTC, 2D 배칭)
 * - PrimitiveMesh3DType: 순수 3D만 (Cube/Sphere/Capsule/Cylinder/Plane/Quad — VertexPNT)
 * - 각 enum은 자기 영역만 담당한다. 2D enum으로 3D 메시를 얻거나 그 반대는 컴파일 타임에 불가능.
 */

#pragma once

#include "sgf/Namespace.h"

NS_SGF_BEGIN

// 2D 프리미티브 메시 타입 — 순수 2D만 (VertexPTC — 2D 배칭)
enum class PrimitiveMesh2DType
{
	Rect,		// 1x1 사각형
	Circle,		// 반지름 1 원
	Triangle,	// 단위 삼각형
	Line,		// 가로 길이 1, 두께 1 선분

	Max
};

// 3D 프리미티브 메시 타입 — 순수 3D만 (VertexPNT)
enum class PrimitiveMesh3DType
{
	Cube,		// 1x1x1 큐브
	Sphere,		// 반지름 1 구
	Capsule,	// 반지름 0.5, 전체 높이 2 캡슐
	Cylinder,	// 반지름 1, 높이 2 원기둥
	Plane,		// XY 1x1 +Z 향 평면
	Quad,		// XY 1x1 +Z 향 쿼드

	Max
};

NS_SGF_END
