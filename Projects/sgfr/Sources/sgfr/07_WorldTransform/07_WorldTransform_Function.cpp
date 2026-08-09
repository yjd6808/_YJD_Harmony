/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:10:00 AM
 * =====================
 * 07. 월드 변환 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/07_WorldTransform/07_WorldTransform_Function.h"

// 상수 버퍼로 변환 행렬을 받는 HLSL 셰이더 소스를 반환한다.
//
// [row_major가 붙는 이유]
//  jc::Mat4는 행 우선(row-major) 방식으로 메모리에 저장된다.
//  HLSL의 기본값은 열 우선(column-major)이므로, 그대로 보내면 행렬이 뒤집힌다.
//  row_major 키워드를 붙이면 전치(Transpose) 없이 그대로 쓸 수 있다.
//
// [mul(v, M) 순서]
//  sgf는 행 벡터(row-vector) 규약을 쓴다. 즉 v' = v * M.
//  그래서 HLSL에서도 mul(벡터, 행렬) 순서로 곱한다.
const char* TransformShaderSource()
{
	return R"(
// === 변환 행렬 상수 버퍼 ===
cbuffer CbTransform : register(b0)
{
	row_major float4x4 gWorldViewProj;	// 월드 x 뷰 x 투영 결합 행렬
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

// 정점 셰이더: 로컬 좌표를 행렬로 변환해 클립 공간으로 보낸다.
// 드디어 "정점은 그대로, 행렬로 움직이는" 진짜 방식이 등장!
VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	output.position = mul(float4(_input.position, 1.0f), gWorldViewProj);
	output.color = _input.color;
	return output;
}

float4 PSMain(VSOutput _input) : SV_TARGET
{
	return _input.color;
}
)";
}

// 행렬 결합과 계층 구조의 원리를 콘솔에 출력한다. (학습용)
void PrintHierarchyExplanation()
{
	printf("\n[계층 변환의 핵심]\n");
	printf(" 태양 월드 = 태양 로컬(자전 x 이동)\n");
	printf(" 지구 월드 = 지구 로컬 x 태양 공전 행렬\n");
	printf(" 달   월드 = 달 로컬 x 지구 공전 행렬 x 태양 공전 행렬\n");
	printf(" => 부모가 움직이면 자식은 자동으로 따라 움직인다!\n");
	printf(" (행 벡터 규약이므로 왼쪽에 있는 행렬이 먼저 적용된다)\n");
	printf(" 이것이 바로 씬 그래프(Scene Graph)의 원리다. Cocos2d-x의 addChild도 똑같다!\n\n");
}
