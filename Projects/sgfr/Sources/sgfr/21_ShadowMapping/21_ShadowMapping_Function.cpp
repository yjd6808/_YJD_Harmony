/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:40:00 PM
 * =====================
 * 21. 그림자 매핑 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/21_ShadowMapping/21_ShadowMapping_Function.h"

using namespace sgf;
using namespace jc;

// [패스 1] 빛 시점에서 깊이만 기록하는 HLSL 셰이더 소스를 반환한다.
//
// [왜 깊이만 기록하나?]
//  그림자 판정에 필요한 건 "빛이 각 방향으로 얼마나 멀리까지 닿는가" 뿐이다.
//  색은 필요 없으므로 렌더 타깃(RTV) 없이 깊이 버퍼(DSV)만 묶어 그린다.
const char* ShadowDepthShaderSource()
{
	return R"(
cbuffer CbDepth : register(b0)
{
	row_major float4x4 gLightWvp;	// 월드 x 빛의 뷰 x 빛의 투영
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
};

VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	// 빛 시점으로 투영하면 래스터라이저가 알아서 깊이를 깊이 버퍼에 적는다.
	output.position = mul(float4(_input.position, 1.0f), gLightWvp);
	return output;
}

float4 PSMain(VSOutput _input) : SV_TARGET
{
	// 렌더 타깃이 없으므로 이 출력은 버려진다. 깊이만 자동으로 기록된다!
	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
)";
}

// [패스 2] 그림자 맵을 비교해 그늘진 곳을 어둡게 그리는 HLSL 셰이더 소스를 반환한다.
//
// [Before/After 분할]
//  픽셀의 화면 x좌표(SV_POSITION)가 gSplitPixelX보다 왼쪽이면
//  그림자 판정을 건너뛰어 "그림자 없는 세상(Before)"을 보여준다.
//  오른쪽은 그림자 판정을 수행(After). 같은 장면에서 그림자의 유무만 비교된다!
const char* ShadowSceneShaderSource()
{
	return R"(
Texture2D gShadowMap : register(t0);	// 패스 1에서 기록한 빛 시점 깊이
SamplerState gShadowSampler : register(s0);

cbuffer CbScene : register(b0)
{
	row_major float4x4 gWorld;			// 모델 -> 월드
	row_major float4x4 gWvp;			// 카메라 기준 합성 행렬
	row_major float4x4 gLightViewProj;	// 빛 기준 뷰 x 투영
};

cbuffer CbLight : register(b1)
{
	float3 gLightDir;		// 빛이 나아가는 방향
	float  gShadowBias;		// 그림자 여드름 방지용 보정값
	float4 gBaseColor;		// 물체 기본색
	float  gSplitPixelX;	// Before/After 경계 픽셀 x (왼쪽=그림자 없음, 오른쪽=그림자 적용)
	float3 gPadding;		// 16바이트 정렬용 여백
};

struct VSInput
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float2 uv       : TEXCOORD0;
};

struct VSOutput
{
	float4 position      : SV_POSITION;
	float3 normal        : NORMAL;
	float4 lightSpacePos : TEXCOORD1;	// 빛 시점에서 본 이 정점의 위치
};

VSOutput VSMain(VSInput _input)
{
	VSOutput output;
	float4 worldPos = mul(float4(_input.position, 1.0f), gWorld);
	output.position = mul(float4(_input.position, 1.0f), gWvp);
	output.normal = mul(float4(_input.normal, 0.0f), gWorld).xyz;
	// 같은 점을 빛 시점으로도 투영해 둔다. (픽셀 셰이더에서 그림자 판정에 사용)
	output.lightSpacePos = mul(worldPos, gLightViewProj);
	return output;
}

