/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:40:00 PM
 * =====================
 * 디폴트 셰이더 소스 (v3 D-17: 코드 내장)
 *
 * [상수버퍼 슬롯 규약] (v3 FR-19)
 *  b0 = 프레임 공통 (view/projection/카메라 위치. SceneRenderer가 소유)
 *  b1 = 오브젝트 공통 (world. SceneRenderer가 오브젝트마다 갱신)
 *  b2 = 머티리얼 (baseColor. Material이 소유)
 *
 * [행렷 규약]
 *  엔진은 행우선(row-major)이므로 HLSL도 row_major로 선언하고
 *  mul(벡터, 행렬) 순서를 쓴다. (전치 없이 그대로 복사)
 */

#pragma once

#include "sgf/Namespace.h"

NS_SGF_BEGIN

// 2D용 디폴트 셰이더 (VertexPTC: POSITION/TEXCOORD/COLOR)
// 텍스처 x 정점색 x 머티리얼 색. 조명 없음.
static constexpr const char* DEFAULT_SHADER_SOURCE_2D = R"(
cbuffer CbFrame : register(b0)
{
	row_major float4x4 gView;
	row_major float4x4 gProjection;
	float4 gCameraPosition;
};

cbuffer CbObject : register(b1)
{
	row_major float4x4 gWorld;
};

cbuffer CbMaterial : register(b2)
{
	float4 gBaseColor;
};

Texture2D gTexture0 : register(t0);
SamplerState gSampler0 : register(s0);

struct VSInput
{
	float3 position : POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

PSInput VSMain(VSInput input)
{
	PSInput output;
	float4 worldPos = mul(float4(input.position, 1.0f), gWorld);
	output.position = mul(mul(worldPos, gView), gProjection);
	output.uv = input.uv;
	output.color = input.color;
	return output;
}

float4 PSMain(PSInput input) : SV_Target
{
	return gTexture0.Sample(gSampler0, input.uv) * input.color * gBaseColor;
}
)";

// 3D용 디폴트 셰이더 (VertexPNT: POSITION/NORMAL/TEXCOORD)
// 람버트 디렉셔널 조명 + 앵비언트. 빛 방향은 고정.
static constexpr const char* DEFAULT_SHADER_SOURCE_3D = R"(
cbuffer CbFrame : register(b0)
{
	row_major float4x4 gView;
	row_major float4x4 gProjection;
	float4 gCameraPosition;
};

cbuffer CbObject : register(b1)
{
	row_major float4x4 gWorld;
};

cbuffer CbMaterial : register(b2)
{
	float4 gBaseColor;
};

Texture2D gTexture0 : register(t0);
SamplerState gSampler0 : register(s0);

struct VSInput
{
	float3 position : POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

PSInput VSMain(VSInput input)
{
	PSInput output;
	float4 worldPos = mul(float4(input.position, 1.0f), gWorld);
	output.position = mul(mul(worldPos, gView), gProjection);
	// 법선은 회전만 적용되도록 3x3만 사용 (균등 스케일 가정)
	output.normal = normalize(mul(input.normal, (float3x3)gWorld));
	output.uv = input.uv;
	return output;
}

float4 PSMain(PSInput input) : SV_Target
{
	float3 lightDir = normalize(float3(-0.5f, -1.0f, 0.5f));	// 고정 방향광
	float ndotl = saturate(dot(normalize(input.normal), -lightDir));
	float lighting = 0.25f + 0.75f * ndotl;						// 앵비언트 0.25
	float4 texColor = gTexture0.Sample(gSampler0, input.uv);
	return float4(texColor.rgb * gBaseColor.rgb * lighting, texColor.a * gBaseColor.a);
}
)";

NS_SGF_END
