/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 9:40:00 AM
 * 수정일: 8/16/2026 (SceneRenderer 흡수 — FrameConstants/ObjectConstants/RenderObject 이동)
 * =====================
 * 3D 배치 렌더러 (신규 + SceneRenderer 통합)
 *
 * [왜 3D도 배치(batch) 방식인가?]
 * GPU에게 "그려라(DrawCall)"라고 말을 건네는 비용은 생각보다 비싸다.
 * 상자 100개를 그릴 때 100번 말을 거는 것보다,
 * 정점을 큰 버퍼에 모아두었다가 한 번에 그리는 게 훨씬 빠르다.
 * Renderer2D와 똑같은 원리다. (동적 버퍼 + 가득 차면 Flush)
 *
 * [Renderer2D와의 차이]
 * - 정점이 VertexPC(위치+색). 텍스처 없이 단색 면/선으로 그린다.
 * - 깊이 테스트를 켠다. (앞뒤 가림이 있어야 3D다워 보인다)
 * - 삼각형 배치와 선 배치를 따로 모았다가 따로 그린다.
 * (GPU는 한 번의 DrawCall에 한 가지 도형 종류만 그릴 수 있으므로)
 *
 * [SceneRenderer 흡수]
 * 기존 SceneRenderer의 메시 파이프라인(BeginScene(frame)/Draw(RenderObject))을
 * 이 클래스로 통합한다. 상수버퍼 b0(프레임)/b1(오브젝트) 소유권도 이전된다.
 * FrameConstants/ObjectConstants/RenderObject는 여기로 이동 (SceneRenderer.h 삭제).
 *
 * [사용 법]
 * // 메시 파이프라인 (Scene3D::RenderScene이 자동 호출)
 * renderer3d.BeginScene(frameData); // b0 갱신
 * renderer3d.Draw(mesh, material, world);
 * renderer3d.Draw(renderObject);
 * renderer3d.EndScene();
 *
 * // 배치 파이프라인 (디버그 도형 — 즉시 모드, 하위 호환)
 * renderer3d.Begin(camera.ViewProjection());
 * renderer3d.DrawGrid(10, 1.0f, color::BLACK);
 * renderer3d.DrawCube(vec3(0, 0.5f, 0), vec3(1, 1, 1), color::RED);
 * renderer3d.End(); // 여기서 모아둔 걸 한 번에 그린다
 *
 * [구조]
 * Begin/End/Initialize의 공통 흐름은 BatchRenderer 베이스가 처리하고,
 * 이 클래스는 삼각형/선용 동적 버퍼와 드로우 콜만 담당한다.
 */

#pragma once

#include "jc/Math.h"
#include "sgf/Graphics/Vertex.h"
#include "sgf/Graphics/BatchRenderer.h"
#include "sgf/Graphics/IResource.h"

#include <jc/Container/Vector.h>

NS_SGF_BEGIN

using namespace jc;

class Mesh;
class Material;
class GraphicContext;

// b0 프레임 공통 상수 (16바이트 배수: 64+64+16 = 144)
struct FrameConstants
{
	mat4 view_;				// 뷰 행렬
	mat4 projection_;		// 투영 행렬
	vec4 cameraPosition_;	// 카메라 월드 위치 (스페큘러 등에 사용)
};

// b1 오브젝트 상수 (64바이트)
struct ObjectConstants
{
	mat4 world_;			// 월드 행렬
};

// 그려야 할 대상 하나 = 메시(무엇을) + 머티리얼(어떻게) + 월드 행렬(어디에)
struct RenderObject
{
	_u64 meshKey_ = INVALID_RESOURCE_KEY;		// ResourceMgr의 메시 키
	_u64 materialKey_ = INVALID_RESOURCE_KEY;	// ResourceMgr의 머티리얼 키
	mat4 world_;								// 월드 행렬 (기본: 단위행렬)
	bool visible_ = true;						// false면 Draw에서 건너뀜
};

class Renderer3D : public BatchRenderer
{
public:
	// 한 번의 Flush로 그릴 수 있는 최대 삼각형/선 개수
	static constexpr _s32 MAX_TRIANGLES = 4096;
	static constexpr _s32 MAX_LINES = 4096;

public:
	Renderer3D();
	virtual ~Renderer3D() override;