float4 PSMain(VSOutput _input) : SV_TARGET
{
	// [경계선] Before/After를 나누는 노란 세로선
	if (abs(_input.position.x - gSplitPixelX) < 1.5f)
	{
		return float4(1.0f, 0.85f, 0.2f, 1.0f);
	}

	float3 N = normalize(_input.normal);
	float3 L = normalize(gLightDir);

	// 1) 기본 라이팅 (10번의 램버트 확산광)
	float diffuseAmount = saturate(dot(N, -L));

	// 2) 그림자 판정
	float shadow = 1.0f;	// 1 = 빛을 받음, 0.35 = 그늘짐

	// [After] 경계 오른쪽만 그림자 판정을 수행한다.
	// [Before] 왼쪽은 이 블록을 건너뛰므로 항상 shadow = 1 (그림자 없는 세상!)
	if (_input.position.x >= gSplitPixelX)
	{
		// 투영 좌표 -> NDC (원근 투영이면 w 나누기가 필수. 직교는 w=1이지만 공통 처리)
		float3 ndc = _input.lightSpacePos.xyz / _input.lightSpacePos.w;

		// NDC(-1~+1) -> 텍스처 UV(0~1). 화면 y는 위가 +, 텍스처 v는 아래가 + 이므로 y는 뒤집는다.
		float2 shadowUv = float2(ndc.x * 0.5f + 0.5f, -ndc.y * 0.5f + 0.5f);

		if (shadowUv.x >= 0.0f && shadowUv.x <= 1.0f &&
			shadowUv.y >= 0.0f && shadowUv.y <= 1.0f &&
			ndc.z >= 0.0f && ndc.z <= 1.0f)
		{
			// 그림자 맵에 기록된 "빛이 처음 닿은 깊이"
			float storedDepth = gShadowMap.Sample(gShadowSampler, shadowUv).r;

			// 내 깊이가 기록된 깊이보다 더 멀다 = 내 앞에 뭔가 있다 = 그림자!
			// 바이어스를 빼서 자기 자신의 깊이와 오차로 생기는 줄무늬(그림자 여드름)를 막는다.
			if (ndc.z - gShadowBias > storedDepth)
			{
				shadow = 0.35f;
			}
		}
	}

	// 3) 최종색 = 주변광 + 확산광 x 그림자 계수
	float3 color = 0.15f * gBaseColor.rgb + diffuseAmount * shadow * gBaseColor.rgb;
	return float4(color, gBaseColor.a);
}
)";
}

// 그림자 매핑의 2패스 원리와 그림자 여드름(acne)/바이어스를 콘솔에 출력한다. (학습용)
void PrintShadowExplanation()
{
	printf("\n[그림자 매핑(Shadow Mapping)의 핵심 아이디어]\n");
	printf(" \"빛의 입장에서 보이지 않는 곳이 그림자다!\"\n\n");
	printf("[Before/After 비교 뷰]\n");
	printf(" 노란 세로선 왼쪽  = 그림자 없음(Before): 라이팅만 있는 세상\n");
	printf(" 노란 세로선 오른쪽 = 그림자 적용(After): 큐브의 그림자가 바닥에 드리워진다\n");
	printf(" 그림자 하나가 공간감을 얼마나 바꾸는지 눈으로 비교해보자!\n\n");
	printf("[2패스 구조]\n");
	printf(" 패스 1: 카메라를 빛의 위치에 놓고 장면의 '깊이만' 텍스처에 기록 (그림자 맵)\n");
	printf(" 패스 2: 보통처럼 그리되, 각 픽셀을 빛 시점으로 재투영해서 비교:\n");
	printf("   '내 깊이 > 그림자 맵의 깊이' 이면 내 앞에 뭔가 있다 = 그늘진 곳!\n\n");
	printf("[그림자 여드름(Shadow Acne)과 바이어스]\n");
	printf(" 그림자 맵 해상도의 한계로 자기 자신이 자기를 가린다고 오판하면 줄무늬가 생긴다.\n");
	printf(" 깊이 비교에 작은 여유(바이어스)를 두면 해결! (단, 크면 그림자가 물체에서 분리된다)\n\n");
	printf("[조작법]\n");
	printf(" 왼쪽/오른쪽 방향키: 태양(빛) 방향 회전 -> 오른쪽 화면의 그림자가 따라 움직인다!\n");
	printf(" 위/아래 방향키: 바이어스 조절 -> 오른쪽에서만 줄무늬 현상을 관찰할 수 있다\n\n");
}

