/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 11:30:00 AM
 * =====================
 * 튜토리얼 공용 헬퍼 구현부
 */

#include "Core.h"
#include "sgfr/Common/TutorialCommon.h"

using namespace sgf;
using namespace jc;

// =====================================================================================
// 색 변환 셰이더: WVP 변환 + 정점 색을 그대로 그린다.
// (17/19/20 튜토리얼이 동일하게 사용하던 본문을 한 곳으로 모은 것)
// =====================================================================================
const char* ColorTransformShaderSource()
{
	return R"(
cbuffer CbTransform : register(b0)
{
	row_major float4x4 gWvp;	// 월드 x 뷰 x 투영 합성 행렬
};

struct VSInput
{
	float3 position : POSITION;
	float4 color    : COLOR0;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	float4 color    : COLOR0;
};

VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	// 행벡터 규약: v' = v * M
	output.position = mul(float4(_input.position, 1.0f), gWvp);
	output.color = _input.color;
	return output;
}

float4 PSMain(VSOutput _input) : SV_TARGET
{
	return _input.color;
}
)";
}

// =====================================================================================
// 텍스처 샘플 셰이더: NDC 통과 + UV 샘플 x 틴트 색.
// (11/12/19/22 튜토리얼이 동일하게 사용 — 11의 기존 구현을 승계)
// =====================================================================================
const char* TextureShaderSource()
{
	return R"(
Texture2D    gTexture : register(t0);	// 텍스처 (C++ Texture::Bind의 slot 0)
SamplerState gSampler : register(s0);	// 샘플러 (GraphicDevice가 자동 바인딩)

struct VSInput
{
	float3 position : POSITION;
	float2 uv       : TEXCOORD0;
	float4 color    : COLOR0;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	float2 uv       : TEXCOORD0;
	float4 color    : COLOR0;
};

// 정점 셰이더: NDC 그대로 통과 + UV/색 전달
VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	output.position = float4(_input.position, 1.0f);
	output.uv = _input.uv;
	output.color = _input.color;
	return output;
}

// 픽셀 셰이더: UV 위치의 텍스처 색 x 틴트 색
float4 PSMain(VSOutput _input) : SV_TARGET
{
	return gTexture.Sample(gSampler, _input.uv) * _input.color;
}
)";
}

// =====================================================================================
// 면마다 색이 다른 정육면체: 정점 8개(반 변 0.5) + 인덱스 36개.
// 바깥에서 볼 때 시계 방향 감기 = 앞면 (D3D 기본).
// (17/19/20 튜토리얼이 동일하게 사용)
// =====================================================================================
void FillColorCube(VertexPC* _pOutVertices8, _u32* _pOutIndices36)
{
	// 앞면(z=-0.5) 4개 -> 뒷면(z=+0.5) 4개
	_pOutVertices8[0] = { vec3(-0.5f, +0.5f, -0.5f), color(1.0f, 0.3f, 0.3f, 1.0f) };	// 앞-왼위
	_pOutVertices8[1] = { vec3(+0.5f, +0.5f, -0.5f), color(1.0f, 1.0f, 0.3f, 1.0f) };	// 앞-오른위
	_pOutVertices8[2] = { vec3(-0.5f, -0.5f, -0.5f), color(0.3f, 1.0f, 0.3f, 1.0f) };	// 앞-왼아래
	_pOutVertices8[3] = { vec3(+0.5f, -0.5f, -0.5f), color(0.3f, 1.0f, 1.0f, 1.0f) };	// 앞-오른아래
	_pOutVertices8[4] = { vec3(-0.5f, +0.5f, +0.5f), color(0.3f, 0.3f, 1.0f, 1.0f) };	// 뒤-왼위
	_pOutVertices8[5] = { vec3(+0.5f, +0.5f, +0.5f), color(1.0f, 0.3f, 1.0f, 1.0f) };	// 뒤-오른위
	_pOutVertices8[6] = { vec3(-0.5f, -0.5f, +0.5f), color(1.0f, 1.0f, 1.0f, 1.0f) };	// 뒤-왼아래
	_pOutVertices8[7] = { vec3(+0.5f, -0.5f, +0.5f), color(0.4f, 0.4f, 0.4f, 1.0f) };	// 뒤-오른아래

	// 각 면마다 삼각형 2개, 바깥에서 볼 때 시계 방향
	const _u32 indices[36] = {
		0, 1, 2,  2, 1, 3,	// 앞면   (-Z)
		5, 4, 7,  7, 4, 6,	// 뒷면   (+Z)
		4, 0, 6,  6, 0, 2,	// 왼면   (-X)
		1, 5, 3,  3, 5, 7,	// 오른면 (+X)
		4, 5, 0,  0, 5, 1,	// 윗면   (+Y)
		2, 3, 6,  6, 3, 7,	// 아랫면 (-Y)
	};
	for (int i = 0; i < 36; ++i)
	{
		_pOutIndices36[i] = indices[i];
	}
}