/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:20:00 AM
 * =====================
 * 08. 카메라 좌표계 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/08_Camera/08_Camera_Function.h"

// 변환 행렬 상수 버퍼를 사용하는 HLSL 셰이더 소스를 반환한다. (07번과 동일 구조)
const char* CameraShaderSource()
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

// 카메라(뷰 행렬)의 원리를 콘솔에 출력한다. (학습용)
void PrintCameraExplanation()
{
	printf("\n[카메라의 비밀]\n");
	printf(" 카메라는 실제로 움직이지 않는다! 세상이 반대로 움직일 뿐이다.\n");
	printf(" 카메라가 오른쪽으로 3칸 이동 = 모든 물체가 왼쪽으로 3칸 이동\n");
	printf(" 그래서 뷰 행렬 = 카메라 이동의 '역변환' 행렬이다.\n");
	printf("   뷰 = Translation(-카메라위치) x Scale(줌배율)\n");
	printf(" 최종 변환: 로컬 -> [월드] -> 월드좌표 -> [뷰] -> 카메라기준 -> [투영] -> NDC\n\n");
}
