/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 9:40:00 AM
 * =====================
 * 3D 배치 렌더러 구현부
 *
 * [구조 요약]
 *  DrawCube/DrawLine3D 등은 즉시 그리지 않고 CPU측 배열에 정점만 쌓는다.
 *  End()에서 한 번에 GPU로 복사해 그린다. (Renderer2D와 같은 배치 원리)
 *  공통 흐름(Begin/End/Initialize)은 BatchRenderer 베이스가 처리한다.
 */

#include "Core.h"
#include "sgf/Graphics/Renderer3D.h"
#include "sgf/Graphics/GraphicDevice.h"

NS_SGF_BEGIN

using namespace jc;

// 위치+색 전용 셔이더.
// 정점을 뷰*투영 행렬로 변환하고, 정점 색을 그대로 칠한다.
static const char* s_szColorShader = R"(
cbuffer CbFrame : register(b0)
{
	row_major float4x4 gViewProjection;
};

struct VsInput
{
	float3 position : POSITION;
	float4 color    : COLOR;
};

struct PsInput
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
};

PsInput VSMain(VsInput input)
{
	PsInput output;
	output.position = mul(float4(input.position, 1.0f), gViewProjection);
	output.color = input.color;
	return output;
}

float4 PSMain(PsInput input) : SV_TARGET
{
	return input.color;
}
)";

//////////////////////////////////////////////////////////////////////////////////////////
Renderer3D::Renderer3D()
	: triangleVertices_(MAX_TRIANGLES * 3)
	, lineVertices_(MAX_LINES * 2)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Renderer3D::~Renderer3D()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
// BatchRenderer 훅: 위치+색 셔이더 소스
const char* Renderer3D::ShaderSource() const
{
	return s_szColorShader;
}

