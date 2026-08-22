/*
 * 작성자: 윤정도
 * 생성일: 8/22/2026
 * =====================
 * 34. GraphicsEnum 한눈에 비교 (Before/After)
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. GraphicsEnums.h의 렌더 상태가 화면에 어떻게 달라 보이는지 한 화면에서 비교한다.
 *    - 왼쪽(Before) = sgf 엔진 디폴트값으로 고정
 *    - 오른쪽(After) = 현재 선택한 enum값 (방향키 Left/Right로 실시간 교체)
 * 2. 상태는 Draw 호출 사이에 끼워 교체한다. 같은 지오메트리라도 상태만 바꾸면 전혀 다르게 보인다.
 * 3. 각 enum의 D3D11 매핑과 쓰임새를 콘솔 설명과 함께 익힌다.
 *
 * [다루는 카테고리 8종]
 * 1 BlendMode      (4) - 픽셀 합성 공식
 * 2 FilterMode     (3) - 텍스처 확대/축소 보간
 * 3 AddressMode    (4) - UV 0~1 밖 처리
 * 4 CullMode       (3) - 어느 면을 버릴지
 * 5 FillMode       (2) - 면 채우기 vs 선
 * 6 DepthMode      (3) - 깊이 테스트/쓰기
 * 7 FrontFace      (2) - 앞면 판정 기준
 * 8 Topology       (5) - 정점 연결 방식
 *
 * [Before/After 비교 뷰]
 * - 화면 중앙 세로 경계선 기준 좌=디폴트, 우=선택값
 * - Blend/Cull/Fill/Depth/FrontFace/Topology는 큐브/도형을 좌우 두 번 그려 비교
 * - Filter/Address는 한 장의 텍스처를 s0(디폴트)와 s1(선택) 두 샘플러로 나눠 읽고,
 *   픽셀 셰이더가 화면 x좌표로 어느 샘플러를 쓸지 골라 한 번에 비교한다. (17번 기법)
 *
 * [조작법]
 * - 1~8 또는 Up/Down : 카테고리 전환
 * - Left/Right       : After 값 순회 (파이프라인에 즉시 반영)
 * - ESC              : 종료
 */

#include "Core.h"
#include "sgfr/Tutorial/34_GraphicsEnum_한눈에_비교/34_GraphicsEnumDiff_Main.h"
#include "sgfr/Common/TutorialCommon.h"

using namespace sgf;
using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
// 카테고리 정의
// - DiffCategory 한 값이 곧 콘솔 1~8번과 대응한다.
// - 각 카테고리의 After 값은 방향키로 순회하고, Before 값은 항상 엔진 디폴트로 고정한다.

enum class DiffCategory
{
	dcBlendMode = 0,	// 블렌드 합성
	dcFilterMode,		// 샘플러 필터
	dcAddressMode,		// 샘플러 주소
	dcCullMode,			// 컬링
	dcFillMode,			// 채우기
	dcDepthMode,		// 깊이
	dcFrontFace,		// 앞면 판정
	dcTopology,			// 토폴로지
	Max
};

namespace
{
	////////////////////////////////////////////////////////////////////////////////////////
	// 엔진 디폴트 인덱스 (Before 고정)
	// - 이 값들은 GraphicDevice::Initialize가 깔아두는 기본 상태와 동일하다.

