/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:10:00 PM
 * =====================
 * 24. 셰이딩 모델 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/Tutorial/24_ShadingModel/24_ShadingModel_Function.h"

#include <math.h>

using namespace sgf;
using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////

// 램버트/퐁/블린-퐁을 모두 지원하는 HLSL 셰이더 소스를 반환한다.
//
// [셰이딩(Shading)이란?]
//  "빛이 표면에서 어떻게 반사되는가"를 수식으로 흉내 내는 것.
//  보통 세 가지 성분을 더한다:
//   1) 주변광(Ambient)  : 사방에서 은은하게 도달하는 기본 밝기
//   2) 확산광(Diffuse)  : 표면이 빛을 마주보는 정도 (램버트 법칙, N·L)
//   3) 정반사광(Specular): 거울처럼 반짝이는 하이라이트 (카메라 위치 필요!)
const char* ShadingShaderSource()
{
	return R"(
cbuffer CbTransform : register(b0)
{
	row_major float4x4 gWorld;	// 모델 -> 월드 (법선 변환에 필요)
	row_major float4x4 gWvp;	// 합성 변환
};

cbuffer CbShading : register(b1)
{
	float3 gLightDir;	// 빛이 나아가는 방향
	int    gMode;		// 0=램버트, 1=퐁, 2=블린-퐁
	float3 gCameraPos;	// 월드 공간 카메라 위치 (정반사광 계산용)
	float  gSpecPower;	// 하이라이트 날카로움 (클수록 작고 선명)
	float4 gBaseColor;	// 물체 기본색
};

struct VSInput
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float2 uv       : TEXCOORD0;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	float3 normal   : NORMAL;
	float3 worldPos : TEXCOORD1;	// 픽셀마다 시선 벡터를 구하기 위해 월드 위치를 넘긴다
};

VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	output.position = mul(float4(_input.position, 1.0f), gWvp);
	output.normal = mul(float4(_input.normal, 0.0f), gWorld).xyz;	// 법선은 방향이므로 w=0
	output.worldPos = mul(float4(_input.position, 1.0f), gWorld).xyz;
	return output;
}

float4 PSMain(VSOutput _input) : SV_TARGET
{
	float3 N = normalize(_input.normal);	// 보간된 법선은 길이가 1이 아니므로 재정규화
	float3 L = normalize(gLightDir);		// 빛이 나아가는 방향
	float3 V = normalize(gCameraPos - _input.worldPos);	// 픽셀 -> 카메라 시선 벡터

	// 1) 주변광: 어둠 속에서도 형태가 보이게 하는 기본 밝기
	float ambient = 0.15f;

	// 2) 확산광(램버트): 법선이 빛을 마주볼수록 밝다. 음수는 0으로 자른다.
	float diffuse = saturate(dot(N, -L));

	// 3) 정반사광: 모드에 따라 계산이 다르다
	float specular = 0.0f;
	if (gMode == 1)
	{
		// [퐁] 반사 벡터 R과 시선 V의 일치도. 전통적이지만 reflect 계산이 약간 비싸다.
		float3 R = reflect(L, N);
		specular = pow(saturate(dot(R, V)), gSpecPower);
	}
	else if (gMode == 2)
	{
		// [블린-퐁] 빛 방향과 시선의 중간(하프) 벡터 H와 법선의 일치도.
		// 퐁보다 싸고 품질도 좋아서 현대 엔진의 표준이 됐다!
		float3 H = normalize(-L + V);
		specular = pow(saturate(dot(N, H)), gSpecPower);
	}

	// 빛을 등진 면에 하이라이트가 생기는 오류 방지: 확산광이 0이면 정반사도 0
	if (diffuse <= 0.0f)
	{
		specular = 0.0f;
	}

	// 최종색 = (주변광 + 확산광) x 물체색 + 정반사광(흰색 하이라이트)
	float3 color = (ambient + diffuse) * gBaseColor.rgb + specular * float3(1.0f, 1.0f, 1.0f);
	return float4(color, gBaseColor.a);
}
)";
}

//////////////////////////////////////////////////////////////////////////////////////////

