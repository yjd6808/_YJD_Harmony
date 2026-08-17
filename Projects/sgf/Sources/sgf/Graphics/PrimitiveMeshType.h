/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026
 * =====================
 * 프리미티브 메시 타입 (2D/3D 분리)
 *
 * [무엇인가?]
 * ResourceMgr가 디폴트 리소스로 보관하는 프리미티브 메시의 종류를 나타내는 enum.
 * 기본 도형 메시를 한 번 만들어 두고 FindPrimitiveMesh2D/3D로 키를 꺼내 쓴다.
 *
 * [왜 2D/3D를 분리했나?]
 * - PrimitiveMesh2DType: 순수 2D(Rect/Circle/Triangle/Line) + 3D 공용(Cube 등)을 모두 포함한 슈퍼셋.
 * → "2D enum은 3D에서도 사용 가능" — FindPrimitiveMesh2D(PrimitiveMesh2DType::Cube)로 3D 메시를 얻는다.
 * - PrimitiveMesh3DType: 3D 공용만 보유. 2D 타입이 없으므로 3D 경로로는 사각형 등을 요청할 수 없다. (반대 불가)
 *
 * [정렬 규약]
 * PrimitiveMesh2DType의 3D 부분(4~9)은 PrimitiveMesh3DType(0~5)과 같은 순서로 정렬한다.
 * 두 Find 함수가 같은 통합 키 테이블을 가리키므로 3D 메시는 2D/3D 경로가 동일 인스턴스를 반환한다.
 */

#pragma once

#include "sgf/Namespace.h"

NS_SGF_BEGIN

// 2D 프리미티브 메시 타입 — 순수 2D(0~3) + 3D 공용(4~9) 슈퍼셋
enum class PrimitiveMesh2DType
{
	Rect,		// 1x1 사각형 (VertexPTC — 2D 배칭)
	Circle,		// 반지름 1 원 (VertexPTC)
	Triangle,	// 단위 삼각형 (VertexPTC)
	Line,		// 가로 길이 1, 두께 1 선분 (VertexPTC)

	Cube,		// 1x1x1 큐브 (VertexPNT — 3D)
	Sphere,		// 반지름 1 구 (VertexPNT)
	Capsule,	// 반지름 0.5, 전체 높이 2 캡슐 (VertexPNT)
	Cylinder,	// 반지름 1, 높이 2 원기둥 (VertexPNT)
	Plane,		// XY 1x1 +Z 향 평면 (VertexPNT)
	Quad,		// XY 1x1 +Z 향 쿼드 (VertexPNT)

	Max
};

// 3D 프리미티브 메시 타입 — 3D 공용만 (PrimitiveMesh2DType의 3D 부분과 동일 순서)
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
