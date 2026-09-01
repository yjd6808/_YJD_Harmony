/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:40:00 PM
 * 수정일: 8/16/2026 (2D 프리미티브 팩토리 + CPU 미러 + VertexFormat)
 * =====================
 * 메시 (정점/인덱스 버퍼 + 토폴로지 묶음)
 *
 * [메시란?]
 * "무엇을 그릴지"의 형태 데이터. 재질(Material)과 분리되어 있어서
 * 같은 메시를 다른 재질로, 같은 재질을 다른 메시에 쓸 수 있다.
 *
 *  [— 2D 프리미티브]
 *  Rect/Circle/Triangle/Line 같은 기본 도형 메시를 만들 수 있다. (모두 VertexPTC)
 *  단위 도형(중심 원점)이라 Transform의 scale/rotation으로 크기·회전을 제어한다.
 *  정점은 로컬 공간에서 불변 — 월드 변환은 Renderer2D가 GPU(b1 상수버퍼)로 수행한다.
 *
 * [StaticLevel — GameObject 전담 (Mesh는 무관)]
 *  정적/동적 개념은 Mesh에 없다. GameObject::SetMeshStaticLevel(slStatic)이
 *  "월드 행렬 고정"을 수행하며, 검사는 GameObject::GetMeshStaticLevel() 하나로 한다.
 *
 * [소유 관계]
 * Mesh는 VB/IB를 직접 소유한다. (하나의 형태 = 하나의 버퍼 묶음)
 * IResource를 상속받아 ResourceMgr에 등록/공유할 수 있다.
 * InputLayout은 더 이상 Mesh가 소유하지 않는다 — 선언×VS 조합으로 Draw 직전에 결정된다.
 */

#pragma once

#include "sgf/Graphics/IResource.h"
#include "sgf/Graphics/GraphicsEnums.h"
#include "sgf/Graphics/Buffers.h"
#include "sgf/Graphics/Vertex.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;
class GraphicContext;
struct FillResult;

// 정점 형식 — 2D 메시(VertexPTC) 판별용. (CPU 미러 개념은 폐기 — GPU 변환 통일)
enum class VertexFormat
{
	vfCustom,	// 범용 (3D 메시 등)
	vfPTC2D,	// 2D 프리미티브/스프라이트 (VertexPTC — Is2D 판별용)
	vfPNT3D,	// 3D 메시 (VertexPNT)
};

class Mesh : public ResourceBase
{
	SGF_RESOURCE_TYPE(rtMesh)

public:
	Mesh();
	~Mesh();

	// 임의의 정점 데이터로 메시를 만든다.
	// @param _pVertices: 정점 배열
	// @param _vertexCount: 정점 개수
	// @param _pDecl: 정점 선언 (VertexPNT::Decl() 등 — stride는 선언에서 나온다)
	// @param _pIndices: 인덱스 배열 (nullptr면 비인덱스 메시)
	// @param _indexCount: 인덱스 개수
	// @param _topology: 정점 해석 방법 (기본: 삼각형 리스트)
	bool Initialize(
		GraphicDevice* _pDevice,
		const void* _pVertices, UINT _vertexCount,
		const VertexDeclaration* _pDecl,
		const _u32* _pIndices = nullptr, UINT _indexCount = 0,
		PrimitiveTopology _topology = PrimitiveTopology::ptTriangleList);

	// XY 평면 1x1 쿼드 (VertexPTC. 2D 스프라이트/UI용. 중심 원점)
	bool InitializeAsQuad2D(GraphicDevice* _pDevice);

	// 1x1x1 큐브 (VertexPNT. 면별 법선. 중심 원점)
	bool InitializeAsCube(GraphicDevice* _pDevice);

	////////////////////////////////////////////////////////////////////////////////////////
	// 3D 프리미티브 팩토리 (— VertexPNT, GPU 전용, CPU 미러 없음)
	// Unity 표준 규격을 따른다. (중심 원점 — scale/rotation으로 제어)
	bool InitializeAsSphere(GraphicDevice* _pDevice, _u32 _slices = 16, _u32 _stacks = 8);	// 반지름 1 구
	bool InitializeAsCylinder(GraphicDevice* _pDevice, _u32 _segments = 16);	// 반지름 1, 높이 2 원기둥
	bool InitializeAsCapsule(GraphicDevice* _pDevice, _u32 _segments = 16);	// 반지름 0.5, 전체 높이 2 캡슐
	bool InitializeAsPlane(GraphicDevice* _pDevice);	// XY 1x1 +Z 향 평면 (8x8 격자)
	bool InitializeAsQuad3D(GraphicDevice* _pDevice);	// XY 1x1 +Z 향 쿼드

	////////////////////////////////////////////////////////////////////////////////////////
	// 2D 프리미티브 팩토리 (— 모두 단위 도형, 중심 원점, VertexPTC)
	// Transform scale/rotation으로 크기·회전을 제어한다. (scale = 크기, rotation = 각도)
	bool InitializeAsRect2D(GraphicDevice* _pDevice);	// 1x1 사각형 (= InitializeAsQuad2D)
	bool InitializeAsCircle2D(GraphicDevice* _pDevice, _u32 _segments = 32);	// 반지름 1 원
	bool InitializeAsTriangle2D(GraphicDevice* _pDevice);	// 단위 삼각형
	bool InitializeAsLine2D(GraphicDevice* _pDevice);	// 가로 길이 1, 두께 1 (중심 원점)

	void Finalize();

	// IA 단계에 이 메시를 장착한다. (VB/IB/토폴로지 — 선언은 VB가 자동 전파)
	void Bind(GraphicContext& _context);

	// 장착된 상태에서 그리기를 수행한다. (인덱스 유무에 따라 DrawIndexed/Draw)
	void Draw(GraphicContext& _context);

	bool IsValid() const { return vertexBuffer_.Count() > 0; }
	UINT VertexCount() const { return vertexBuffer_.Count(); }
	UINT IndexCount() const { return indexBuffer_.Count(); }
	PrimitiveTopology Topology() const { return topology_; }

	////////////////////////////////////////////////////////////////////////////////////////
	// 정점 형식 (— 2D/3D 판별용. Is2D 체크는 Renderer2D::DrawMesh가 수행)
	VertexFormat GetFormat() const { return format_; }
	bool Is2D() const { return format_ == VertexFormat::vfPTC2D; }

private:
	// FillResult 기하 → GPU 메시 + vfPTC2D 표기 (CPU 미러 없음 — GPU 변환 통일)
	bool Build2DPrimitive(GraphicDevice* _pDevice, FillResult& _result, const jc::String& _pName);

	VertexBuffer vertexBuffer_;		// 정점 버퍼 (소유 — 선언은 VB가 보관)
	IndexBuffer indexBuffer_;		// 인덱스 버퍼 (소유. 비인덱스면 비어있음)
	PrimitiveTopology topology_;	// 정점 해석 방법
	bool indexed_;					// 인덱스 메시 여부

	VertexFormat format_ = VertexFormat::vfCustom;	// 정점 형식 (2D/3D 판별)
};

NS_SGF_END