//////////////////////////////////////////////////////////////////////////////////////////
// BatchRenderer 훅: VertexPC 레이아웃
const D3D11_INPUT_ELEMENT_DESC* Renderer3D::VertexLayout(UINT* _outCount) const
{
	return VertexPC::LayoutDescs(_outCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
// BatchRenderer 훅: 셰이더/상수 버퍼 생성 후 호출된다.
// 삼각형/선용 동적(DYNAMIC) 정점 버퍼를 만든다. (매 프레임 CPU 배치를 복사해 넣는다)
bool Renderer3D::CreateBatchResources(GraphicDevice* _pDevice)
{
	if (!triangleVb_.Create(_pDevice, nullptr, sizeof(VertexPC), MAX_TRIANGLES * 3, true))
	{
		return false;
	}
	if (!lineVb_.Create(_pDevice, nullptr, sizeof(VertexPC), MAX_LINES * 2, true))
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Renderer3D::Finalize()
{
	// 셰이더/버퍼는 각자의 소멸자(ComPtr)가 GPU 리소스를 알아서 해제한다.
	// (Renderer2D::Finalize와 같은 방식)
	pDevice_ = nullptr;
	BatchRenderer::Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
// BatchRenderer 훅: Begin() 공통 처리 후 호출된다.
// [주의] 깊이/블렌드 상태는 여기서 바꾸지 않고 Flush 직전에 적용한다.
// Renderer2D와 Begin~End 구간이 겹쳐도 서로 상태를 덮어쓰지 않기 위함이다.
void Renderer3D::OnBegin()
{
	triangleVertices_.Clear();
	lineVertices_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Renderer3D::Flush()
{
	FlushBatch(triangleVb_, triangleVertices_, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	FlushBatch(lineVb_, lineVertices_, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 하나의 배치를 GPU로 복사해 한 번의 DrawCall로 그린다.
void Renderer3D::FlushBatch(VertexBuffer& _vertexBuffer, jc::Vector<VertexPC>& _vertices,
	D3D11_PRIMITIVE_TOPOLOGY _topology)
{
	const _s32 vertexCount = _vertices.Size();
	if (vertexCount == 0)
	{
		return;
	}

	// 3D는 앞뒤 가림이 있어야 하므로 깊이 테스트를 켜고,
	// 단색 면/선은 불투명이므로 블렌딩은 끈다.
	pDevice_->SetDepthTest(true);
	pDevice_->SetAlphaBlending(false);

	_vertexBuffer.Update(pDevice_, _vertices.Source(), UINT(vertexCount));

	ApplyFrameStates();
	_vertexBuffer.Bind(pDevice_);
	pDevice_->Context()->IASetPrimitiveTopology(_topology);
	pDevice_->Context()->Draw(UINT(vertexCount), 0);

	_vertices.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
// [내부 도우미] 사각형(면) 하나를 삼각형 2개로 쪼개 배치에 넣는다.
// 정점 순서는 바깥에서 볼 때 (왼위, 오른위, 왼아래, 오른아래).
// 19장 큐브와 동일한 시계 방향 감기(winding)를 사용한다. (D3D 기본 = 앞면)
static void sAddQuad(jc::Vector<VertexPC>& _vertices,
	const vec3& _topLeft, const vec3& _topRight,
	const vec3& _bottomLeft, const vec3& _bottomRight,
	const color& _color)
{
	// 삼각형 1: 왼위 -> 오른위 -> 왼아래
	_vertices.PushBack({ _topLeft, _color });
	_vertices.PushBack({ _topRight, _color });
	_vertices.PushBack({ _bottomLeft, _color });
	// 삼각형 2: 왼아래 -> 오른위 -> 오른아래
	_vertices.PushBack({ _bottomLeft, _color });
	_vertices.PushBack({ _topRight, _color });
	_vertices.PushBack({ _bottomRight, _color });
}

//////////////////////////////////////////////////////////////////////////////////////////
// 삼각형 하나 배치. (바깥에서 볼 때 시계 방향 순서가 앞면이다)
void Renderer3D::DrawTriangle(const vec3& _p0, const vec3& _p1, const vec3& _p2, const color& _color)
{
	jc_assert(begun_);

	if (triangleVertices_.Size() + 3 > MAX_TRIANGLES * 3)
	{
		FlushBatch(triangleVb_, triangleVertices_, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// 버퍼가 가득 찼으면 지금까지 모은 걸 먼저 그린다
	}

	triangleVertices_.PushBack({ _p0, _color });
	triangleVertices_.PushBack({ _p1, _color });
	triangleVertices_.PushBack({ _p2, _color });
}

//////////////////////////////////////////////////////////////////////////////////////////
// 단색 상자 배치.
// [CPU측 간이 음영] 조명 셔이더 없이도 입체감이 느껴지도록
// 면마다 밝기 계수를 달리 적용한다. (윗면이 가장 밝고 아랫면이 가장 어둡다)
void Renderer3D::DrawCube(const vec3& _center, const vec3& _size, const color& _color)
{
	jc_assert(begun_);

	// 큐브 한 개 = 6면 x 삼각형 2개 x 정점 3개 = 36 정점
	if (triangleVertices_.Size() + 36 > MAX_TRIANGLES * 3)
	{
		FlushBatch(triangleVb_, triangleVertices_, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	const _f32 hx = _size.x * 0.5f;
	const _f32 hy = _size.y * 0.5f;
	const _f32 hz = _size.z * 0.5f;
	const _f32 cx = _center.x;
	const _f32 cy = _center.y;
	const _f32 cz = _center.z;

	// 면별 밝기 계수로 색을 미리 계산해둔다.
	auto shade = [&_color](_f32 _k)
	{
		return color(_color.r * _k, _color.g * _k, _color.b * _k, _color.a);
	};

	// 각 면의 네 귀퍼는 "바깥에서 볼 때" (왼위, 오른위, 왼아래, 오른아래) 순서.
	// (19장 큐브 인덱스와 동일한 감기)

	// 앞면 (-Z)
	sAddQuad(triangleVertices_,
		vec3(cx - hx, cy + hy, cz - hz), vec3(cx + hx, cy + hy, cz - hz),
		vec3(cx - hx, cy - hy, cz - hz), vec3(cx + hx, cy - hy, cz - hz), shade(0.80f));

	// 넫면 (+Z)
	sAddQuad(triangleVertices_,
		vec3(cx + hx, cy + hy, cz + hz), vec3(cx - hx, cy + hy, cz + hz),
		vec3(cx + hx, cy - hy, cz + hz), vec3(cx - hx, cy - hy, cz + hz), shade(0.65f));

	// 왼면 (-X)
	sAddQuad(triangleVertices_,
		vec3(cx - hx, cy + hy, cz + hz), vec3(cx - hx, cy + hy, cz - hz),
		vec3(cx - hx, cy - hy, cz + hz), vec3(cx - hx, cy - hy, cz - hz), shade(0.55f));

	// 오른면 (+X)
	sAddQuad(triangleVertices_,
		vec3(cx + hx, cy + hy, cz - hz), vec3(cx + hx, cy + hy, cz + hz),
		vec3(cx + hx, cy - hy, cz - hz), vec3(cx + hx, cy - hy, cz + hz), shade(0.90f));

	// 윗면 (+Y) - 가장 밝다 (하늘에서 빛이 내리쪼는 느낌)
	sAddQuad(triangleVertices_,
		vec3(cx - hx, cy + hy, cz + hz), vec3(cx + hx, cy + hy, cz + hz),
		vec3(cx - hx, cy + hy, cz - hz), vec3(cx + hx, cy + hy, cz - hz), shade(1.00f));

	// 아랫면 (-Y) - 가장 어둡다
	sAddQuad(triangleVertices_,
		vec3(cx - hx, cy - hy, cz - hz), vec3(cx + hx, cy - hy, cz - hz),
		vec3(cx - hx, cy - hy, cz + hz), vec3(cx + hx, cy - hy, cz + hz), shade(0.45f));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 3D 선분 하나 배치.
void Renderer3D::DrawLine3D(const vec3& _from, const vec3& _to, const color& _color)
{
	jc_assert(begun_);

	if (lineVertices_.Size() + 2 > MAX_LINES * 2)
	{
		FlushBatch(lineVb_, lineVertices_, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}

	lineVertices_.PushBack({ _from, _color });
	lineVertices_.PushBack({ _to, _color });
}

//////////////////////////////////////////////////////////////////////////////////////////
// XZ 평면 격자 (바닥 기준선)
void Renderer3D::DrawGrid(_s32 _halfCount, _f32 _spacing, const color& _color)
{
	const _f32 extent = _halfCount * _spacing;

	for (_s32 i = -_halfCount; i <= _halfCount; ++i)
	{
		const _f32 offset = i * _spacing;
		// Z 방향 세로선 + X 방향 가로선
		DrawLine3D(vec3(offset, 0.0f, -extent), vec3(offset, 0.0f, +extent), _color);
		DrawLine3D(vec3(-extent, 0.0f, offset), vec3(+extent, 0.0f, offset), _color);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 원점 좌표축: X=빨강, Y=초록, Z=파랑
void Renderer3D::DrawAxis(_f32 _length)
{
	DrawLine3D(vec3(0.0f, 0.0f, 0.0f), vec3(_length, 0.0f, 0.0f), color(1.0f, 0.2f, 0.2f, 1.0f));
	DrawLine3D(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, _length, 0.0f), color(0.2f, 1.0f, 0.2f, 1.0f));
	DrawLine3D(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, _length), color(0.2f, 0.4f, 1.0f, 1.0f));
}

NS_SGF_END
