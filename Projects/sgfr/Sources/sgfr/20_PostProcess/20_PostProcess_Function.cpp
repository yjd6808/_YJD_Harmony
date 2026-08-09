/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:30:00 PM
 * =====================
 * 20. 포스트 프로세싱 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/20_PostProcess/20_PostProcess_Function.h"

using namespace sgf;
using namespace jc;

// 화면 전체 텍스처에 후보정 효과를 입혀 출력하는 HLSL 셰이더 소스를 반환한다.
//
// [포스트 프로세싱(Post Processing)이란?]
//  장면을 먼저 렌더 타깃(텍스처)에 다 그린 다음,
//  그 텍스처를 "사진 보정"하듯 픽셀 단위로 가공해서 화면에 출력하는 기법.
//  장면이 아무리 복잡해도 후보정 비용은 "화면 픽셀 수"에만 비례한다!
//
// [Before/After 분할 슬라이더]
//  gSplit(0~1) 왼쪽은 원본 그대로, 오른쪽은 효과를 적용해 출력한다.
//  경계를 좀이며 같은 장면의 보정 전/후를 한 화면에서 직접 비교할 수 있다.
const char* PostProcessShaderSource()
{
	return R"(
Texture2D gScene : register(t0);		// 장면이 그려진 렌더 타깃 텍스처
SamplerState gSampler : register(s0);

cbuffer CbPost : register(b0)
{
	int    gMode;		// 0=원본, 1=그레이, 2=세피아, 3=반전, 4=비네트, 5=물결
	float  gTime;		// 물결 애니메이션용 누적 시간
	float  gSplit;		// Before/After 경계 (UV x, 0~1). 왼쪽=원본, 오른쪽=효과
	float  gPadding;	// 16바이트 정렬용 여백
};

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
};

VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	output.position = float4(_input.position, 1.0f);	// 화면 전체 사각형, NDC 그대로
	output.uv = _input.uv;
	return output;
}

float4 PSMain(VSOutput _input) : SV_TARGET
{
	float2 uv = _input.uv;

	// [경계선] Before/After를 나누는 노란 세로선
	if (abs(uv.x - gSplit) < 0.002f)
	{
		return float4(1.0f, 0.85f, 0.2f, 1.0f);
	}

	// [Before] 경계 왼쪽: 어떤 효과도 없이 원본 그대로 출력
	if (uv.x < gSplit)
	{
		return gScene.Sample(gSampler, uv);
	}

	// [After] 경계 오른쪽: 선택한 효과 적용
	// [5. 물결] 샘플링 위치(UV) 자체를 사인파로 흔들어 버린다.
	if (gMode == 5)
	{
		uv.x += sin(uv.y * 30.0f + gTime * 4.0f) * 0.01f;
		uv.y += cos(uv.x * 30.0f + gTime * 4.0f) * 0.01f;
	}

	float4 color = gScene.Sample(gSampler, uv);

	if (gMode == 1)
	{
		// [1. 그레이스케일] 눈은 초록에 가장 민감 -> 가중치 평균(표준 계수)
		float gray = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));
		color.rgb = float3(gray, gray, gray);
	}
	else if (gMode == 2)
	{
		// [2. 세피아] 그레이로 만든 뒤 갈색 계열로 색을 입힌다 (오래된 사진 느낌)
		float gray = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));
		color.rgb = float3(gray * 1.07f, gray * 0.87f, gray * 0.65f);
	}
	else if (gMode == 3)
	{
		// [3. 색 반전] 1에서 몬다. 흰 <-> 검, 빨강 <-> 청록
		color.rgb = 1.0f - color.rgb;
	}
	else if (gMode == 4)
	{
		// [4. 비네트] 화면 중심에서 멀어질수록 어둡게 (카메라 렌즈 주변감)
		float dist = length(_input.uv - float2(0.5f, 0.5f));
		float vignette = saturate(1.0f - dist * 1.4f);
		color.rgb *= vignette * vignette;
	}

	return color;
}
)";
}

// 포스트 프로세싱의 개념과 각 효과의 수식을 콘솔에 출력한다. (학습용)
void PrintPostProcessExplanation()
{
	printf("\n[포스트 프로세싱이란?]\n");
	printf(" 1단계: 장면을 화면이 아닌 '렌더 타깃 텍스처'에 그린다 (19번에서 배움!)\n");
	printf(" 2단계: 화면 전체 사각형을 그리면서 그 텍스처를 픽셀단위로 가공한다\n");
	printf(" = 게임 화면에 '사진 필터'를 입히는 것과 같다!\n\n");
	printf("[Before/After 비교 슬라이더]\n");
	printf(" 노란 세로선 왼쪽  = 원본(Before), 오른쪽 = 효과 적용(After)\n");
	printf(" 왼쪽/오른쪽 방향키로 경계를 직접 좀여가며 비교해보세요!\n\n");
	printf("[효과 목록 (0~5 키로 전환)]\n");
	printf(" 0. 원본       : 효과 없음 (경계 양쪽이 완전히 같아진다)\n");
	printf(" 1. 그레이     : 밝기만 남긴다. gray = 0.299R + 0.587G + 0.114B\n");
	printf(" 2. 세피아     : 그레이에 갈색 입히기. 옛날 사진 느낌\n");
	printf(" 3. 색 반전    : 1 - RGB\n");
	printf(" 4. 비네트     : 중심에서 멀수록 어둡게. 시선 집중 효과\n");
	printf(" 5. 물결       : UV를 사인파로 흔들기. 취함/지지직 이펙트\n\n");
	printf(" 핵심: 장면이 아무리 복잡해도 후보정 비용은 화면 픽셀 수에만 비례한다!\n\n");
}

// 화면 전체를 덮는 NDC 사각형 정점 4개를 채운다. (UV 0~1, 흰색)
void FillFullscreenQuad(VertexPTC* _pOutVertices4)
{
	const Color white = Color::White();

	// NDC 왼위(-1,+1) -> 오른아래(+1,-1)까지 꽉 채운다.
	// UV는 왼위가 (0,0): 화면 y는 위가 +, 텍스처 v는 아래가 + 임에 주의!
	_pOutVertices4[0] = { Vec3(-1.0f, +1.0f, 0.0f), Vec2(0.0f, 0.0f), white };
	_pOutVertices4[1] = { Vec3(+1.0f, +1.0f, 0.0f), Vec2(1.0f, 0.0f), white };
	_pOutVertices4[2] = { Vec3(-1.0f, -1.0f, 0.0f), Vec2(0.0f, 1.0f), white };
	_pOutVertices4[3] = { Vec3(+1.0f, -1.0f, 0.0f), Vec2(1.0f, 1.0f), white };
}
