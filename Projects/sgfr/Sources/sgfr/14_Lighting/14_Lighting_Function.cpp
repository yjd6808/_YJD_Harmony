/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:40:00 AM
 * =====================
 * 14. 라이팅 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/14_Lighting/14_Lighting_Function.h"

using namespace sgf;
using namespace jc;

// 람버트 확산광 계산이 들어간 HLSL 셰이더 소스를 반환한다.
//
// [람버트 법칙 (Lambert's Cosine Law)]
//  표면이 받는 빛의 양 = 법선과 빛 방향이 이루는 각도의 코사인.
//  코사인은 내적(dot)으로 구한다! (01번 선형대수에서 배운 내적의 실전 활용)
//  빛을 정면으로 받는 면은 밝고(cos 0도 = 1), 비스듬히 받는 면은 어둡다(cos 90도 = 0).
const char* LambertShaderSource()
{
	return R"(
cbuffer CbTransform : register(b0)
{
	row_major float4x4 gWorld;			// 월드 행렬 (법선 변환에 별도로 필요)
	row_major float4x4 gWorldViewProj;	// 월드 x 뷰 x 투영 결합 행렬
};

cbuffer CbLight : register(b1)
{
	float3 gLightDir;		// 빛이 나아가는 방향 (정규화된 벡터)
	float  gAmbient;		// 주변광 세기 (0~1)
	float4 gBaseColor;		// 물체 기본 색상
};

struct VSInput
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float2 uv       : TEXCOORD0;
};

struct VSOutput
{
	float4 position    : SV_POSITION;
	float3 worldNormal : NORMAL;		// 월드 공간으로 변환된 법선
};

VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	output.position = mul(float4(_input.position, 1.0f), gWorldViewProj);

	// 법선은 '방향'이므로 w = 0으로 변환한다. (이동 성분 무시)
	// 주의: 비균등 스케일이 있으면 역전치 행렬이 필요하지만,
	//       이 튜토리얼은 균등 스케일만 쓰므로 월드 행렬을 그대로 써도 된다.
	output.worldNormal = normalize(mul(float4(_input.normal, 0.0f), gWorld).xyz);
	return output;
}

float4 PSMain(VSOutput _input) : SV_TARGET
{
	// 보간을 거치면 길이가 1이 아닐 수 있으므로 다시 정규화한다.
	float3 normal = normalize(_input.worldNormal);

	// === 람버트 확산광의 핵심 한 줄! ===
	// -gLightDir: 표면에서 빛을 향하는 방향으로 뒤집는다.
	// saturate: 음수(반대쪽을 보는 면)를 0으로 잘라낸다.
	float diffuse = saturate(dot(normal, -gLightDir));

	// 최종 밝기 = 주변광(최소 밝기 보장) + 확산광
	float lighting = saturate(gAmbient + diffuse * (1.0f - gAmbient));
	return float4(gBaseColor.rgb * lighting, gBaseColor.a);
}
)";
}

// 람버트 법칙의 원리를 콘솔에 출력한다. (학습용)
void PrintLambertExplanation()
{
	printf("\n[람버트 확산광 (Lambert Diffuse)]\n");
	printf(" 밝기 = max(0, dot(법선, -빛방향))\n");
	printf(" - 법선(Normal): 면이 바라보는 방향의 단위 벡터\n");
	printf(" - 빛을 정면으로 받으면 1 (가장 밝음), 수직이면 0 (안 받음)\n");
	printf(" - 내적 하나로 계산되는 가장 기초적이면서 핵심적인 조명 모델\n");
	printf(" - 주변광(Ambient): 완전한 암흑을 막기 위한 최소 밝기 (간접광 흉내)\n\n");
}

// 법선이 포함된 정육면체 정점 24개와 인덱스 36개를 채워준다.
// 꼭짓점은 8개지만 면마다 법선이 다르므로 면별로 정점을 분리해야 한다. (4개 x 6면 = 24개)
void FillCubeWithNormals(VertexPNT* _pOutVertices24, _u32* _pOutIndices36)
{
	// 면 하나를 채우는 보조 람다: 중심 방향(법선)과 가로/세로 축을 받아 4정점 생성
	int v = 0;
	int i = 0;
	auto AddFace = [&](const vec3& _normal, const vec3& _right, const vec3& _up)
	{
		const vec3 center = _normal * 0.5f;	// 면의 중심은 법선 방향으로 0.5 떨어져 있다

		// 사각형 4정점 (면 바깥에서 볼 때 왼위 -> 오른위 -> 왼아래 -> 오른아래)
		_pOutVertices24[v + 0] = { center + (_up - _right) * 0.5f, _normal, vec2(0.0f, 0.0f) };
		_pOutVertices24[v + 1] = { center + (_up + _right) * 0.5f, _normal, vec2(1.0f, 0.0f) };
		_pOutVertices24[v + 2] = { center - (_up + _right) * 0.5f, _normal, vec2(0.0f, 1.0f) };
		_pOutVertices24[v + 3] = { center - (_up - _right) * 0.5f, _normal, vec2(1.0f, 1.0f) };

		// 삼각형 2개 (바깥에서 볼 때 시계 방향)
		_pOutIndices36[i + 0] = v + 0; _pOutIndices36[i + 1] = v + 1; _pOutIndices36[i + 2] = v + 2;
		_pOutIndices36[i + 3] = v + 2; _pOutIndices36[i + 4] = v + 1; _pOutIndices36[i + 5] = v + 3;

		v += 4;
		i += 6;
	};

	// 6개 면: 법선과 그 면의 오른쪽/위 축을 지정한다.
	// (바깥에서 봤을 때 시계 방향이 되도록 축 방향을 골랐다)
	AddFace(vec3(0.0f, 0.0f, -1.0f), vec3(+1.0f, 0.0f, 0.0f), vec3(0.0f, +1.0f, 0.0f));	// 앞면   (z-)
	AddFace(vec3(0.0f, 0.0f, +1.0f), vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, +1.0f, 0.0f));	// 뒷면   (z+)
	AddFace(vec3(0.0f, +1.0f, 0.0f), vec3(+1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, +1.0f));	// 윗면   (y+)
	AddFace(vec3(0.0f, -1.0f, 0.0f), vec3(+1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));	// 아랫면 (y-)
	AddFace(vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, +1.0f, 0.0f));	// 왼면   (x-)
	AddFace(vec3(+1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, +1.0f), vec3(0.0f, +1.0f, 0.0f));	// 오른면 (x+)
}
