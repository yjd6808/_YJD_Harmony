/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:42:00 PM
 * =====================
 * 공용 정점 구조체 정의
 *
 * [정점(Vertex)이란?]
 * 3D 모델을 구성하는 점 하나. 위치만 있는 게 아니라 색상/텍스처좌표/법선 등
 * 다양한 부가 정보를 가질 수 있다. 삼각형 = 정점 3개.
 * 어떤 정보를 담을지는 그리려는 대상에 따라 다르므로 여러 종류를 정의한다.
 */

#pragma once

#include "jc/Math.h"
#include "sgf/Graphics/VertexDeclaration.h"

NS_SGF_BEGIN

using namespace jc;

// 위치 + 색상 정점 (단순 도형, 선 그리기용)
struct VertexPC
{
	vec3 position_;		// 로컬 좌표계 위치
	color color_;		// 정점 색상 (삼각형 내부는 자동 보간됨)

	// 메모리 배치 선언 — 오프셋은 자동 누적 (하드코딩 제거)
	static const VertexDeclaration* Decl()
	{
		static const VertexElement ELEMENTS[] =
		{
			{ VertexSemantic::vsPosition, 0, VertexElementFormat::vefFloat3 },
			{ VertexSemantic::vsColor, 0, VertexElementFormat::vefColor32 },
		};
		static const VertexDeclaration* s_pDecl = VertexDeclaration::GetOrCreate(ELEMENTS);
		return s_pDecl;
	}
};

// 위치 + 텍스처좌표 + 색상 정점 (스프라이트용)
// [텍스처 좌표 UV란?]
// 이미지의 어느 부분을 이 정점에 붙일지 나타내는 0~1 좌표.
// (0,0)=이미지 왼위, (1,1)=이미지 오른아래.
struct VertexPTC
{
	vec3 position_;		// 로컬 좌표계 위치
	vec2 uv_;			// 텍스처 좌표
	color color_;		// 색상 틴트 (흰색이면 원본 그대로)

	static const VertexDeclaration* Decl()
	{
		static const VertexElement ELEMENTS[] =
		{
			{ VertexSemantic::vsPosition, 0, VertexElementFormat::vefFloat3 },
			{ VertexSemantic::vsTexCoord, 0, VertexElementFormat::vefFloat2 },
			{ VertexSemantic::vsColor, 0, VertexElementFormat::vefColor32 },
		};
		static const VertexDeclaration* s_pDecl = VertexDeclaration::GetOrCreate(ELEMENTS);
		return s_pDecl;
	}
};

// 위치 + 법선 + 텍스처좌표 정점 (3D 조명용)
// [법선(Normal)이란?]
// 면이 바라보는 방향의 단위 벡터. 빛 방향과의 각도로 밝기를 계산한다.
// 빛을 정면으로 받으면 밝고, 비스듬히 받으면 어둡다. (람버트 조명)
struct VertexPNT
{
	vec3 position_;		// 로컬 좌표계 위치
	vec3 normal_;		// 법선 벡터
	vec2 uv_;			// 텍스처 좌표

	static const VertexDeclaration* Decl()
	{
		static const VertexElement ELEMENTS[] =
		{
			{ VertexSemantic::vsPosition, 0, VertexElementFormat::vefFloat3 },
			{ VertexSemantic::vsNormal, 0, VertexElementFormat::vefFloat3 },
			{ VertexSemantic::vsTexCoord, 0, VertexElementFormat::vefFloat2 },
		};
		static const VertexDeclaration* s_pDecl = VertexDeclaration::GetOrCreate(ELEMENTS);
		return s_pDecl;
	}
};

NS_SGF_END