// 면별 법선을 가진 정육면체 정점 24개와 인덱스 36개를 채운다. (10번과 동일 패턴)
void FillShadowCube(VertexPNT* _pOutVertices24, _u32* _pOutIndices36)
{
	int vertexBase = 0;
	int indexBase = 0;

	// 한 면(사각형)을 추가하는 보조 람다: 중심 방향 법선과 두 접선 벡터로 4점을 만든다.
	auto AddFace = [&](const Vec3& _normal, const Vec3& _up, const Vec3& _right)
	{
		const Vec3 center = _normal * 0.5f;

		// 왼위 -> 오른위 -> 왼아래 -> 오른아래 (밖에서 볼 때 시계 방향)
		_pOutVertices24[vertexBase + 0] = { center + (_up * 0.5f) - (_right * 0.5f), _normal, Vec2(0.0f, 0.0f) };
		_pOutVertices24[vertexBase + 1] = { center + (_up * 0.5f) + (_right * 0.5f), _normal, Vec2(1.0f, 0.0f) };
		_pOutVertices24[vertexBase + 2] = { center - (_up * 0.5f) - (_right * 0.5f), _normal, Vec2(0.0f, 1.0f) };
		_pOutVertices24[vertexBase + 3] = { center - (_up * 0.5f) + (_right * 0.5f), _normal, Vec2(1.0f, 1.0f) };

		_pOutIndices36[indexBase + 0] = vertexBase + 0;
		_pOutIndices36[indexBase + 1] = vertexBase + 1;
		_pOutIndices36[indexBase + 2] = vertexBase + 2;
		_pOutIndices36[indexBase + 3] = vertexBase + 2;
		_pOutIndices36[indexBase + 4] = vertexBase + 1;
		_pOutIndices36[indexBase + 5] = vertexBase + 3;

		vertexBase += 4;
		indexBase += 6;
	};

	AddFace(Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));		// 앞면   (-Z)
	AddFace(Vec3(0.0f, 0.0f, +1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));	// 뒷면   (+Z)
	AddFace(Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));	// 왼면   (-X)
	AddFace(Vec3(+1.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 0.0f, +1.0f));	// 오른면 (+X)
	AddFace(Vec3(0.0f, +1.0f, 0.0f), Vec3(0.0f, 0.0f, +1.0f), Vec3(1.0f, 0.0f, 0.0f));	// 윗면   (+Y)
	AddFace(Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));	// 아랫면 (-Y)
}

// 위를 바라보는 바닥 평면(사각형) 정점 4개와 인덱스 6개를 채운다.
void FillGroundPlane(VertexPNT* _pOutVertices4, _u32* _pOutIndices6, _f32 _halfSize)
{
	const Vec3 up = Vec3::Up();

	// 위에서 내려다볼 때 시계 방향이 되도록: 멀리-왼 → 멀리-오른 → 가까이-왼 → 가까이-오른
	_pOutVertices4[0] = { Vec3(-_halfSize, 0.0f, +_halfSize), up, Vec2(0.0f, 0.0f) };
	_pOutVertices4[1] = { Vec3(+_halfSize, 0.0f, +_halfSize), up, Vec2(1.0f, 0.0f) };
	_pOutVertices4[2] = { Vec3(-_halfSize, 0.0f, -_halfSize), up, Vec2(0.0f, 1.0f) };
	_pOutVertices4[3] = { Vec3(+_halfSize, 0.0f, -_halfSize), up, Vec2(1.0f, 1.0f) };

	_pOutIndices6[0] = 0; _pOutIndices6[1] = 1; _pOutIndices6[2] = 2;
	_pOutIndices6[3] = 2; _pOutIndices6[4] = 1; _pOutIndices6[5] = 3;
}
