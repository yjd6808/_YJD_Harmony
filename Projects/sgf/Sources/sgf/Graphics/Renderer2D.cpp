/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:26:00 AM
 * =====================
 * 2D 즉시 렌더러 구현부
 */

#include "Core.h"
#include "sgf/Graphics/Renderer2D.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/Texture.h"

#include <cmath>

NS_SGF_BEGIN

using namespace jc;

// ==================================================
// 내장 HLSL 셰이더
// ==================================================
// [셰이더 해설]
//  cbuffer      : CPU에서 넘겨주는 전역값. row_major로 선언해서
//                 C++의 행우선 Mat4를 전치 없이 그대로 받는다.
//  VSMain       : 정점마다 실행. 월드 좌표 -> 클립 좌표 변환.
//                 mul(v, M)은 행벡터 x 행렬 순서로, 우리 수학 규약과 일치.
//  PSMain       : 픽셀마다 실행. 텍스처 색 x 정점 색(틴트).
//  SV_POSITION  : "이 값이 클립 좌표다"라고 파이프라인에 알려주는 의미소(Semantic).
static const char* s_szSpriteShader = R"(
cbuffer CbFrame : register(b0)
{
	row_major float4x4 gViewProjection;	// 뷰 x 프로젝션 행렬
};

Texture2D gTexture : register(t0);		// 그릴 텍스처
SamplerState gSampler : register(s0);	// 텍스처 읽기 규칙

struct VSInput
{
	float3 position : POSITION;		// 월드 좌표 (CPU에서 미리 변환됨)
	float2 uv       : TEXCOORD0;	// 텍스처 좌표
	float4 color    : COLOR0;		// 틴트 색상
};

struct PSInput
{
	float4 position : SV_POSITION;	// 클립 좌표
	float2 uv       : TEXCOORD0;
	float4 color    : COLOR0;
};

// 정점 셰이더: 월드 좌표에 뷰프로젝션만 곱해 클립 좌표로 변환
PSInput VSMain(VSInput input)
{
	PSInput output;
	output.position = mul(float4(input.position, 1.0f), gViewProjection);
	output.uv = input.uv;
	output.color = input.color;
	return output;
}

// 픽셀 셰이더: 텍스처 색상에 틴트를 곱한다.
// 흰색 1x1 텍스처를 쓰면 결과가 틴트 색 그대로가 되므로 단색 도형도 같은 셰이더로 그린다.
float4 PSMain(PSInput input) : SV_TARGET
{
	return gTexture.Sample(gSampler, input.uv) * input.color;
}
)";

//////////////////////////////////////////////////////////////////////////////////////////
// 생성자
Renderer2D::Renderer2D()
	: pWhiteTexture_(nullptr)
	, vertices_(MAX_VERTICES)
	, pCurrentTexture_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 소멸자
Renderer2D::~Renderer2D()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
// BatchRenderer 훅: 스프라이트 셰이더 소스
const char* Renderer2D::ShaderSource() const
{
	return s_szSpriteShader;
}

