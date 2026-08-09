/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 1:40:00 PM
 * =====================
 * 15. 샘플러와 필터링 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/15_SamplerFiltering/15_SamplerFiltering_Function.h"

using namespace sgf;
using namespace jc;

// 좌(Before)/우(After) 분할 비교로 텍스처 사각형을 그리는 HLSL 셰이더 소스를 반환한다.
//
// [샘플링(Sampling)이란?]
//  셰이더가 "텍스처의 UV 위치에서 색을 꺼내 오는 행위".
//  화면 픽셀과 텍스처 픽셀(텍셀)은 1:1로 맞지 않기 때문에
//  "어떻게 꺼내 올지" 규칙이 필요하다. 그 규칙 묶음이 샘플러다.
//
// [Before/After 분할의 원리]
//  같은 텍스처를 s0(기준 샘플러)과 s1(선택 샘플러) 두 규칙으로 읽을 수 있게 준비하고,
//  픽셀 셰이더가 자기 화면 x좌표(SV_POSITION)를 경계값과 비교해 어느 쪽을 쓸지 고른다.
//  덕분에 한 번의 드로우로 좌우가 다른 규칙으로 그려진다!
const char* TextureQuadShaderSource()
{
	return R"(
Texture2D gTexture : register(t0);			// 읽을 텍스처 (t = texture 레지스터)
SamplerState gSamplerBefore : register(s0);	// [Before] 기준 규칙: Point + Wrap 고정
SamplerState gSamplerAfter  : register(s1);	// [After]  현재 선택한 규칙

cbuffer CbSplit : register(b0)
{
	float  gSplitPixelX;	// 좌/우를 나누는 경계의 픽셀 x좌표 (화면 절반)
	float3 gPadding;		// 16바이트 정렬용 여백
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
	float4 color    : COLOR0;
};

VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	// 이미 NDC(-1~+1) 좌표로 만든 정점이므로 행렬 계산 없이 그대로 통과시킨다.
	output.position = float4(_input.position, 1.0f);
	output.uv = _input.uv;
	output.color = _input.color;
	return output;
}

float4 PSMain(VSOutput _input) : SV_TARGET
{
	// SV_POSITION은 픽셀 셰이더에서 "현재 픽셀의 화면 좌표"로 읽힌다.
	// 경계선 주변 픽셀은 노란 세로선으로 칠해 Before/After 영역을 눈에 띄게 나눈다.
	if (abs(_input.position.x - gSplitPixelX) < 1.5f)
	{
		return float4(1.0f, 0.85f, 0.2f, 1.0f);
	}

	// === 이 부분이 오늘의 주인공! ===
	// Sample(규칙, UV): 규칙(필터/주소모드)에 따라 UV 위치의 색을 꺼내 온다.
	// 왼쪽 화면은 기준 규칙(s0), 오른쪽 화면은 선택 규칙(s1)으로 같은 텍스처를 읽는다.
	if (_input.position.x < gSplitPixelX)
	{
		return gTexture.Sample(gSamplerBefore, _input.uv) * _input.color;	// Before
	}
	return gTexture.Sample(gSamplerAfter, _input.uv) * _input.color;		// After
}
)";
}