	// 배치 종료 시 정리 (공통 리소스는 BatchRenderer::Finalize가 처리)
	virtual void Finalize() override;

	// 모아둔 걸 즉시 그린다. (버퍼가 가득 찼을 때도 자동 호출)
	virtual void Flush() override;

	////////////////////////////////////////////////////////////////////////////////////////
	// 프레임/메시 파이프라인 (— 기존 SceneRenderer 기능 흡수)
	// 프레임 공통 상수를 갱신하고 b0에 장착한다. (프레임당 1회 호출)
	void BeginScene(const FrameConstants& _frame);
	// 잔여 배치 플러시 + 배치 종료
	void EndScene();
	// 렌더 오브젝트 하나를 그린다. (키 해서 -> 머티리얼/메시 바인딩 -> b1 갱신 -> 드로우)
	void Draw(const RenderObject& _object);
	// 포인터로 직접 그리기 (ResourceMgr를 거치지 않는 경우용)
	void Draw(Mesh* _pMesh, Material* _pMaterial, const mat4& _world);

	////////////////////////////////////////////////////////////////////////////////////////
	// 삼각형 배치
	// 삼각형 하나를 배치에 추가한다. (정점은 시계 반대 방향 = 앞면)
	void DrawTriangle(const vec3& _p0, const vec3& _p1, const vec3& _p2, const color& _color);

	// 단색 상자를 배치에 추가한다.
	// 면마다 밝기를 살짝 다르게 해서(CPU측 간이 음영) 입체감이 느껴지게 한다.
	// @param _center: 상자 중심 (월드 좌표)
	// @param _size: 가로/세로/깊이 길이
	void DrawCube(const vec3& _center, const vec3& _size, const color& _color);

	////////////////////////////////////////////////////////////////////////////////////////
	// 선 배치
	// 3D 선분 하나를 배치에 추가한다.
	void DrawLine3D(const vec3& _from, const vec3& _to, const color& _color);

	// XZ 평면 격자를 그린다. (바닥 기준선)
	// @param _halfCount: 중심에서 한쪽으로 몇 칸 (전체 = 2*_halfCount)
	// @param _spacing: 한 칸 간격
	void DrawGrid(_s32 _halfCount, _f32 _spacing, const color& _color);

	// 원점에 X(빨강)/Y(초록)/Z(파랑) 축을 그린다.
	void DrawAxis(_f32 _length);

protected:
	////////////////////////////////////////////////////////////////////////////////////////
	// BatchRenderer 훅 구현
	virtual const char* ShaderSource() const override;
	virtual const D3D11_INPUT_ELEMENT_DESC* VertexLayout(UINT* _outCount) const override;
	virtual bool CreateBatchResources(GraphicDevice* _pDevice) override;
	virtual void OnBegin() override;

private:
	// 하나의 동적 버퍼를 GPU로 복사해 한 번의 DrawCall로 그린다.
	// (FlushTriangles/FlushLines의 공통 헬퍼)
	void FlushBatch(VertexBuffer& _vertexBuffer, jc::Vector<VertexPC>& _vertices,
		D3D11_PRIMITIVE_TOPOLOGY _topology);

private:
	VertexBuffer triangleVb_;					// 삼각형용 동적 정점 버퍼
	VertexBuffer lineVb_;						// 선용 동적 정점 버퍼

	jc::Vector<VertexPC> triangleVertices_;	// CPU측에 모아둔 삼각형 정점들
	jc::Vector<VertexPC> lineVertices_;		// CPU측에 모아둔 선 정점들

	////////////////////////////////////////////////////////////////////////////////////////
	// 메시 파이프라인 (SceneRenderer에서 이전)
	ConstantBuffer<FrameConstants> frameCb_;	// b0 (프레임 공통 — 소유권 이전)
	ConstantBuffer<ObjectConstants> objectCb_;	// b1 (오브젝트 — 소유권 이전)

	// 같은 키가 연속될 때 Find를 생략하는 소형 캐시 (D-13)
	_u64 lastMeshKey_ = INVALID_RESOURCE_KEY;
	_u64 lastMaterialKey_ = INVALID_RESOURCE_KEY;
	Mesh* pLastMesh_ = nullptr;
	Material* pLastMaterial_ = nullptr;
};

NS_SGF_END
