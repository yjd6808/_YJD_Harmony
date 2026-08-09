/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:30:00 AM
 * =====================
 * 09. 3D 큐브 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/09_Cube3D/09_Cube3D_Function.h"

// 변환 행렬 상수 버퍼를 사용하는 HLSL 셰이더 소스를 반환한다. (07/08번과 동일 구조)
const char* CubeShaderSource()
{
	return R"(
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

// 깊이 버퍼와 원근 투영의 원리를 콘솔에 출력한다. (학습용)
void PrintDepthAndPerspectiveExplanation()
{
	printf("\n[원근 투영 (Perspective Projection)]\n");
	printf(" 멀리 있는 물체일수록 작게 보이게 만드는 투영. (사람 눈과 동일)\n");
	printf(" PerspectiveFovLH(시야각, 화면비율, 근평면, 원평면)으로 만든다.\n");
	printf(" 직교 투영(지금까지 사용)은 거리와 무관하게 같은 크기 => 2D/UI용.\n");
	printf("\n[깊이 버퍼 (Depth Buffer / Z-Buffer)]\n");
	printf(" 픽셀마다 '지금까지 그려진 가장 가까운 깊이'를 기억하는 별도 버퍼.\n");
	printf(" 새 픽셀이 더 멀다면 버리고, 더 가깝다면 덮어쓴다. => 그리는 순서와 무관하게 앞뒤가 정확해진다!\n");
	printf(" 이게 없으면 나중에 그린 뒷면이 앞면을 덮어버리는 참사가 벌어진다.\n");
	printf(" sgf에서는 device.SetDepthTest(true)로 켜고 끈다.\n\n");
}