// 샘플러/필터링의 원리를 콘솔에 출력한다. (학습용)
void PrintSamplerExplanation()
{
	printf("\n[샘플링(Sampling)이란?]\n");
	printf(" 화면 픽셀 하나를 칠할 때 텍스처의 어느 색을 꺼내 올지 정하는 규칙입니다.\n");
	printf(" 작은 텍스처를 크게 늘리면(확대) 화면 픽셀 여러 개가 텍셀 하나를 나눠 씁니다.\n\n");
	printf("[Before/After 비교 뷰]\n");
	printf(" 노란 세로선 왼쪽  = 기준 상태 (Point + Wrap 고정)\n");
	printf(" 노란 세로선 오른쪽 = 현재 선택한 필터/주소 모드\n");
	printf(" 키를 눌러 오른쪽만 바꿔가며 왼쪽과 직접 비교해보세요!\n\n");
	printf("[필터 (1/2 키로 전환)]\n");
	printf(" 1. Point  : 가장 가까운 텍셀 하나를 그대로 사용 -> 네모가 각진 도트 느낌\n");
	printf(" 2. Linear : 주변 텍셀 4개를 거리비례로 섞음   -> 부드럽지만 흐릿해짐\n\n");
	printf("[주소 모드 (3/4/5 키로 전환)] - UV가 0~1 범위를 벗어났을 때\n");
	printf(" 3. Wrap   : 처음부터 반복 (바닥 타일링에 필수)\n");
	printf(" 4. Clamp  : 가장자리 색을 잡아당겨 늘임 (빨간 테두리가 줄줄 늘어난다!)\n");
	printf(" 5. Mirror : 거울처럼 뒤집으며 반복\n\n");
	printf(" 이 화면의 사각형은 UV를 0~3으로 주므로 주소 모드 차이가 바로 보입니다.\n");
	printf(" 텍스처는 고작 32x32 픽셀! 확대해 보면 필터 차이도 바로 보입니다.\n\n");
}

// 체커보드 패턴의 RGBA 픽셀 배열을 채운다.
void FillCheckerPixels(_u8* _pOutPixels, int _width, int _height, int _cellSize)
{
	for (int y = 0; y < _height; ++y)
	{
		for (int x = 0; x < _width; ++x)
		{
			_u8 r, g, b;

			// 가장자리 1픽셀은 빨간 테두리:
			// Clamp 모드에서 이 테두리가 줄줄 늘어나는 것을 관찰하기 위해서다.
			const bool bBorder = (x == 0 || y == 0 || x == _width - 1 || y == _height - 1);
			if (bBorder)
			{
				r = 220; g = 60; b = 60;
			}
			else
			{
				// (칸 가로 번호 + 칸 세로 번호)가 짝수면 밝은 칸, 홀수면 파란 칸
				const bool bLight = (((x / _cellSize) + (y / _cellSize)) % 2) == 0;
				if (bLight) { r = 240; g = 240; b = 240; }
				else        { r = 40;  g = 70;  b = 160; }
			}

			// RGBA 8비트씩, 한 줄은 _width * 4바이트
			_u8* pPixel = _pOutPixels + (y * _width + x) * 4;
			pPixel[0] = r;
			pPixel[1] = g;
			pPixel[2] = b;
			pPixel[3] = 255;
		}
	}
}

// UV가 0~_uvScale 범위인 사각형 정점 4개와 인덱스 6개를 채운다.
void FillUvQuad(VertexPTC* _pOutVertices4, _u32* _pOutIndices6, _f32 _uvScale)
{
	const Color white = Color::White();

	// NDC 좌표로 화면 대부분을 덮는 사각형 (왼위 -> 오른위 -> 왼아래 -> 오른아래)
	_pOutVertices4[0] = { Vec3(-0.85f, +0.85f, 0.0f), Vec2(0.0f,     0.0f),     white };
	_pOutVertices4[1] = { Vec3(+0.85f, +0.85f, 0.0f), Vec2(_uvScale, 0.0f),     white };
	_pOutVertices4[2] = { Vec3(-0.85f, -0.85f, 0.0f), Vec2(0.0f,     _uvScale), white };
	_pOutVertices4[3] = { Vec3(+0.85f, -0.85f, 0.0f), Vec2(_uvScale, _uvScale), white };

	// 삼각형 2개 (시계 방향)
	_pOutIndices6[0] = 0; _pOutIndices6[1] = 1; _pOutIndices6[2] = 2;
	_pOutIndices6[3] = 2; _pOutIndices6[4] = 1; _pOutIndices6[5] = 3;
}