// 세 셰이딩 모델의 수식과 차이를 콘솔에 출력한다. (학습용)
void PrintShadingExplanation()
{
	jc::Console::WriteLine("\n[셰이딩(Shading)이란?]");
	jc::Console::WriteLine(" \"빛이 표면에서 어떻게 반사되는가\"를 수식으로 흉내 내는 것.\n");
	jc::Console::WriteLine("[세 가지 성분]");
	jc::Console::WriteLine(" 1. 주변광(Ambient)   : 사방에서 은은히 도달하는 기본 밝기 (상수 0.15)");
	jc::Console::WriteLine(" 2. 확산광(Diffuse)   : max(0, N\xc2\xb7(-L)) - 빛을 마주볼수록 밝다 (램버트 법칙)");
	jc::Console::WriteLine(" 3. 정반사광(Specular): 거울 반사 하이라이트 - 카메라 위치가 필요하다!\n");
	jc::Console::WriteLine("[모델별 정반사 계산]");
	jc::Console::WriteLine(" 1. 램버트   : 정반사 없음. 무광택 표면 (분필, 천)");
	jc::Console::WriteLine(" 2. 퐁       : pow(max(0, R\xc2\xb7V), n), R = reflect(L, N)");
	jc::Console::WriteLine(" 3. 블린-퐁  : pow(max(0, N\xc2\xb7H), n), H = normalize(-L + V)");
	jc::Console::WriteLine("    -> reflect 계산이 빠지고 결과도 자연스러워 현대 엔진의 표준!\n");
	jc::Console::WriteLine("[조작법]");
	jc::Console::WriteLine(" 1/2/3: 셰이딩 모델 전환");
	jc::Console::WriteLine(" 위/아래 방향키: 하이라이트 날카로움(SpecPower) 2~256 증감");
	jc::Console::WriteLine("  -> 값이 클수록 작고 선명한 하이라이트 = 매끈한 표면 느낌\n");
}

//////////////////////////////////////////////////////////////////////////////////////////

// UV 구(Sphere) 지오메트리를 생성해 정점/인덱스 배열을 채운다.
void GenerateSphere(jc::Vector<VertexPNT>& _outVertices, jc::Vector<_u32>& _outIndices,
	_f32 _radius, _s32 _rings, _s32 _segments)
{
	_outVertices.Clear();
	_outIndices.Clear();

	// 정점 생성: 위에서 아래로(위도), 한 바퀴(경도) 돌며 격자를 만든다.
	// 경도는 _segments + 1열: 마지막 열은 첫 열과 위치가 같지만 UV가 다르다. (이음자리 복제)
	for (_s32 ring = 0; ring <= _rings; ++ring)
	{
		// 위도각: 0(북극) ~ 파이(남극)
		const _f32 phi = jc_math_pi * (_f32)ring / (_f32)_rings;
		const _f32 sinPhi = sinf(phi);
		const _f32 cosPhi = cosf(phi);

		for (_s32 seg = 0; seg <= _segments; ++seg)
		{
			// 경도각: 0 ~ 2파이
			const _f32 theta = jc_math_pi2 * (_f32)seg / (_f32)_segments;

			// 구면 좌표 -> 직교 좌표. 반지름 1짜리 단위 방향이 곧 법선이 된다!
			VertexPNT vertex;
			vertex.normal_ = vec3(sinPhi * cosf(theta), cosPhi, sinPhi * sinf(theta));
			vertex.position_ = vertex.normal_ * _radius;
			vertex.uv_ = vec2((_f32)seg / (_f32)_segments, (_f32)ring / (_f32)_rings);
			_outVertices.PushBack(vertex);
		}
	}

	// 인덱스 생성: 격자 한 칸을 삼각형 2개로 분할 (밖에서 볼 때 시계 방향)
	const _s32 columns = _segments + 1;
	for (_s32 ring = 0; ring < _rings; ++ring)
	{
		for (_s32 seg = 0; seg < _segments; ++seg)
		{
			const _u32 i0 = (_u32)(ring * columns + seg);			// 왼위
			const _u32 i1 = i0 + 1;									// 오른위
			const _u32 i2 = i0 + (_u32)columns;						// 왼아래
			const _u32 i3 = i2 + 1;									// 오른아래

			_outIndices.PushBack(i0);
			_outIndices.PushBack(i1);
			_outIndices.PushBack(i2);

			_outIndices.PushBack(i2);
			_outIndices.PushBack(i1);
			_outIndices.PushBack(i3);
		}
	}
}