	const _s32 DEFAULT_INDEX[] =
	{
		1,	// Blend     bmAlpha      (일반 반투명)
		1,	// Filter    fmLinear     (선형 보간)
		2,	// Address   amClamp      (가장자리 고정)
		2,	// Cull      cmBack       (뒷면 제거)
		0,	// Fill      fmSolid      (면 채우기)
		1,	// Depth     dmReadWrite  (읽기+쓰기)
		0,	// FrontFace ffClockwise  (시계=앞면)
		3,	// Topology  ptTriangleList (삼각형 목록)
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// 카테고리별 항목 수

	const _s32 COUNT_PER_CATEGORY[] =
	{
		4,	// Blend
		3,	// Filter
		4,	// Address
		3,	// Cull
		2,	// Fill
		3,	// Depth
		2,	// FrontFace
		5,	// Topology
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// 화면 타이틀에 표시할 카테고리 이름

	const char* CATEGORY_NAMES[] =
	{
		"BlendMode",
		"FilterMode",
		"AddressMode",
		"CullMode",
		"FillMode",
		"DepthMode",
		"FrontFace",
		"Topology",
	};

	const char* CATEGORY_NAMES_KO[] =
	{
		"블렌드",
		"필터",
		"주소모드",
		"컬링",
		"채우기",
		"깊이",
		"앞면판정",
		"토폴로지",
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// 각 카테고리 enum 이름표 (타이틀/콘솔에 그대로 보여준다)

	const char* BLEND_NAMES[]   = { "bmNone(덮어쓰기)", "bmAlpha(반투명)", "bmAdd(가산)", "bmMultiply(곱셈)" };
	const char* FILTER_NAMES[]  = { "fmPoint(최근접)", "fmLinear(선형)", "fmAnisotropic(비등방)" };
	const char* ADDRESS_NAMES[] = { "amWrap(반복)", "amMirror(거울)", "amClamp(고정)", "amBorder(테두리색)" };
	const char* CULL_NAMES[]    = { "cmNone(양면)", "cmFront(앞면제거)", "cmBack(뒷면제거)" };
	const char* FILL_NAMES[]    = { "fmSolid(면)", "fmWireframe(선)" };
	const char* DEPTH_NAMES[]   = { "dmDisabled(끄기)", "dmReadWrite(읽기+쓰기)", "dmReadOnly(읽기만)" };
	const char* FRONT_FACE_NAMES[]   = { "ffClockwise(시계=앞)", "ffCounterClockwise(반시계=앞)" };
	const char* TOPOLOGY_NAMES[]    = { "ptPointList(점)", "ptLineList(선분)", "ptLineStrip(연결선)", "ptTriangleList(삼각형)", "ptTriangleStrip(띠)" };

	////////////////////////////////////////////////////////////////////////////////////////
	// 실제 엔진 enum값 매핑
	// - 화면에서 고른 인덱스를 실제 D3D 상태로 바꾸는 테이블이다.

	const BlendMode BLEND_VALUES[] = { BlendMode::bmNone, BlendMode::bmAlpha, BlendMode::bmAdd, BlendMode::bmMultiply };
	const FilterMode FILTER_VALUES[] = { FilterMode::fmPoint, FilterMode::fmLinear, FilterMode::fmAnisotropic };
	const AddressMode ADDRESS_VALUES[] = { AddressMode::amWrap, AddressMode::amMirror, AddressMode::amClamp, AddressMode::amBorder };
	const CullMode CULL_VALUES[] = { CullMode::cmNone, CullMode::cmFront, CullMode::cmBack };
	const FillMode FILL_VALUES[] = { FillMode::fmSolid, FillMode::fmWireframe };
	const DepthMode DEPTH_VALUES[] = { DepthMode::dmDisabled, DepthMode::dmReadWrite, DepthMode::dmReadOnly };
	const FrontFace FRONT_FACE_VALUES[] = { FrontFace::ffClockwise, FrontFace::ffCounterClockwise };
	const PrimitiveTopology TOPOLOGY_VALUES[] = { PrimitiveTopology::ptPointList, PrimitiveTopology::ptLineList, PrimitiveTopology::ptLineStrip, PrimitiveTopology::ptTriangleList, PrimitiveTopology::ptTriangleStrip };

	////////////////////////////////////////////////////////////////////////////////////////
	// 셰이더 소스 - 색 변환 (큐브/깊이/토폴로지용)
	// - 정점 색을 그대로 출력하고, 행렬(gWvp)로 월드->화면 변환만 한다.

	const char* ColorTransformShaderSource34()
	{
		return R"(
cbuffer CbTransform : register(b0)
{
	row_major float4x4 gWvp; // 월드 x 뷰 x 투영 합성 행렬
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
	output.position = mul(float4(_input.position, 1.0f), gWvp);
	output.color    = _input.color;
	return output;
}

float4 PSMain(VSOutput _input) : SV_TARGET
{
	return _input.color;
}
)";
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// 셰이더 소스 - 블렌드 쿼드 (부드러운 원)
	// - 흰 원 텍스처 x 정점 색으로 색입힌 원을 만든다.
	// - 원 밖(알파 0)은 흰색(변화 없음)으로 섞어 곱셈 모드에서 검은 사각형이 안 보이게 한다.

	const char* BlendQuadShaderSource34()
	{
		return R"(
Texture2D    gTexture : register(t0);
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
	output.position = float4(_input.position, 1.0f);
	output.uv       = _input.uv;
	output.color    = _input.color;
	return output;
}

float4 PSMain(VSOutput _input) : SV_TARGET
{
	float4 tex = gTexture.Sample(gSampler, _input.uv) * _input.color;

	// 원 밖(알파 0)은 흰색(변화 없음)으로 섞어 곱셈 모드에서 검은 사각형이 안 보이게 한다.
	return float4(lerp(float3(1.0f, 1.0f, 1.0f), tex.rgb, tex.a), tex.a);
}
)";
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// 셰이더 소스 - 샘플러 분할 비교 (Filter/Address용)
	// - s0=Before(디폴트), s1=After(선택) 두 샘플러로 같은 텍스처를 읽는다.
	// - 픽셀의 화면 x좌표가 경계(gSplitPixelX)보다 왼쪽이면 s0, 오른쪽이면 s1을 쓴다.
	// - 덕분에 한 번의 Draw로 좌우가 다른 샘플링 결과를 비교할 수 있다.

	const char* SamplerSplitShaderSource34()
	{
		return R"(
Texture2D    gTexture        : register(t0);
SamplerState gSamplerBefore  : register(s0); // Before(디폴트) 샘플러
SamplerState gSamplerAfter   : register(s1); // After(선택)  샘플러

cbuffer CbSplit : register(b0)
{
	float  gSplitPixelX; // 좌/우를 나누는 경계의 픽셀 x좌표
	float3 gPad;         // 16바이트 정렬용 여백
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
	output.position = float4(_input.position, 1.0f);
	output.uv       = _input.uv;
	output.color    = _input.color;
	return output;
}

float4 PSMain(VSOutput _input) : SV_TARGET
{
	// 경계선은 노란 세로선으로 눈에 띄게 표시
	if (abs(_input.position.x - gSplitPixelX) < 1.5f)
	{
		return float4(1.0f, 0.85f, 0.2f, 1.0f);
	}

	// 화면 x좌표가 경계보다 왼쪽이면 Before, 오른쪽이면 After 샘플러로 같은 텍스처를 읽는다.
	if (_input.position.x < gSplitPixelX)
	{
		return gTexture.Sample(gSamplerBefore, _input.uv) * _input.color;
	}
	return gTexture.Sample(gSamplerAfter, _input.uv) * _input.color;
}
)";
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// 상수 버퍼 구조체
	// - HLSL의 cbuffer와 메모리 배치가 정확히 일치해야 한다. (16바이트 배수)

	struct CbTransform
	{
		mat4 wvp_;	// 월드 x 뷰 x 투영 합성 행렬
	};

	struct CbSplit
	{
		_f32 splitPixelX_;	// 좌/우를 나누는 경계의 픽셀 x좌표
		_f32 pad_[3];		// 16바이트 정렬용 여백
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// 부드러운 원 텍스처를 채운다.
	// - 중심은 불투명 흰색, 가장자리로 갈수록 알파가 0으로 부드럽게 줄어든다.
	// - 색은 흰색 고정: 실제 색은 정점 색으로 입힌다. (텍스처 재활용)

	void FillSoftCirclePixels(_u8* _pOutPixels, _s32 _size)
	{
		const _f32 center = (_size - 1) * 0.5f;
		const _f32 maxDist = center;

		for (_s32 y = 0; y < _size; ++y)
		{
			for (_s32 x = 0; x < _size; ++x)
			{
				_f32 dx = x - center;
				_f32 dy = y - center;
				_f32 dist = sqrtf(dx * dx + dy * dy) / maxDist;

				_f32 alpha = 1.0f;
				if (dist > 0.7f)
				{
					alpha = Clamp(1.0f - (dist - 0.7f) / 0.3f, 0.0f, 1.0f);
				}

				_u8* pPixel = _pOutPixels + (y * _size + x) * 4;
				pPixel[0] = 255;
				pPixel[1] = 255;
				pPixel[2] = 255;
				pPixel[3] = static_cast<_u8>(alpha * 255.0f);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// 체커보드 텍스처를 채운다.
	// - 가장자리 1픽셀은 빨간 테두리: Clamp에서 테두리가 늘어나는 것을 보기 위해서다.

	void FillCheckerPixels(_u8* _pOutPixels, _s32 _width, _s32 _height, _s32 _cellSize)
	{
		for (_s32 y = 0; y < _height; ++y)
		{
			for (_s32 x = 0; x < _width; ++x)
			{
				bool isBorder = (x == 0 || y == 0 || x == _width - 1 || y == _height - 1);

				_u8 r, g, b;
				if (isBorder)
				{
					r = 220; g = 60; b = 60;
				}
				else
				{
					bool isLight = (((x / _cellSize) + (y / _cellSize)) % 2) == 0;
					if (isLight) { r = 240; g = 240; b = 240; }
					else         { r = 40;  g = 70;  b = 160; }
				}

				_u8* pPixel = _pOutPixels + (y * _width + x) * 4;
				pPixel[0] = r;
				pPixel[1] = g;
				pPixel[2] = b;
				pPixel[3] = 255;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// UV가 0~_uvScale인 화면 덮개 사각형을 채운다.
	// - _uvScale이 1보다 크면 UV가 0~1을 벗어나 주소 모드 차이를 관찰할 수 있다.

	void FillUvQuad(VertexPTC* _pOutVertices4, _u32* _pOutIndices6, _f32 _uvScale)
	{
		const color white = color::WHITE;

		// NDC 좌표로 화면 대부분을 덮는 사각형
		_pOutVertices4[0] = { vec3(-0.85f, +0.85f, 0.0f), vec2(0.0f, 0.0f), white };
		_pOutVertices4[1] = { vec3(+0.85f, +0.85f, 0.0f), vec2(_uvScale, 0.0f), white };
		_pOutVertices4[2] = { vec3(-0.85f, -0.85f, 0.0f), vec2(0.0f, _uvScale), white };
		_pOutVertices4[3] = { vec3(+0.85f, -0.85f, 0.0f), vec2(_uvScale, _uvScale), white };

		_pOutIndices6[0] = 0; _pOutIndices6[1] = 1; _pOutIndices6[2] = 2;
		_pOutIndices6[3] = 2; _pOutIndices6[4] = 1; _pOutIndices6[5] = 3;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// NDC 기준 사각형 정점 4개를 채운다. (UV 0~1, 지정 색)

	void FillQuadVertices(VertexPTC* _pOutVertices4, const vec2& _center, _f32 _halfSize, const color& _color)
	{
		_pOutVertices4[0] = { vec3(_center.x - _halfSize, _center.y + _halfSize, 0.0f), vec2(0.0f, 0.0f), _color };
		_pOutVertices4[1] = { vec3(_center.x + _halfSize, _center.y + _halfSize, 0.0f), vec2(1.0f, 0.0f), _color };
		_pOutVertices4[2] = { vec3(_center.x - _halfSize, _center.y - _halfSize, 0.0f), vec2(0.0f, 1.0f), _color };
		_pOutVertices4[3] = { vec3(_center.x + _halfSize, _center.y - _halfSize, 0.0f), vec2(1.0f, 1.0f), _color };
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// 콘솔 가이드 출력
	// - 현재 카테고리의 의미와 각 enum값의 D3D 매핑을 초심자 눈높이로 설명한다.

	void PrintGuide(DiffCategory _category, _s32 _afterIndex)
	{
		Console::WriteLine("\n==================================================");
		Console::WriteLine(" 34. GraphicsEnum 한눈에 비교 (Before=디폴트 / After=선택)");
		Console::WriteLine("==================================================");
		Console::WriteLine(" 좌(Before)=엔진 디폴트 고정, 우(After)=선택값. 같은 장면을 두 번 그려 비교합니다.");
		Console::WriteLine("");
		Console::WriteLine("[조작법]");
		Console::WriteLine(" 1~8 / Up,Down : 카테고리 전환");
		Console::WriteLine("    1 Blend(4)  2 Filter(3)  3 Address(4)  4 Cull(3)");
		Console::WriteLine("    5 Fill(2)   6 Depth(3)   7 FrontFace(2) 8 Topology(5)");
		Console::WriteLine(" Left,Right    : After 값 순회 (파이프라인 즉시 반영)");
		Console::WriteLine(" ESC            : 종료");
		Console::WriteLine("");
		Console::WriteLine("[엔진 디폴트(Before 고정)]");
		Console::WriteLine(" Blend bmAlpha / Filter fmLinear / Address amClamp / Cull cmBack");
		Console::WriteLine(" Fill fmSolid / Depth dmReadWrite / Front ffClockwise / Topo ptTriangleList");
		Console::WriteLine("");

		Console::WriteLine("[현재 카테고리]");
		char buf[128];
		sprintf_s(buf, " %d. %s", static_cast<_s32>(_category) + 1, CATEGORY_NAMES[static_cast<_s32>(_category)]);
		Console::WriteLine(buf);
		Console::WriteLine("");

		if (_category == DiffCategory::dcBlendMode)
		{
			Console::WriteLine("[BlendMode - 픽셀 합성 공식: 최종색 = 새색*Src + 기존색*Dest]");
			Console::WriteLine(" bmNone    : BlendEnable FALSE (ONE/ZERO) - 덮어쓰기 불투명");
			Console::WriteLine(" bmAlpha   : SRC_ALPHA / INV_SRC_ALPHA - 일반 반투명 (UI, 스프라이트) [디폴트]");
			Console::WriteLine(" bmAdd     : SRC_ALPHA / ONE - 가산, 겹칠수록 밝아짐 (빛, 이펙트)");
			Console::WriteLine(" bmMultiply: DEST_COLOR / ZERO - 곱셈, 겹칠수록 어두워짐 (그림자)");
		}
		else 		if (_category == DiffCategory::dcFilterMode)
		{
			Console::WriteLine("[FilterMode - 확대/축소 시 보간 규칙]");
			Console::WriteLine(" fmPoint      : MIN_MAG_MIP_POINT - 가장 가까운 1픽셀 그대로 (도트, 계단 현상, 확대 시 블록 느낌)");
			Console::WriteLine(" fmLinear     : MIN_MAG_MIP_LINEAR - 주변 4픽셀을 섞어 부드럽게 [디폴트] (정면 평면에서는 Anisotropic과 동일)");
			Console::WriteLine(" fmAnisotropic: ANISOTROPIC 16x - 비스듬한 면(복도 바닥, 먼 벽)에서 방향을 따라 최대 16개 샘플을 모아 선명도 유지");
			Console::WriteLine("  -> 정면 체커(현재 화면)는 Linear와 Anisotropic이 거의 같아 보인다. 차이는 멀리 비스듬히 보이는 바닥/벽에서만 드러난다.");
		}
		else if (_category == DiffCategory::dcAddressMode)
		{
			Console::WriteLine("[AddressMode - UV 0~1 벗어날 때]");
			Console::WriteLine(" amWrap  : 반복 타일링");
			Console::WriteLine(" amMirror: 거울 뒤집어 반복");
			Console::WriteLine(" amClamp : 가장자리 색 고정 [디폴트]");
			Console::WriteLine(" amBorder: BorderColor(검정)로 채움");
		}
		else if (_category == DiffCategory::dcCullMode)
		{
			Console::WriteLine("[CullMode - 어느 면을 버릴지]");
			Console::WriteLine(" cmNone : 양면 모두 그림");
			Console::WriteLine(" cmFront: 앞면 버림 (특수)");
			Console::WriteLine(" cmBack : 뒷면 버림 [디폴트] - 큐브 속이 안 보임");
		}
		else if (_category == DiffCategory::dcFillMode)
		{
			Console::WriteLine("[FillMode - 면을 채울지]");
			Console::WriteLine(" fmSolid    : 면 채우기 [디폴트]");
			Console::WriteLine(" fmWireframe: 선만 (토폴로지 디버깅)");
		}
		else if (_category == DiffCategory::dcDepthMode)
		{
			Console::WriteLine("[DepthMode - 깊이 테스트/쓰기]");
			Console::WriteLine(" dmDisabled : DepthEnable FALSE - 2D 적층 순서대로");
			Console::WriteLine(" dmReadWrite: Enable TRUE, Write ALL, Func LESS_EQUAL [디폴트] - 3D 기본");
			Console::WriteLine(" dmReadOnly : Enable TRUE, Write ZERO - 반투명 3D (테스트만)");
		}
		else if (_category == DiffCategory::dcFrontFace)
		{
			Console::WriteLine("[FrontFace - 앞면 판정]");
			Console::WriteLine(" ffClockwise       : 시계방향=앞면, CounterClockwise=FALSE [디폴트] (D3D 기본)");
			Console::WriteLine(" ffCounterClockwise: 반시계=앞면, TRUE (OpenGL 스타일)");
		}
		else if (_category == DiffCategory::dcTopology)
		{
			Console::WriteLine("[PrimitiveTopology - 정점 연결 방식]");
			Console::WriteLine(" ptPointList    : 점 목록");
			Console::WriteLine(" ptLineList     : 독립 선분 (2점당 1선)");
			Console::WriteLine(" ptLineStrip    : 연결된 선");
			Console::WriteLine(" ptTriangleList : 독립 삼각형 [디폴트]");
			Console::WriteLine(" ptTriangleStrip: 띠(이전 2점+신점)");
		}

		const char* afterName = "?";
		if (_category == DiffCategory::dcBlendMode)       afterName = BLEND_NAMES[_afterIndex];
		else if (_category == DiffCategory::dcFilterMode)  afterName = FILTER_NAMES[_afterIndex];
		else if (_category == DiffCategory::dcAddressMode) afterName = ADDRESS_NAMES[_afterIndex];
		else if (_category == DiffCategory::dcCullMode)    afterName = CULL_NAMES[_afterIndex];
		else if (_category == DiffCategory::dcFillMode)    afterName = FILL_NAMES[_afterIndex];
		else if (_category == DiffCategory::dcDepthMode)   afterName = DEPTH_NAMES[_afterIndex];
		else if (_category == DiffCategory::dcFrontFace)   afterName = FRONT_FACE_NAMES[_afterIndex];
		else if (_category == DiffCategory::dcTopology)    afterName = TOPOLOGY_NAMES[_afterIndex];

		char afterBuf[128];
		sprintf_s(afterBuf, " -> After: %s", afterName);
		Console::WriteLine(afterBuf);
		Console::WriteLine(" Left,Right로 After 값을 바꿔보세요. 좌=디폴트, 우=선택 비교!\n");
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// 현재 카테고리/인덱스에 해당하는 표시 이름을 반환한다.

	const char* GetAfterName(DiffCategory _category, _s32 _index)
	{
		if (_category == DiffCategory::dcBlendMode)       return BLEND_NAMES[_index];
		if (_category == DiffCategory::dcFilterMode)      return FILTER_NAMES[_index];
		if (_category == DiffCategory::dcAddressMode)     return ADDRESS_NAMES[_index];
		if (_category == DiffCategory::dcCullMode)        return CULL_NAMES[_index];
		if (_category == DiffCategory::dcFillMode)        return FILL_NAMES[_index];
		if (_category == DiffCategory::dcDepthMode)       return DEPTH_NAMES[_index];
		if (_category == DiffCategory::dcFrontFace)       return FRONT_FACE_NAMES[_index];
		return TOPOLOGY_NAMES[_index];
	}

	const char* GetBeforeName(DiffCategory _category)
	{
		return GetAfterName(_category, DEFAULT_INDEX[static_cast<_s32>(_category)]);
	}

} // namespace

//////////////////////////////////////////////////////////////////////////////////////////
// 튜토리얼 진입점
// - 좌(Before)=디폴트 고정, 우(After)=선택값. Left/Right로 After 상태를 즉시 교체한다.

void GraphicsEnumDiff_Main()
{
	// 현재 보고 있는 카테고리와 After 선택값
	DiffCategory category = DiffCategory::dcBlendMode;
	_s32 afterIdx = 1; // Blend는 bmAlpha(디폴트)와 같은 값으로 시작
	PrintGuide(category, afterIdx);

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("34. GraphicsEnum 한눈에 비교 (1~8 카테고리, Left/Right 값, ESC 종료)", 960, 600))
	{
		Console::WriteLine("윈도우 생성 실패!");
		return;
	}

	InputManager input;
	window.ConnectInput(&input);

	GraphicDevice device;
	if (!device.Initialize(window.Handle(), window.Width(), window.Height()))
	{
		Console::WriteLine("그래픽 디바이스 초기화 실패!");
		window.Destroy();
		return;
	}

	GraphicContext& ctx = device.GetContext();

	// 2. 텍스처 2종 생성
	// - 소프트 원: 블렌드 비교용 (원 밖은 투명)
	// - 체커: 필터/주소 비교용 (작게 만들어 확대 시 차이 강조)

	static _u8 s_SoftPixels[128 * 128 * 4];
	FillSoftCirclePixels(s_SoftPixels, 128);
	Texture texSoft;
	texSoft.CreateFromMemory(&device, s_SoftPixels, 128, 128);

	static _u8 s_Checker[32 * 32 * 4];
	FillCheckerPixels(s_Checker, 32, 32, 4);
	Texture texChecker;
	texChecker.CreateFromMemory(&device, s_Checker, 32, 32);

	// 3. 버퍼 생성
	// - quad: 블렌드용 동적 쿼드 (매 프레임 위치 갱신)
	// - uvQuad: 필터/주소용 전체 화면 쿼드 (UV 0~3)
	// - cube: 컬링/채우기/깊이/앞면용 큐브
	// - topo: 토폴로지 비교용 별 모양

	VertexPTC quadVertices[4] = {};
	_u32 quadIndices[6] = { 0, 1, 2, 2, 1, 3 };
	VertexBuffer vbQuad;
	IndexBuffer ibQuad;
	vbQuad.Create(&device, quadVertices, sizeof(VertexPTC), 4, ResourceUsage::ruDynamic);
	ibQuad.Create(&device, quadIndices, 6);

	VertexPTC uvQuad[4];
	_u32 uvIdx[6];
	FillUvQuad(uvQuad, uvIdx, 3.0f);
	VertexBuffer vbUv;
	IndexBuffer ibUv;
	vbUv.Create(&device, uvQuad, sizeof(VertexPTC), 4);
	ibUv.Create(&device, uvIdx, 6);

	VertexPC cubeVerts[8];
	_u32 cubeIdx[36];
	FillColorCube(cubeVerts, cubeIdx);
	VertexBuffer vbCube;
	IndexBuffer ibCube;
	vbCube.Create(&device, cubeVerts, sizeof(VertexPC), 8);
	ibCube.Create(&device, cubeIdx, 36);

	// 토폴로지용: 5개 외곽점 + 중심 1점 = 별 모양
	VertexPC topoVerts[] =
	{
		{ vec3(0.0f, 0.6f, 0.0f),   color(0xFF, 0xFF, 0x4D, 0xFF) },
		{ vec3(0.57f, 0.18f, 0.0f),  color(0xFF, 0x4D, 0x4D, 0xFF) },
		{ vec3(0.35f, -0.48f, 0.0f), color(0x4D, 0xFF, 0x4D, 0xFF) },
		{ vec3(-0.35f, -0.48f, 0.0f),color(0x4D, 0xFF, 0xFF, 0xFF) },
		{ vec3(-0.57f, 0.18f, 0.0f), color(0x4D, 0x4D, 0xFF, 0xFF) },
		{ vec3(0.0f, 0.0f, 0.0f),    color(0xFF, 0xFF, 0xFF, 0xFF) },
	};
	const _u32 topoIdxList[] = { 0, 1, 2, 0, 2, 3, 0, 3, 4 }; // triangleList
	const _u32 topoIdxStrip[] = { 0, 1, 5, 2, 3, 4 };         // triangleStrip
	VertexBuffer vbTopo;
	IndexBuffer ibTopoTriList, ibTopoStrip, ibTopoLine, ibTopoLineStrip;
	vbTopo.Create(&device, topoVerts, sizeof(VertexPC), 6);
	ibTopoTriList.Create(&device, topoIdxList, 9);
	ibTopoStrip.Create(&device, topoIdxStrip, 6);
	_u32 lineIdx[] = { 0, 1, 1, 2, 2, 3, 3, 4, 4, 0 };
	ibTopoLine.Create(&device, lineIdx, 10);
	_u32 lineStripIdx[] = { 0, 1, 2, 3, 4, 0 };
	ibTopoLineStrip.Create(&device, lineStripIdx, 6);

	// 4. 셰이더 + 상수 버퍼
	UINT layoutPTCCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pPTCLayout = VertexPTC::LayoutDescs(&layoutPTCCount);
	UINT layoutPCCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pPCLayout = VertexPC::LayoutDescs(&layoutPCCount);

	Shader shaderBlend, shaderSplit, shaderColor;
	shaderBlend.CompileFromString(&device, BlendQuadShaderSource34(), pPTCLayout, layoutPTCCount);
	shaderSplit.CompileFromString(&device, SamplerSplitShaderSource34(), pPTCLayout, layoutPTCCount);
	shaderColor.CompileFromString(&device, ColorTransformShaderSource34(), pPCLayout, layoutPCCount);

	ConstantBuffer<CbTransform> cbTransform;
	cbTransform.Create(&device);
	ConstantBuffer<CbSplit> cbSplit;
	cbSplit.Create(&device);

	// 5. 카테고리별 After 선택값 기억 (카테고리를 옮겨도 이전 선택 유지)
	// - 초기 After는 Before와 확실히 다르도록 한 칸 옆 값으로 둔다. (처음부터 차이 보이게)
	_s32 afterIndices[static_cast<_s32>(DiffCategory::Max)] = {};
	for (_s32 i = 0; i < static_cast<_s32>(DiffCategory::Max); ++i)
	{
		_s32 count = COUNT_PER_CATEGORY[i];
		_s32 def = DEFAULT_INDEX[i];
		// Filter는 Point(0)가 Linear와 가장 대비되므로 0으로 고정
		if (static_cast<DiffCategory>(i) == DiffCategory::dcFilterMode)
		{
			afterIndices[i] = 0; // fmPoint
		}
		else
		{
			afterIndices[i] = (def + 1) % count;
		}
	}
	afterIdx = afterIndices[static_cast<_s32>(category)];

	auto UpdateTitle = [&]()
	{
		jc::String title = jc::StringUtil::Format("34. %s (%s)  Before:%s  |  After:%s  [1~8 카테고리, Left/Right 값, ESC]", CATEGORY_NAMES_KO[static_cast<_s32>(category)],
			CATEGORY_NAMES[static_cast<_s32>(category)],
			GetBeforeName(category), GetAfterName(category, afterIdx));
		window.SetTitle(title);
	};
	UpdateTitle();

	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;

	// 6. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}

		// 입력 처리: 카테고리 전환과 값 순회
		bool isCategoryChanged = false;
		bool isValueChanged = false;

		// 숫자 1~8로 카테고리 직접 선택
		for (_s32 i = 0; i < static_cast<_s32>(DiffCategory::Max); ++i)
		{
			if (input.IsKeyPressed('1' + i))
			{
				category = static_cast<DiffCategory>(i);
				afterIdx = afterIndices[static_cast<_s32>(category)];
				isCategoryChanged = true;
				break;
			}
		}

		// Up/Down으로도 카테고리 순회
		if (input.IsKeyPressed(VK_UP))
		{
			_s32 c = (static_cast<_s32>(category) - 1 + static_cast<_s32>(DiffCategory::Max)) % static_cast<_s32>(DiffCategory::Max);
			category = static_cast<DiffCategory>(c);
			afterIdx = afterIndices[c];
			isCategoryChanged = true;
		}
		if (input.IsKeyPressed(VK_DOWN))
		{
			_s32 c = (static_cast<_s32>(category) + 1) % static_cast<_s32>(DiffCategory::Max);
			category = static_cast<DiffCategory>(c);
			afterIdx = afterIndices[c];
			isCategoryChanged = true;
		}

		// Left/Right로 After 값 순회 (파이프라인에 즉시 반영)
		if (input.IsKeyPressed(VK_LEFT))
		{
			_s32 count = COUNT_PER_CATEGORY[static_cast<_s32>(category)];
			afterIdx = (afterIdx - 1 + count) % count;
			afterIndices[static_cast<_s32>(category)] = afterIdx;
			isValueChanged = true;
		}
		if (input.IsKeyPressed(VK_RIGHT))
		{
			_s32 count = COUNT_PER_CATEGORY[static_cast<_s32>(category)];
			afterIdx = (afterIdx + 1) % count;
			afterIndices[static_cast<_s32>(category)] = afterIdx;
			isValueChanged = true;
		}

		if (isCategoryChanged || isValueChanged)
		{
			UpdateTitle();
			PrintGuide(category, afterIdx);

			char buf[128];
			sprintf_s(buf, "[After %d/%d] %s", afterIdx + 1, COUNT_PER_CATEGORY[static_cast<_s32>(category)], GetAfterName(category, afterIdx));
			Console::WriteLine(buf);
		}

		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		device.BeginFrame(color(0x14, 0x14, 0x1F, 0xFF));

		// 큐브 카테고리에서 공통으로 쓰는 뷰/투영 (약간 위에서 내려다보기)
		const mat4 view = mat4::LookAtLH(vec3(0.0f, 1.2f, -4.0f), vec3::Zero(), vec3::Up());
		const mat4 proj = mat4::PerspectiveFovLH(jc_math_pi_div4, window.AspectRatio(), 0.1f, 100.0f);
		const mat4 rot = mat4::RotationY(elapsed * 0.5f) * mat4::RotationX(elapsed * 0.2f);

		if (category == DiffCategory::dcBlendMode)
		{
			// Blend 비교: 3개 색 원이 겹치며 도는 장면
			// - 왼쪽은 디폴트(bmAlpha), 오른쪽은 선택값. 같은 움직임으로 합성 차이만 비교한다.

			const color circleColors[3] =
			{
				color(0xFF, 0x40, 0x40, 0xFF), // 빨강
				color(0x40, 0xFF, 0x40, 0xFF), // 초록
				color(0x4D, 0x66, 0xFF, 0xFF),  // 파랑
			};

			texSoft.Bind(&device, 0);
			shaderBlend.Bind(&device);
			ctx.SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);

			auto DrawCircleGroup = [&](const vec2& _groupCenter, BlendMode _mode)
			{
				device.SetBlendMode(_mode);
				for (_s32 i = 0; i < 3; ++i)
				{
					_f32 angle = elapsed * 0.8f + jc_math_pi2 * i / 3.0f;
					vec2 center(_groupCenter.x + cosf(angle) * 0.15f, _groupCenter.y + sinf(angle) * 0.15f);
					FillQuadVertices(quadVertices, center, 0.28f, circleColors[i]);
					vbQuad.Update(&device, quadVertices, 4);
					vbQuad.Bind(&device);
					ibQuad.Bind(&device);
					ctx.DrawIndexed(6, 0, 0);
				}
			};

			// [Before] 왼쪽: 디폴트 고정
			DrawCircleGroup(vec2(-0.5f, 0.0f), BLEND_VALUES[DEFAULT_INDEX[0]]);
			// [After] 오른쪽: 선택값
			DrawCircleGroup(vec2(+0.5f, 0.0f), BLEND_VALUES[afterIdx]);

			// 경계선: 중앙에 흰 세로선 (텍스처 중심 한 점만 샘플링해 불투명 흰색)
			device.SetBlendMode(BlendMode::bmNone);
			const vec2 uvCenter(0.5f, 0.5f);
			const color lineColor(0xF2, 0xF2, 0xF2);
			quadVertices[0] = { vec3(-0.004f, +1.0f, 0.0f), uvCenter, lineColor };
			quadVertices[1] = { vec3(+0.004f, +1.0f, 0.0f), uvCenter, lineColor };
			quadVertices[2] = { vec3(-0.004f, -1.0f, 0.0f), uvCenter, lineColor };
			quadVertices[3] = { vec3(+0.004f, -1.0f, 0.0f), uvCenter, lineColor };
			vbQuad.Update(&device, quadVertices, 4);
			vbQuad.Bind(&device);
			ibQuad.Bind(&device);
			ctx.DrawIndexed(6, 0, 0);
		}
		else if (category == DiffCategory::dcFilterMode || category == DiffCategory::dcAddressMode)
		{
			// Filter/Address 비교: 한 장의 체커를 두 샘플러(s0/s1)로 나눠 읽기
			// - Filter는 Wrap에서, Address는 UV 0~3에서 차이가 보인다.
			// - s0=Before, s1=After. 셰이더가 화면 x좌표로 어느 쪽을 쓸지 고른다.

			FilterMode filterBefore;
			FilterMode filterAfter;
			AddressMode addressBefore;
			AddressMode addressAfter;

			if (category == DiffCategory::dcFilterMode)
			{
				filterBefore = FILTER_VALUES[DEFAULT_INDEX[1]]; // fmLinear
				filterAfter = FILTER_VALUES[afterIdx];
				addressBefore = AddressMode::amWrap;
				addressAfter = AddressMode::amWrap;
			}
			else
			{
				filterBefore = FilterMode::fmLinear;
				filterAfter = FilterMode::fmLinear;
				addressBefore = ADDRESS_VALUES[DEFAULT_INDEX[2]]; // amClamp
				addressAfter = ADDRESS_VALUES[afterIdx];
			}

			device.SetSampler(filterBefore, addressBefore, 0);
			device.SetSampler(filterAfter, addressAfter, 1);

			CbSplit cb;
			cb.splitPixelX_ = window.Width() * 0.5f;
			cb.pad_[0] = cb.pad_[1] = cb.pad_[2] = 0.0f;
			cbSplit.UpdateAndBind(&device, cb, 0);

			texChecker.Bind(&device, 0);
			vbUv.Bind(&device);
			ibUv.Bind(&device);
			shaderSplit.Bind(&device);
			ctx.SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
			ctx.DrawIndexed(6, 0, 0);
		}
		else if (category == DiffCategory::dcCullMode || category == DiffCategory::dcFillMode || category == DiffCategory::dcFrontFace)
		{
			// Cull/Fill/FrontFace 비교: 같은 큐브를 좌우 두 번 그리기
			// - 왼쪽은 디폴트 고정, 오른쪽은 선택값. 회전으로 모든 면을 관찰한다.

			vbCube.Bind(&device);
			ibCube.Bind(&device);
			shaderColor.Bind(&device);
			ctx.SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);

			auto DrawCube = [&](_f32 _offsetX, CullMode _cull, FillMode _fill, FrontFace _front)
			{
				// FrontFace가 달라지는 경우는 GraphicDevice의 간단 래퍼로 처리할 수 없어
				// RenderStates에서 직접 래스터라이저 상태를 꺼내 바인딩한다.
				if (category == DiffCategory::dcFrontFace)
				{
					auto* pRs = device.States().GetRasterizerState(_cull, _fill, _front);
					ctx.SetRasterizerStateRaw(pRs);
				}
				else
				{
					device.SetWireframe(_fill == FillMode::fmWireframe);
					device.SetCullMode(_cull);
				}

				CbTransform cb;
				cb.wvp_ = rot * mat4::Translation(_offsetX, 0.0f, 0.0f) * view * proj;
				cbTransform.UpdateAndBind(&device, cb, 0);
				ctx.DrawIndexed(36, 0, 0);
			};

			CullMode cullBefore = CULL_VALUES[DEFAULT_INDEX[3]];
			CullMode cullAfter = cullBefore;
			FillMode fillBefore = FILL_VALUES[DEFAULT_INDEX[4]];
			FillMode fillAfter = fillBefore;
			FrontFace frontBefore = FRONT_FACE_VALUES[DEFAULT_INDEX[6]];
			FrontFace frontAfter = frontBefore;

			if (category == DiffCategory::dcCullMode)        cullAfter = CULL_VALUES[afterIdx];
			else if (category == DiffCategory::dcFillMode)   fillAfter = FILL_VALUES[afterIdx];
			else if (category == DiffCategory::dcFrontFace)  frontAfter = FRONT_FACE_VALUES[afterIdx];

			DrawCube(-1.1f, cullBefore, fillBefore, frontBefore);
			DrawCube(+1.1f, cullAfter, fillAfter, frontAfter);
		}
		else if (category == DiffCategory::dcDepthMode)
		{
			// Depth 비교: 앞뒤 두 큐브의 겹침을 깊이 상태로 제어
			// - dmDisabled면 그리는 순서대로, dmReadWrite면 Z가 앞선 것이 이긴다.

			vbCube.Bind(&device);
			ibCube.Bind(&device);
			shaderColor.Bind(&device);
			ctx.SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
			device.SetWireframe(false);
			device.SetCullMode(CullMode::cmBack);

			auto DrawDepthPair = [&](_f32 _offsetX, DepthMode _mode)
			{
				auto* pDs = device.States().GetDepthState(_mode);
				ctx.SetDepthStencilStateRaw(pDs);

				// 뒤 큐브 (Z=0.5)
				{
					CbTransform cb;
					cb.wvp_ = mat4::Translation(_offsetX - 0.3f, 0.0f, 0.5f) * view * proj;
					cbTransform.UpdateAndBind(&device, cb, 0);
					ctx.DrawIndexed(36, 0, 0);
				}
				// 앞 큐브 (Z=0.0)
				{
					CbTransform cb;
					cb.wvp_ = mat4::Translation(_offsetX + 0.3f, 0.0f, 0.0f) * view * proj;
					cbTransform.UpdateAndBind(&device, cb, 0);
					ctx.DrawIndexed(36, 0, 0);
				}
			};

			DepthMode depthBefore = DEPTH_VALUES[DEFAULT_INDEX[5]];
			DepthMode depthAfter = DEPTH_VALUES[afterIdx];
			DrawDepthPair(-1.1f, depthBefore);
			DrawDepthPair(+1.1f, depthAfter);

			// 다음 프레임을 위해 기본 깊이로 복구
			ctx.SetDepthStencilStateRaw(device.States().GetDepthState(DepthMode::dmReadWrite));
		}
		else if (category == DiffCategory::dcTopology)
		{
			// Topology는 Before/After 분할 없이 중앙에 하나만 표시한다.
			// - 정점 연결 방식 자체가 화면에 드러나므로 좌우 비교보다 중앙 집중이 더 명확하다.

			shaderColor.Bind(&device);
			vbTopo.Bind(&device);

			PrimitiveTopology topoAfter = TOPOLOGY_VALUES[afterIdx];
			{
				CbTransform cb;
				cb.wvp_ = mat4::Translation(0.0f, 0.0f, 0.0f) * view * proj;
				cbTransform.UpdateAndBind(&device, cb, 0);
				ctx.SetPrimitiveTopology(topoAfter);

				if (topoAfter == PrimitiveTopology::ptPointList)
				{
					ctx.Draw(5, 0);
				}
				else if (topoAfter == PrimitiveTopology::ptLineList)
				{
					ibTopoLine.Bind(&device);
					ctx.DrawIndexed(10, 0, 0);
				}
				else if (topoAfter == PrimitiveTopology::ptLineStrip)
				{
					ibTopoLineStrip.Bind(&device);
					ctx.DrawIndexed(6, 0, 0);
				}
				else if (topoAfter == PrimitiveTopology::ptTriangleList)
				{
					ibTopoTriList.Bind(&device);
					ctx.DrawIndexed(9, 0, 0);
				}
				else // ptTriangleStrip
				{
					ibTopoStrip.Bind(&device);
					ctx.DrawIndexed(6, 0, 0);
				}
			}
			ctx.SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
		}

		device.EndFrame(true);
	}

	// 정리: 다음 튜토리얼에 영향을 주지 않도록 기본 상태로 되돌린다.
	device.SetBlendMode(BlendMode::bmNone);
	device.SetWireframe(false);
	device.SetCullMode(CullMode::cmBack);
	ctx.SetDepthStencilStateRaw(device.States().GetDepthState(DepthMode::dmReadWrite));
	ctx.SetRasterizerStateRaw(device.States().GetRasterizerState(CullMode::cmBack, FillMode::fmSolid, FrontFace::ffClockwise));
	device.Finalize();
	window.Destroy();
}
