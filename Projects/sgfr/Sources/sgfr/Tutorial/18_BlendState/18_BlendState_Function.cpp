/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 1:50:00 PM
 * =====================
 * 18. 블렌드 스테이트 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/Tutorial/18_BlendState/18_BlendState_Function.h"

using namespace sgf;
using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////

// 부드러운 원 텍스처를 그리는 HLSL 셰이더 소스를 반환한다.
//
// [블렌딩(Blending)이란?]
// 픽셀 셰이더가 계산한 "새 색"과 백버퍼에 "이미 그려진 색"을
// 어떻게 섞을지 정하는 출력 병합(OM) 단계의 규칙이다.
// 섞는 공식은 C++의 블렌드 스테이트가 정하고, 셰이더는 색만 내밀면 된다.
const char* BlendQuadShaderSource()
{
	return R"(
Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

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
	output.position = float4(_input.position, 1.0f);	// NDC 그대로 통과
	output.uv = _input.uv;
	output.color = _input.color;
	return output;
}

float4 PSMain(VSOutput _input) : SV_TARGET
{
	// 흰색 원 텍스처 x 정점 색 = 색입혀진 부드러운 원
	float4 tex = gTexture.Sample(gSampler, _input.uv) * _input.color;

	// [곱셈 블렌드를 위한 트릭]
	// 곱셈 모드는 알파를 보지 않고 "기존색 x 새색"을 그대로 계산한다.
	// 원 밖(알파 0)이 검은 사각형으로 나오지 않도록
	// 알파가 0일수록 색을 흰색(x1 = 변화 없음)으로 섞어서 내보낸다.
	return float4(lerp(float3(1.0f, 1.0f, 1.0f), tex.rgb, tex.a), tex.a);
}
)";
}

//////////////////////////////////////////////////////////////////////////////////////////

// 블렌딩 공식과 네 가지 모드의 원리를 콘솔에 출력한다. (학습용)
void PrintBlendExplanation()
{
	jc::Console::WriteLine("\n[블렌딩 공식]");
	jc::Console::WriteLine(" 최종색 = 새색 x SrcBlend  (+)  기존색 x DestBlend");
	jc::Console::WriteLine(" SrcBlend/DestBlend 계수만 바꿔 끼우면 아래 모든 효과가 나온다!\n");
	jc::Console::WriteLine("[네 가지 모드 (1~4 키로 전환)]");
	jc::Console::WriteLine(" 1. Opaque   : 섞지 않고 덮어쓴다          (알파 무시! 사각형이 통째로 보인다)");
	jc::Console::WriteLine(" 2. Alpha    : 새색x알파 + 기존색x(1-알파) (일반적인 반투명, UI/스프라이트)");
	jc::Console::WriteLine(" 3. Additive : 새색x알파 + 기존색          (밝아지기만 한다! 빛/폭발/이펙트)");
	jc::Console::WriteLine(" 4. Multiply : 새색 x 기존색               (어두워지기만 한다! 그림자/스테인드글라스)\n");
	jc::Console::WriteLine(" 세 원이 겹치는 부분을 관찰하세요:");
	jc::Console::WriteLine(" - Additive는 빨+초+파 = 흰색으로 밝아진다 (빛의 삼원색!)");
	jc::Console::WriteLine(" - Multiply는 겹칠수록 검게 어두워진다 (물감의 삼원색처럼)\n");
}

//////////////////////////////////////////////////////////////////////////////////////////

// 중심에서 가장자리로 갈수록 알파가 부드럽게 0으로 줄어드는 흰색 원 텍스처를 채운다.
void FillSoftCirclePixels(_u8* _pOutPixels, _s32 _size)
{
	const _f32 center = (_size - 1) * 0.5f;	// 중심 픽셀 좌표
	const _f32 maxDist = center;			// 중심에서 가장자리까지 거리

	for (_s32 y = 0; y < _size; ++y)
	{
		for (_s32 x = 0; x < _size; ++x)
		{
			// 중심에서의 거리를 0~1로 정규화
			const _f32 dx = x - center;
			const _f32 dy = y - center;
			const _f32 dist = sqrtf(dx * dx + dy * dy) / maxDist;

			// 안쪽 70%는 완전 불투명, 그 밖은 가장자리로 갈수록 부드럽게 0으로
			_f32 alpha = 1.0f;
			if (dist > 0.7f)
			{
				alpha = Clamp(1.0f - (dist - 0.7f) / 0.3f, 0.0f, 1.0f);
			}

			// 색은 흰색 고정: 실제 색은 정점 색으로 입힌다 (텍스처 재활용!)
			_u8* pPixel = _pOutPixels + (y * _size + x) * 4;
			pPixel[0] = 255;
			pPixel[1] = 255;
			pPixel[2] = 255;
			pPixel[3] = static_cast<_u8>(alpha * 255.0f);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

// NDC 좌표기준 사각형 정점 4개를 채운다. (UV 0~1, 지정 색 채색)
void FillQuadVertices(VertexPTC* _pOutVertices4, const vec2& _center, _f32 _halfSize, const color& _color)
{
	// 왼위 -> 오른위 -> 왼아래 -> 오른아래 (인덱스 0,1,2 / 2,1,3과 짝)
	_pOutVertices4[0] = { vec3(_center.x - _halfSize, _center.y + _halfSize, 0.0f), vec2(0.0f, 0.0f), _color };
	_pOutVertices4[1] = { vec3(_center.x + _halfSize, _center.y + _halfSize, 0.0f), vec2(1.0f, 0.0f), _color };
	_pOutVertices4[2] = { vec3(_center.x - _halfSize, _center.y - _halfSize, 0.0f), vec2(0.0f, 1.0f), _color };
	_pOutVertices4[3] = { vec3(_center.x + _halfSize, _center.y - _halfSize, 0.0f), vec2(1.0f, 1.0f), _color };
}
