/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:50:00 AM
 * =====================
 * 06. HLSL 셰이더 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/06_HLSL/06_HLSL_Function.h"

// 상수 버퍼로 시간을 받아 움직이는 HLSL 셰이더 소스를 반환한다.
//
// [상수 버퍼(Constant Buffer)란?]
//  C++에서 GPU로 "매 프레임 바뀌는 작은 데이터"를 보내는 통로.
//  정점 버퍼는 '정점마다 다른 값', 상수 버퍼는 '모든 정점이 공유하는 값'이다.
//
// [16바이트 규칙 (중요!)]
//  GPU 상수 버퍼는 16바이트 단위로 정렬된다.
//  float 1개(4바이트)만 보내도 나머지 12바이트를 채워서 16바이트로 맞춰야 한다.
//  그래서 gTime 뒤에 float3 gPadding을 넣었다. (C++ 쪽 구조체도 똑같이!)
const char* AnimatedShaderSource()
{
	return R"(
cbuffer CbTime : register(b0)
{
	float  gTime;		// 경과 시간(초). C++에서 매 프레임 갱신한다.
	float3 gPadding;	// 16바이트 정렬용 빈 공간 (사용하지 않음)
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

// 정점 셰이더: 시간에 따라 삼각형 크기를 맥박처럼 키웠다 줄였다 한다.
// sin은 -1~+1을 오가므로 0.25를 곱하고 1을 더하면 0.75~1.25 배율이 된다.
VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	float scale = sin(gTime * 2.0f) * 0.25f + 1.0f;
	output.position = float4(_input.position * scale, 1.0f);
	output.color = _input.color;
	return output;
}

// 픽셀 셰이더: 원래 색과 반전색 사이를 시간에 따라 오가며 섞는다.
// lerp(a, b, t): t=0이면 a, t=1이면 b, 중간이면 그 사이 값. (선형 보간)
float4 PSMain(VSOutput _input) : SV_TARGET
{
	float t = sin(gTime * 3.0f) * 0.5f + 0.5f;		// 0~1 왕복
	float3 inverted = 1.0f - _input.color.rgb;		// 반전색
	float3 mixed = lerp(_input.color.rgb, inverted, t);
	return float4(mixed, _input.color.a);
}
)";
}

// HLSL 문법 핵심 요약을 콘솔에 출력한다. (학습용)
void PrintHlslSummary()
{
	printf("\n[HLSL 핵심 요약]\n");
	printf(" 1. HLSL은 GPU에서 돌아가는 C유사 언어다. (High Level Shading Language)\n");
	printf(" 2. 기본 타입: float / float2 / float3 / float4 / float4x4 (행렬)\n");
	printf(" 3. 시맨틱(Semantic): 변수의 '역할 꾸리표'. POSITION, COLOR0, SV_POSITION 등\n");
	printf("    - SV_ 접두사는 GPU가 특별 취급하는 시스템 값 (SV_POSITION = 최종 위치)\n");
	printf(" 4. cbuffer: C++이 보내주는 공유 데이터. register(b0)의 0이 슬롯 번호\n");
	printf(" 5. 내장 함수: sin/cos/lerp/saturate/normalize/dot/mul 등\n");
	printf(" 6. 16바이트 정렬: cbuffer 크기는 반드시 16의 배수여야 한다! (패딩 필수)\n\n");
}