//////////////////////////////////////////////////////////////////////////////////////////
// BatchRenderer 훅: VertexPTC 레이아웃
const D3D11_INPUT_ELEMENT_DESC* Renderer2D::VertexLayout(UINT* _outCount) const
{
	return VertexPTC::LayoutDescs(_outCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
// BatchRenderer 훅: 셰이더/상수 버퍼 생성 후 호출된다.
// 정점/인덱스 버퍼와 흰색 텍스처를 만든다.
bool Renderer2D::CreateBatchResources(GraphicDevice* _pDevice)
{
	// 1. DYNAMIC 정점 버퍼 (매 프레임 CPU가 채워 넣는다)
	if (!vertexBuffer_.Create(_pDevice, nullptr, sizeof(VertexPTC), MAX_VERTICES, true))
	{
		return false;
	}

	// 2. 사각형용 인덱스 버퍼 (패턴이 고정이므로 미리 전부 만들어둔다)
	// 사각형 1개 = 정점 4개(0,1,2,3) = 삼각형 2개(0-1-2, 0-2-3)
	{
		jc::Vector<_u32> indices(MAX_INDICES, 0u);
		for (_s32 i = 0; i < MAX_QUADS; ++i)
		{
			const _u32 base = _u32(i) * 4;
			indices[i * 6 + 0] = base + 0;
			indices[i * 6 + 1] = base + 1;
			indices[i * 6 + 2] = base + 2;
			indices[i * 6 + 3] = base + 0;
			indices[i * 6 + 4] = base + 2;
			indices[i * 6 + 5] = base + 3;
		}
		if (!indexBuffer_.Create(_pDevice, indices.Source(), MAX_INDICES))
		{
			return false;
		}
	}

	// 3. 1x1 흰색 텍스처 (단색 도형을 그릴 때 사용)
	JC_DELETE_SAFE(pWhiteTexture_);
	pWhiteTexture_ = new Texture();
	const _u8 whitePixel[4] = { 255, 255, 255, 255 };
	if (!pWhiteTexture_->CreateFromMemory(_pDevice, whitePixel, 1, 1))
	{
		JC_DELETE_SAFE(pWhiteTexture_);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Renderer2D::Finalize()
{
	JC_DELETE_SAFE(pWhiteTexture_);
	BatchRenderer::Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
// BatchRenderer 훅: Begin() 공통 처리 후 호출된다.
// 2D는 깊이 테스트 끄고(그리는 순서로 적층), 알파 블렌딩을 켠다.
void Renderer2D::OnBegin()
{
	vertices_.Clear();
	pCurrentTexture_ = nullptr;

	pDevice_->SetDepthTest(false);
	pDevice_->SetAlphaBlending(true);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 사각형 4정점을 배치에 추가
void Renderer2D::PushQuad(Texture* _pTexture, const VertexPTC (&_vertices)[4])
{
	jc_assert(begun_);

	// 텍스처가 바뀌면 지금까지 모은 배치를 먼저 그린다.
	// (한 드로우 콜은 텍스처 1장만 쓸 수 있으므로)
	if (pCurrentTexture_ != nullptr && pCurrentTexture_ != _pTexture)
	{
		Flush();
	}
	// 배치가 꽉 차도 먼저 그린다.
	if (vertices_.Size() + 4 > MAX_VERTICES)
	{
		Flush();
	}

	pCurrentTexture_ = _pTexture;
	for (_s32 i = 0; i < 4; ++i)
	{
		vertices_.PushBack(_vertices[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 단색 사각형
void Renderer2D::DrawRect(const vec2& _center, const vec2& _size, const color& _color, _f32 _radian)
{
	// 흰색 텍스처 x 색상 틴트 = 단색. 사각형은 회전 포함 DrawSprite와 동일 로직.
	DrawSprite(pWhiteTexture_, _center, _size, _color, _radian);
}

// 텍스처 스프라이트
void Renderer2D::DrawSprite(Texture* _pTexture, const vec2& _center, const vec2& _size,
	const color& _tint, _f32 _radian, const vec2& _uvMin, const vec2& _uvMax)
{
	if (_pTexture == nullptr || !_pTexture->IsValid())
	{
		return;
	}

	// 중심 기준 반폭
	const _f32 halfW = _size.x * 0.5f;
	const _f32 halfH = _size.y * 0.5f;

	// 회전 행렬을 직접 풌어 쓴다. (2D 회전은 2x2만 있으면 충분)
	// [ cos -sin ]   로컬 (x,y) -> 회전된 (x*cos - y*sin, x*sin + y*cos)
	// [ sin  cos ]
	const _f32 c = cosf(_radian);
	const _f32 s = sinf(_radian);

	// 로컬 4구석 (좌하, 우하, 우상, 좌상) - 월드는 Y위쪽+ 기준
	const vec2 locals[4] = {
		vec2(-halfW, -halfH),
		vec2(+halfW, -halfH),
		vec2(+halfW, +halfH),
		vec2(-halfW, +halfH),
	};
	// UV는 텍스처 좌표계(Y아래쪽+)이므로 위아래를 뒤집어 매칭한다.
	const vec2 uvs[4] = {
		vec2(_uvMin.x, _uvMax.y),	// 좌하 정점 <- UV 좌하단
		vec2(_uvMax.x, _uvMax.y),	// 우하 정점 <- UV 우하단
		vec2(_uvMax.x, _uvMin.y),	// 우상 정점 <- UV 우상단
		vec2(_uvMin.x, _uvMin.y),	// 좌상 정점 <- UV 좌상단
	};

	VertexPTC vertices[4];
	for (_s32 i = 0; i < 4; ++i)
	{
		// 회전 후 평행이동 = 월드 위치
		const _f32 x = locals[i].x * c - locals[i].y * s + _center.x;
		const _f32 y = locals[i].x * s + locals[i].y * c + _center.y;
		vertices[i].position_ = vec3(x, y, 0.0f);
		vertices[i].uv_ = uvs[i];
		vertices[i].color_ = _tint;
	}

	PushQuad(_pTexture, vertices);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 선분: 두 점을 잇는 가늘고 긴 사각형을 회전시켜 그린다.
void Renderer2D::DrawLine(const vec2& _from, const vec2& _to, const color& _color, _f32 _thickness)
{
	const _f32 dx = _to.x - _from.x;
	const _f32 dy = _to.y - _from.y;
	const _f32 length = sqrtf(dx * dx + dy * dy);
	if (length <= Epsilon_v)
	{
		return;
	}

	// 중심 = 두 점의 중간, 회전각 = 방향 벡터의 각도
	const vec2 center((_from.x + _to.x) * 0.5f, (_from.y + _to.y) * 0.5f);
	const _f32 radian = atan2f(dy, dx);
	DrawRect(center, vec2(length, _thickness), _color, radian);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 원: 중심에서 부채꼴로 폼치는 삼각형들로 근사한다.
// 사각형 배치를 재사용하기 위해 삼각형을 "높이 0인 사각형"처럼 넣는 대신
// 정점 4개 중 2개를 같은 위치로 만들어 삼각형을 표현한다.
void Renderer2D::DrawCircle(const vec2& _center, _f32 _radius, const color& _color, _s32 _segments)
{
	if (_segments < 3)
	{
		_segments = 3;
	}

	for (_s32 i = 0; i < _segments; ++i)
	{
		const _f32 a0 = (jc_math_pi2) * _f32(i) / _f32(_segments);
		const _f32 a1 = (jc_math_pi2) * _f32(i + 1) / _f32(_segments);

		const vec2 p0(_center.x + cosf(a0) * _radius, _center.y + sinf(a0) * _radius);
		const vec2 p1(_center.x + cosf(a1) * _radius, _center.y + sinf(a1) * _radius);

		// 삼각형(중심, p0, p1)을 정점 4개짜리 쿨로 표현 (마지막 정점 중복)
		VertexPTC vertices[4];
		const vec2 positions[4] = { _center, p0, p1, p1 };
		for (_s32 corner = 0; corner < 4; ++corner)
		{
			vertices[corner].position_ = vec3(positions[corner].x, positions[corner].y, 0.0f);
			vertices[corner].uv_ = vec2(0.0f, 0.0f);
			vertices[corner].color_ = _color;
		}
		PushQuad(pWhiteTexture_, vertices);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 배치를 GPU로 전송하고 드로우 콜 실행
void Renderer2D::Flush()
{
	const _s32 vertexCount = vertices_.Size();
	if (vertexCount == 0)
	{
		return;
	}

	// [v2.1] 깊이/블렌드 상태는 Flush 직전에 확정한다.
	// 같은 프레임에 Renderer3D(깊이 켜고 그림)와 섞여 쓰여도
	// 2D는 항상 "깊이 끄고, 반투명 켜고" 그려지도록 보장하기 위함이다.
	pDevice_->SetDepthTest(false);
	pDevice_->SetAlphaBlending(true);

	// 1. CPU 배치 -> GPU 정점 버퍼 복사
	vertexBuffer_.Update(pDevice_, vertices_.Source(), UINT(vertexCount));

	// 2. 파이프라인 구성: 셰이더/버퍼/텍스처/상수버퍼/토폴로지
	ApplyFrameStates();
	vertexBuffer_.Bind(pDevice_);
	indexBuffer_.Bind(pDevice_);
	if (pCurrentTexture_ != nullptr)
	{
		pCurrentTexture_->Bind(pDevice_, 0);
	}
	pDevice_->Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 3. 드로우 콜: 정점 4개당 인덱스 6개 비율로 그린다.
	const UINT indexCount = UINT(vertexCount / 4) * 6;

	// [DEBUG-DIAG] 드로우 콜 발생 확인용 임시 로그 (수정 후 제거 예정)
	{
		FILE* pFile = nullptr;
		fopen_s(&pFile, "C:\\Users\\jdyun\\AppData\\Local\\Temp\\opencode\\sgfr29_diag.log", "a");
		if (pFile != nullptr)
		{
			fprintf(pFile, "[Renderer2D::Flush] vertexCount=%d indexCount=%u\n", vertexCount, indexCount);
			fflush(pFile);
			fclose(pFile);
		}
	}

	pDevice_->Context()->DrawIndexed(indexCount, 0, 0);
	pDevice_->Context()->DrawIndexed(indexCount, 0, 0);

	// 4. 배치 비우기
	vertices_.Clear();
}

NS_SGF_END
