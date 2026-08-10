/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:40:00 PM
 * =====================
 * 메시 (v3: 정점/인덱스 버퍼 + 입력 레이아웃 + 토폴로지 묶음)
 *
 * [메시란?]
 *  "무엇을 그릴지"의 형태 데이터. 재질(Material)과 분리되어 있어서
 *  같은 메시를 다른 재질로, 같은 재질을 다른 메시에 쓸 수 있다.
 *
 * [소유 관계]
 *  Mesh는 VB/IB/InputLayout을 직접 소유한다. (하나의 형태 = 하나의 버퍼 묶음)
 *  IResource를 상속받아 ResourceMgr에 등록/공유할 수 있다.
 */

#pragma once

#include "sgf/Graphics/IResource.h"
#include "sgf/Graphics/GraphicsEnums.h"
#include "sgf/Graphics/Buffers.h"
#include "sgf/Graphics/ShaderProgram.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;
class GraphicContext;

class Mesh : public ResourceBase
{
	SGF_RESOURCE_TYPE(rtMesh)

public:
	Mesh();
	~Mesh();

	// 임의의 정점 데이터로 메시를 만든다.
	// @param _pVertices    : 정점 배열
	// @param _stride       : 정점 1개 바이트 크기 (sizeof(VertexPNT) 등)
	// @param _vertexCount  : 정점 개수
	// @param _pLayoutDescs : 정점 구조 설명 배열 (VertexPNT::LayoutDescs() 등)
	// @param _layoutCount  : 설명 배열 개수
	// @param _pVs          : 입력 레이아웃 서명 검증용 버텍스 셰이더
	// @param _pIndices     : 인덱스 배열 (nullptr면 비인덱스 메시)
	// @param _indexCount   : 인덱스 개수
	// @param _topology     : 정점 해석 방법 (기본: 삼각형 리스트)
	bool Initialize(
		GraphicDevice* _pDevice,
		const void* _pVertices, UINT _stride, UINT _vertexCount,
		const D3D11_INPUT_ELEMENT_DESC* _pLayoutDescs, UINT _layoutCount,
		VertexShader* _pVs,
		const _u32* _pIndices = nullptr, UINT _indexCount = 0,
		PrimitiveTopology _topology = PrimitiveTopology::ptTriangleList);

	// XY 평면 1x1 쿼드 (VertexPTC. 2D 스프라이트/UI용. 중심 원점)
	bool InitializeAsQuad2D(GraphicDevice* _pDevice, VertexShader* _pVs);

	// 1x1x1 큐브 (VertexPNT. 면별 법선. 중심 원점)
	bool InitializeAsCube(GraphicDevice* _pDevice, VertexShader* _pVs);

	void Finalize();

	// IA 단계에 이 메시를 장착한다. (VB/IB/레이아웃/토폴로지)
	void Bind(GraphicContext& _context);

	// 장착된 상태에서 그리기를 수행한다. (인덱스 유무에 따라 DrawIndexed/Draw)
	void Draw(GraphicContext& _context);

	bool IsValid() const { return vertexBuffer_.Count() > 0; }
	UINT VertexCount() const { return vertexBuffer_.Count(); }
	UINT IndexCount() const { return indexBuffer_.Count(); }
	PrimitiveTopology Topology() const { return topology_; }

private:
	VertexBuffer vertexBuffer_;		// 정점 버퍼 (소유)
	IndexBuffer indexBuffer_;		// 인덱스 버퍼 (소유. 비인덱스면 비어있음)
	InputLayout inputLayout_;		// 입력 레이아웃 (소유)
	PrimitiveTopology topology_;	// 정점 해석 방법
	bool bIndexed_;					// 인덱스 메시 여부
};

NS_SGF_END
