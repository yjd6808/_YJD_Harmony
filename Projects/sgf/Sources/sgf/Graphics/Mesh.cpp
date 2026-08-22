/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:40:00 PM
 * 수정일: 8/16/2026 (2D 프리미티브 팩토리 + CPU 미러)
 * =====================
 * 메시 구현부
 */

#include "Core.h"
#include "sgf/Graphics/Mesh.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/GraphicContext.h"
#include "sgf/Graphics/Vertex.h"
#include "sgf/Graphics/Fill.h"
#include "sgf/Graphics/PrimitiveBuilder.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
Mesh::Mesh()
	: topology_(PrimitiveTopology::ptTriangleList)
	, indexed_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Mesh::~Mesh()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Mesh::Initialize(
	GraphicDevice* _pDevice,
	const void* _pVertices, UINT _stride, UINT _vertexCount,
	const D3D11_INPUT_ELEMENT_DESC* _pLayoutDescs, UINT _layoutCount,
	VertexShader* _pVs,
	const _u32* _pIndices, UINT _indexCount,
	PrimitiveTopology _topology)
{
	jc_assert_msg(_pVertices != nullptr && _vertexCount > 0, "정점 데이터가 비어있습니다.");

	if (!vertexBuffer_.Create(_pDevice, _pVertices, _stride, _vertexCount))
	{
		return false;
	}

	indexed_ = (_pIndices != nullptr && _indexCount > 0);
	if (indexed_)
	{
		if (!indexBuffer_.Create(_pDevice, _pIndices, _indexCount))
		{
			return false;
		}
	}

	if (!inputLayout_.Initialize(_pDevice, _pLayoutDescs, _layoutCount, _pVs))
	{
		return false;
	}

	topology_ = _topology;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// XY 평면 1x1 쿼드. (— 2D 프리미티브 팩토리로 위임)
bool Mesh::InitializeAsQuad2D(GraphicDevice* _pDevice, VertexShader* _pVs)
{
	return InitializeAsRect2D(_pDevice, _pVs);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 1x1 사각형. 중심이 원점이라 스케일 = 크기, 이동 = 위치로 바로 쓰인다. (VertexPTC)
// UV는 기존 InitializeAsQuad2D와 동일(왼위→(0,0)) — 텍스처 방향을 보존한다.
bool Mesh::InitializeAsRect2D(GraphicDevice* _pDevice, VertexShader* _pVs)
{
	const color white = color::WHITE;
	FillResult result;
	result.vertices_.PushBack(VertexPTC{ vec3(-0.5f, +0.5f, 0.0f), vec2(0.0f, 0.0f), white });	// 왼위
	result.vertices_.PushBack(VertexPTC{ vec3(+0.5f, +0.5f, 0.0f), vec2(1.0f, 0.0f), white });	// 오른위
	result.vertices_.PushBack(VertexPTC{ vec3(+0.5f, -0.5f, 0.0f), vec2(1.0f, 1.0f), white });	// 오른아래
	result.vertices_.PushBack(VertexPTC{ vec3(-0.5f, -0.5f, 0.0f), vec2(0.0f, 1.0f), white });	// 왼아래
	result.indices_.PushBack((_u16)0); result.indices_.PushBack((_u16)1); result.indices_.PushBack((_u16)2);
	result.indices_.PushBack((_u16)0); result.indices_.PushBack((_u16)2); result.indices_.PushBack((_u16)3);
	result.pTexture_ = nullptr;
	return Build2DPrimitive(_pDevice, _pVs, result, "Rect2D");
}

//////////////////////////////////////////////////////////////////////////////////////////
// 단위원 (반지름 1). 중심이 원점 — 스케일로 반지름을 조절한다. (VertexPTC)
bool Mesh::InitializeAsCircle2D(GraphicDevice* _pDevice, VertexShader* _pVs, _u32 _segments)
{
	FillResult result;
	PrimitiveBuilder::BuildCircle(vec2::Zero(), 1.0f, color::WHITE, _segments, result);
	return Build2DPrimitive(_pDevice, _pVs, result, "Circle2D");
}

//////////////////////////////////////////////////////////////////////////////////////////
// 단위 삼각형. 중심이 원점, 시계 반대 방향(앞면) 감기. (VertexPTC)
bool Mesh::InitializeAsTriangle2D(GraphicDevice* _pDevice, VertexShader* _pVs)
{
	const vec2 pTop(0.0f, 0.5f);			// 위 꼭짓점
	const vec2 pBottomLeft(-0.5f, -0.5f);	// 왼아래
	const vec2 pBottomRight(0.5f, -0.5f);	// 오른아래

	FillResult result;
	PrimitiveBuilder::BuildTriangle(pTop, pBottomLeft, pBottomRight, color::WHITE, result);
	return Build2DPrimitive(_pDevice, _pVs, result, "Triangle2D");
}

//////////////////////////////////////////////////////////////////////////////////////////
// 단위 선분 — 가로 길이 1, 두께 1, 중심 원점. (VertexPTC)
// Transform: scale=(길이, 두께, 1), rotation=각도, position=중점.
bool Mesh::InitializeAsLine2D(GraphicDevice* _pDevice, VertexShader* _pVs)
{
	FillResult result;
	PrimitiveBuilder::BuildLine(vec2(-0.5f, 0.0f), vec2(0.5f, 0.0f), 1.0f, color::WHITE, result);
	return Build2DPrimitive(_pDevice, _pVs, result, "Line2D");
}

//////////////////////////////////////////////////////////////////////////////////////////
// [공용] FillResult 기하 → GPU 메시 + vfPTC2D 표기. 2D 팩토리 전용. (CPU 미러 없음)
bool Mesh::Build2DPrimitive(GraphicDevice* _pDevice, VertexShader* _pVs,
	FillResult& _result, const jc::String& _pName)
{
	if (_result.vertices_.Size() == 0 || _result.indices_.Size() == 0)
	{
		return false;
	}

	// GPU 초기화용 _u32 인덱스 배열 (Mesh::Initialize 입력 형식 — 임시로만 사용)
	jc::Vector<_u32> indices32;
	indices32.Reserve((_s32)_result.indices_.Size());
	for (_u16 index : _result.indices_)
	{
		indices32.PushBack((_u32)index);
	}

	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pDescs = VertexPTC::LayoutDescs(&layoutCount);

	if (!Initialize(_pDevice, _result.vertices_.Source(), sizeof(VertexPTC), _result.vertices_.Size(),
		pDescs, layoutCount, _pVs, indices32.Source(), indices32.Size()))
	{
		return false;
	}

	format_ = VertexFormat::vfPTC2D;
	SetDebugName(_pName);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 1x1x1 큐브. 면마다 법선이 달라서 정점을 공유하지 않는다. (24정점/36인덱스)
bool Mesh::InitializeAsCube(GraphicDevice* _pDevice, VertexShader* _pVs)
{
	const VertexPNT vertices[] = {
		// +Z 앞면
		{ { -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 0.0f } },
		{ { +0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 0.0f } },
		{ { +0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 1.0f } },
		// -Z 뒷면
		{ { +0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
		{ { -0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		// -X 왼면
		{ { -0.5f, +0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { -0.5f, +0.5f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		// +X 오른면
		{ { +0.5f, +0.5f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { +0.5f, +0.5f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { +0.5f, -0.5f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -0.5f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		// +Y 윗면
		{ { -0.5f, +0.5f, -0.5f }, { 0.0f, +1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { +0.5f, +0.5f, -0.5f }, { 0.0f, +1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { +0.5f, +0.5f, +0.5f }, { 0.0f, +1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, +0.5f, +0.5f }, { 0.0f, +1.0f, 0.0f }, { 0.0f, 1.0f } },
		// -Y 아랫면
		{ { -0.5f, -0.5f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { +0.5f, -0.5f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { +0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
	};

	_u32 indices[36];
	for (_u32 face = 0; face < 6; ++face)
	{
		const _u32 base = face * 4;
		indices[face * 6 + 0] = base + 0;
		indices[face * 6 + 1] = base + 1;
		indices[face * 6 + 2] = base + 2;
		indices[face * 6 + 3] = base + 0;
		indices[face * 6 + 4] = base + 2;
		indices[face * 6 + 5] = base + 3;
	}

	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pDescs = VertexPNT::LayoutDescs(&layoutCount);

	if (!Initialize(_pDevice, vertices, sizeof(VertexPNT), _countof(vertices),
		pDescs, layoutCount, _pVs, indices, _countof(indices)))
	{
		return false;
	}

	SetDebugName("Cube");
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 반지름 1 구 (VertexPNT). UV 스피어 — slices(경도)/stacks(위도) 그리드, 북극→남극.
bool Mesh::InitializeAsSphere(GraphicDevice* _pDevice, VertexShader* _pVs, _u32 _slices, _u32 _stacks)
{
	jc_assert_msg(_slices >= 3 && _stacks >= 2, "구는 slices>=3, stacks>=2 필요합니다.");

	const _f32 radius = 1.0f;
	const _s32 ringCount = (_s32)_stacks + 1;	// 위도 고리 수 (북극~남극)
	const _s32 colCount = (_s32)_slices + 1;	// 경도 열 수 (시임 중복 포함)

	jc::Vector<VertexPNT> vertices;
	vertices.Reserve(ringCount * colCount);
	for (_s32 i = 0; i < ringCount; ++i)
	{
		const _f32 phi = jc_math_pi * (_f32)i / (_f32)_stacks;		// 0(북극) ~ pi(남극)
		const _f32 sinPhi = sinf(phi);
		const _f32 cosPhi = cosf(phi);
		for (_s32 j = 0; j < colCount; ++j)
		{
			const _f32 theta = jc_math_pi2 * (_f32)j / (_f32)_slices;	// 0 ~ 2pi
			const _f32 sinTheta = sinf(theta);
			const _f32 cosTheta = cosf(theta);

			VertexPNT vertex;
			vertex.position_ = vec3(radius * sinPhi * cosTheta, radius * cosPhi, radius * sinPhi * sinTheta);
			vertex.normal_ = vec3(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);	// 단위 구 = 법선
			vertex.uv_ = vec2((_f32)j / (_f32)_slices, (_f32)i / (_f32)_stacks);
			vertices.PushBack(vertex);
		}
	}

	jc::Vector<_u32> indices;
	indices.Reserve((_s32)_stacks * (_s32)_slices * 6);
	for (_s32 i = 0; i < (_s32)_stacks; ++i)
	{
		for (_s32 j = 0; j < (_s32)_slices; ++j)
		{
			const _u32 v00 = (_u32)(i * colCount + j);
			const _u32 v10 = (_u32)((i + 1) * colCount + j);
			const _u32 v01 = (_u32)(i * colCount + j + 1);
			const _u32 v11 = (_u32)((i + 1) * colCount + j + 1);
			indices.PushBack(v00); indices.PushBack(v10); indices.PushBack(v01);
			indices.PushBack(v01); indices.PushBack(v10); indices.PushBack(v11);
		}
	}

	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pDescs = VertexPNT::LayoutDescs(&layoutCount);

	if (!Initialize(_pDevice, vertices.Source(), sizeof(VertexPNT), vertices.Size(),
		pDescs, layoutCount, _pVs, indices.Source(), indices.Size()))
	{
		return false;
	}

	format_ = VertexFormat::vfPNT3D;
	SetDebugName("Sphere");
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 반지름 1, 높이 2 원기둥 (VertexPNT). 옆면 + 상하 원판 (y ±1, 중심 원점).
bool Mesh::InitializeAsCylinder(GraphicDevice* _pDevice, VertexShader* _pVs, _u32 _segments)
{
	jc_assert_msg(_segments >= 3, "원기둥은 segments>=3 필요합니다.");

	const _f32 radius = 1.0f;
	const _f32 halfHeight = 1.0f;	// y -1 ~ +1 (전체 높이 2)
	const _s32 n = (_s32)_segments;

	// 정점: 아랫원판 중심(0) + 위원판 중심(1) + 아랫링(n+1) + 위링(n+1)
	jc::Vector<VertexPNT> vertices;
	vertices.Reserve(2 + 2 * (n + 1));
	vertices.PushBack(VertexPNT{ vec3(0.0f, -halfHeight, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec2(0.5f, 0.5f) });
	vertices.PushBack(VertexPNT{ vec3(0.0f, +halfHeight, 0.0f), vec3(0.0f, +1.0f, 0.0f), vec2(0.5f, 0.5f) });

	const _s32 bottomRing = 2;
	for (_s32 j = 0; j <= n; ++j)
	{
		const _f32 theta = jc_math_pi2 * (_f32)j / (_f32)n;
		const _f32 c = cosf(theta);
		const _f32 s = sinf(theta);
		vertices.PushBack(VertexPNT{ vec3(c * radius, -halfHeight, s * radius), vec3(c, 0.0f, s), vec2((_f32)j / (_f32)n, 1.0f) });
	}

	const _s32 topRing = bottomRing + (n + 1);
	for (_s32 j = 0; j <= n; ++j)
	{
		const _f32 theta = jc_math_pi2 * (_f32)j / (_f32)n;
		const _f32 c = cosf(theta);
		const _f32 s = sinf(theta);
		vertices.PushBack(VertexPNT{ vec3(c * radius, +halfHeight, s * radius), vec3(c, 0.0f, s), vec2((_f32)j / (_f32)n, 0.0f) });
	}

	jc::Vector<_u32> indices;
	indices.Reserve(n * 12);
	// 옆면 (바깥에서 볼 때 시계방향): 위링→아랫링→다음 위링 셀
	for (_s32 j = 0; j < n; ++j)
	{
		const _u32 b0 = (_u32)(bottomRing + j);
		const _u32 b1 = (_u32)(bottomRing + j + 1);
		const _u32 t0 = (_u32)(topRing + j);
		const _u32 t1 = (_u32)(topRing + j + 1);
		indices.PushBack(t0); indices.PushBack(b0); indices.PushBack(t1);
		indices.PushBack(t1); indices.PushBack(b0); indices.PushBack(b1);
	}
	// 아랫원판 (법선 -Y): 중심→다음→현재 (밖에서 볼 때 시계방향)
	for (_s32 j = 0; j < n; ++j)
	{
		const _u32 v0 = (_u32)(bottomRing + j);
		const _u32 v1 = (_u32)(bottomRing + j + 1);
		indices.PushBack((_u32)0); indices.PushBack(v1); indices.PushBack(v0);
	}
	// 위원판 (법선 +Y): 중심→현재→다음
	for (_s32 j = 0; j < n; ++j)
	{
		const _u32 v0 = (_u32)(topRing + j);
		const _u32 v1 = (_u32)(topRing + j + 1);
		indices.PushBack((_u32)1); indices.PushBack(v0); indices.PushBack(v1);
	}

	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pDescs = VertexPNT::LayoutDescs(&layoutCount);

	if (!Initialize(_pDevice, vertices.Source(), sizeof(VertexPNT), vertices.Size(),
		pDescs, layoutCount, _pVs, indices.Source(), indices.Size()))
	{
		return false;
	}

	format_ = VertexFormat::vfPNT3D;
	SetDebugName("Cylinder");
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 반지름 0.5, 전체 높이 2 캡슐 (VertexPNT). 원통부(높이 1) + 상하 반구 (Unity 규격).
bool Mesh::InitializeAsCapsule(GraphicDevice* _pDevice, VertexShader* _pVs, _u32 _segments)
{
	jc_assert_msg(_segments >= 3, "캡슐은 segments>=3 필요합니다.");

	const _f32 radius = 0.5f;
	const _f32 cylinderHalf = 0.5f;		// 원통부 y ±0.5 → 전체 높이 2
	const _s32 n = (_s32)_segments;
	const _s32 H = 4;					// 반구 분할 수 (위/아래 각각)
	const _s32 C = 4;					// 원통부 세로 분할 수
	const _s32 totalRows = 2 * H + C;	// 0(북극) ~ totalRows(남극)

	const _s32 colCount = n + 1;
	jc::Vector<VertexPNT> vertices;
	vertices.Reserve((totalRows + 1) * colCount);

	for (_s32 i = 0; i <= totalRows; ++i)
	{
		_f32 y, ringRadius, normalY;
		if (i <= H)		// 위 반구 (북극 → 위 원통 경계)
		{
			const _f32 phi = jc_math_pi_half * (_f32)i / (_f32)H;
			y = cylinderHalf + radius * cosf(phi);
			ringRadius = radius * sinf(phi);
			normalY = cosf(phi);
		}
		else if (i <= H + C)	// 원통부 (위 경계 → 아래 경계)
		{
			const _f32 t = (_f32)(i - H) / (_f32)C;
			y = cylinderHalf - t * (cylinderHalf * 2.0f);
			ringRadius = radius;
			normalY = 0.0f;
		}
		else				// 아래 반구 (아래 원통 경계 → 남극)
		{
			const _f32 phi = jc_math_pi_half * (_f32)(totalRows - i) / (_f32)H;
			y = -cylinderHalf - radius * cosf(phi);
			ringRadius = radius * sinf(phi);
			normalY = -cosf(phi);
		}

		// 법선 수평 성분 = sqrt(1 - normalY^2) — 반구(sinφ)와 원통부(1)를 일관되게 처리
		const _f32 horizontal = sqrtf(1.0f - normalY * normalY);
		for (_s32 j = 0; j < colCount; ++j)
		{
			const _f32 theta = jc_math_pi2 * (_f32)j / (_f32)n;
			const _f32 c = cosf(theta);
			const _f32 s = sinf(theta);

			VertexPNT vertex;
			vertex.position_ = vec3(c * ringRadius, y, s * ringRadius);
			vertex.normal_ = vec3(c * horizontal, normalY, s * horizontal);
			vertex.uv_ = vec2((_f32)j / (_f32)n, (_f32)i / (_f32)totalRows);
			vertices.PushBack(vertex);
		}
	}

	jc::Vector<_u32> indices;
	indices.Reserve(totalRows * n * 6);
	for (_s32 i = 0; i < totalRows; ++i)
	{
		for (_s32 j = 0; j < n; ++j)
		{
			const _u32 v00 = (_u32)(i * colCount + j);
			const _u32 v10 = (_u32)((i + 1) * colCount + j);
			const _u32 v01 = (_u32)(i * colCount + j + 1);
			const _u32 v11 = (_u32)((i + 1) * colCount + j + 1);
			indices.PushBack(v00); indices.PushBack(v10); indices.PushBack(v01);
			indices.PushBack(v01); indices.PushBack(v10); indices.PushBack(v11);
		}
	}

	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pDescs = VertexPNT::LayoutDescs(&layoutCount);

	if (!Initialize(_pDevice, vertices.Source(), sizeof(VertexPNT), vertices.Size(),
		pDescs, layoutCount, _pVs, indices.Source(), indices.Size()))
	{
		return false;
	}

	format_ = VertexFormat::vfPNT3D;
	SetDebugName("Capsule");
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// XY 1x1 평면 (VertexPNT). +Z 향, 8x8 격자 (Unity Plane 형태). 중심 원점.
bool Mesh::InitializeAsPlane(GraphicDevice* _pDevice, VertexShader* _pVs)
{
	constexpr _s32 SEGMENTS = 8;
	const _f32 half = 0.5f;
	const _s32 colCount = SEGMENTS + 1;

	jc::Vector<VertexPNT> vertices;
	vertices.Reserve(colCount * colCount);
	for (_s32 gy = 0; gy <= SEGMENTS; ++gy)
	{
		const _f32 y = -half + (_f32)gy / (_f32)SEGMENTS;	// -0.5 ~ +0.5
		for (_s32 gx = 0; gx <= SEGMENTS; ++gx)
		{
			const _f32 x = -half + (_f32)gx / (_f32)SEGMENTS;

			VertexPNT vertex;
			vertex.position_ = vec3(x, y, 0.0f);
			vertex.normal_ = vec3(0.0f, 0.0f, 1.0f);
			vertex.uv_ = vec2((_f32)gx / (_f32)SEGMENTS, (_f32)gy / (_f32)SEGMENTS);
			vertices.PushBack(vertex);
		}
	}

	jc::Vector<_u32> indices;
	indices.Reserve(SEGMENTS * SEGMENTS * 6);
	for (_s32 i = 0; i < SEGMENTS; ++i)
	{
		for (_s32 j = 0; j < SEGMENTS; ++j)
		{
			const _u32 v00 = (_u32)(i * colCount + j);
			const _u32 v10 = (_u32)((i + 1) * colCount + j);
			const _u32 v01 = (_u32)(i * colCount + j + 1);
			const _u32 v11 = (_u32)((i + 1) * colCount + j + 1);
			indices.PushBack(v00); indices.PushBack(v10); indices.PushBack(v01);
			indices.PushBack(v01); indices.PushBack(v10); indices.PushBack(v11);
		}
	}

	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pDescs = VertexPNT::LayoutDescs(&layoutCount);

	if (!Initialize(_pDevice, vertices.Source(), sizeof(VertexPNT), vertices.Size(),
		pDescs, layoutCount, _pVs, indices.Source(), indices.Size()))
	{
		return false;
	}

	format_ = VertexFormat::vfPNT3D;
	SetDebugName("Plane");
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// XY 1x1 쿼드 (VertexPNT). +Z 향, 4정점/2삼각형. 중심 원점.
bool Mesh::InitializeAsQuad3D(GraphicDevice* _pDevice, VertexShader* _pVs)
{
	const VertexPNT vertices[4] = {
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, +0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
		{ { +0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, +0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	};

	const _u32 indices[6] = { 0, 1, 2, 2, 1, 3 };

	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pDescs = VertexPNT::LayoutDescs(&layoutCount);

	if (!Initialize(_pDevice, vertices, sizeof(VertexPNT), _countof(vertices),
		pDescs, layoutCount, _pVs, indices, _countof(indices)))
	{
		return false;
	}

	format_ = VertexFormat::vfPNT3D;
	SetDebugName("Quad3D");
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void Mesh::Finalize()
{
	inputLayout_.Finalize();
	indexed_ = false;
	format_ = VertexFormat::vfCustom;
}

//////////////////////////////////////////////////////////////////////////////////////////
// ★ IA(입력 조립기) 세팅 — "이 메시를 어떻게 해석할지"를 컨텍스트에 알린다.
// - 하는 일: 입력 레이아웃(정점 해석법) + 정점 버퍼 + (있으면) 인덱스 버퍼 + 토폴로지(삼각형/선 등).
// - Bind(세팅)와 Draw(발사)가 분리된 이유: 한 번 세팅해 두면 같은 메시를 여러 번 그릴 수 있다.
void Mesh::Bind(GraphicContext& _context)
{
	_context.SetInputLayout(&inputLayout_);
	_context.SetVertexBuffer(&vertexBuffer_);
	if (indexed_)
	{
		_context.SetIndexBuffer(&indexBuffer_);
	}
	_context.SetPrimitiveTopology(topology_);
}

//////////////////////////////////////////////////////////////////////////////////////////
// ★ 그리기 발사 — 정점 수만큼 GPU 파이프라인을 통과시킨다.
// - 인덱스 메시는 DrawIndexed(점을 아껴 쓰는 방식), 아니면 Draw(정점 나열 방식).
void Mesh::Draw(GraphicContext& _context)
{
	if (indexed_)
	{
		_context.DrawIndexed(indexBuffer_.Count());
	}
	else
	{
		_context.Draw(vertexBuffer_.Count());
	}
}

NS_SGF_END
