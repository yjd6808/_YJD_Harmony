/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:40:00 PM
 * =====================
 * 메시 구현부
 */

#include "Core.h"
#include "sgf/Graphics/Mesh.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/GraphicContext.h"
#include "sgf/Graphics/Vertex.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
Mesh::Mesh()
	: topology_(PrimitiveTopology::ptTriangleList)
	, bIndexed_(false)
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

	bIndexed_ = (_pIndices != nullptr && _indexCount > 0);
	if (bIndexed_)
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
// XY 평면 1x1 쿼드. 중심이 원점이라 스케일 = 크기, 이동 = 위치로 바로 쓰인다.
bool Mesh::InitializeAsQuad2D(GraphicDevice* _pDevice, VertexShader* _pVs)
{
	const color white{ 1.0f, 1.0f, 1.0f, 1.0f };
	const VertexPTC vertices[] = {
		{ { -0.5f, +0.5f, 0.0f }, { 0.0f, 0.0f }, white },	// 왼위
		{ { +0.5f, +0.5f, 0.0f }, { 1.0f, 0.0f }, white },	// 오른위
		{ { +0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, white },	// 오른아래
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f }, white },	// 왼아래
	};
	const _u32 indices[] = { 0, 1, 2, 0, 2, 3 };	// 시계방향 삼각형 2개

	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pDescs = VertexPTC::LayoutDescs(&layoutCount);

	if (!Initialize(_pDevice, vertices, sizeof(VertexPTC), _countof(vertices),
		pDescs, layoutCount, _pVs, indices, _countof(indices)))
	{
		return false;
	}

	SetDebugName("Quad2D");
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
void Mesh::Finalize()
{
	inputLayout_.Finalize();
	bIndexed_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Mesh::Bind(GraphicContext& _context)
{
	_context.SetInputLayout(&inputLayout_);
	_context.SetVertexBuffer(&vertexBuffer_);
	if (bIndexed_)
	{
		_context.SetIndexBuffer(&indexBuffer_);
	}
	_context.SetPrimitiveTopology(topology_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Mesh::Draw(GraphicContext& _context)
{
	if (bIndexed_)
	{
		_context.DrawIndexed(indexBuffer_.Count());
	}
	else
	{
		_context.Draw(vertexBuffer_.Count());
	}
}

NS_SGF_END
