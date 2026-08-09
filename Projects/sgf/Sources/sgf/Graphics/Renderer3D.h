/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 9:40:00 AM
 * =====================
 * 3D 배치 렌더러 (v2.1 신규)
 *
 * [왜 3D도 배치(batch) 방식인가?]
 *  GPU에게 "그려라(DrawCall)"라고 말을 건네는 비용은 생각보다 비싸다.
 *  상자 100개를 그릴 때 100번 말을 거는 것보다,
 *  정점을 큰 버퍼에 모아두었다가 한 번에 그리는 게 훨씬 빠르다.
 *  Renderer2D와 똑같은 원리다. (동적 버퍼 + 가득 차면 Flush)
 *
 * [Renderer2D와의 차이]
 *  - 정점이 VertexPC(위치+색). 텍스처 없이 단색 면/선으로 그린다.
 *  - 깊이 테스트를 켠다. (앞뒤 가림이 있어야 3D다워 보인다)
 *  - 삼각형 배치와 선 배치를 따로 모았다가 따로 그린다.
 *    (GPU는 한 번의 DrawCall에 한 가지 도형 종류만 그릴 수 있으므로)
 *
 * [사용 법]
 *  renderer3d.Begin(camera.ViewProjection());
 *  renderer3d.DrawGrid(10, 1.0f, Color::Gray());
 *  renderer3d.DrawCube(Vec3(0, 0.5f, 0), Vec3(1, 1, 1), Color::Red());
 *  renderer3d.End();  // 여기서 모아둔 걸 한 번에 그린다
 *
 * [구조]
 *  Begin/End/Initialize의 공통 흐름은 BatchRenderer 베이스가 처리하고,
 *  이 클래스는 삼각형/선용 동적 버퍼와 드로우 콜만 담당한다.
 */

#pragma once

#include "jc/Math.h"
#include "sgf/Graphics/Vertex.h"
#include "sgf/Graphics/BatchRenderer.h"

#include <jc/Container/Vector.h>

NS_SGF_BEGIN

using namespace jc;

class Renderer3D : public BatchRenderer
{
public:
	// 한 번의 Flush로 그릴 수 있는 최대 삼각형/선 개수
	static constexpr _s32 MaxTriangles_v = 4096;
	static constexpr _s32 MaxLines_v = 4096;

public:
	Renderer3D();
	~Renderer3D() override;

	// 배치 종료 시 정리 (공통 리소스는 BatchRenderer::Finalize가 처리)
	void Finalize() override;

	// 모아둔 걸 즉시 그린다. (버퍼가 가득 찼을 때도 자동 호출)
	void Flush() override;

	// === 삼각형 배치 ===

	// 삼각형 하나를 배치에 추가한다. (정점은 시계 반대 방향 = 앞면)
	void DrawTriangle(const Vec3& _p0, const Vec3& _p1, const Vec3& _p2, const Color& _color);

	// 단색 상자를 배치에 추가한다.
	// 면마다 밝기를 살짝 다르게 해서(CPU측 간이 음영) 입체감이 느껴지게 한다.
	// @param _center : 상자 중심 (월드 좌표)
	// @param _size   : 가로/세로/깊이 길이
	void DrawCube(const Vec3& _center, const Vec3& _size, const Color& _color);

	// === 선 배치 ===

	// 3D 선분 하나를 배치에 추가한다.
	void DrawLine3D(const Vec3& _from, const Vec3& _to, const Color& _color);

	// XZ 평면 격자를 그린다. (바닥 기준선)
	// @param _halfCount : 중심에서 한쪽으로 몇 칸 (전체 = 2*_halfCount)
	// @param _spacing   : 한 칸 간격
	void DrawGrid(_s32 _halfCount, _f32 _spacing, const Color& _color);

	// 원점에 X(빨강)/Y(초록)/Z(파랑) 축을 그린다.
	void DrawAxis(_f32 _length);

protected:
	// === BatchRenderer 훅 구현 ===
	const char* ShaderSource() const override;
	const D3D11_INPUT_ELEMENT_DESC* VertexLayout(UINT* _outCount) const override;
	bool CreateBatchResources(GraphicDevice* _pDevice) override;
	void OnBegin() override;

private:
	// 하나의 동적 버퍼를 GPU로 복사해 한 번의 DrawCall로 그린다.
	// (FlushTriangles/FlushLines의 공통 헬퍼)
	void FlushBatch(VertexBuffer& _vertexBuffer, jc::Vector<VertexPC>& _vertices,
		D3D11_PRIMITIVE_TOPOLOGY _topology);

private:
	VertexBuffer m_TriangleVb;					// 삼각형용 동적 정점 버퍼
	VertexBuffer m_LineVb;						// 선용 동적 정점 버퍼

	jc::Vector<VertexPC> m_TriangleVertices;	// CPU측에 모아둔 삼각형 정점들
	jc::Vector<VertexPC> m_LineVertices;		// CPU측에 모아둔 선 정점들
};

NS_SGF_END
