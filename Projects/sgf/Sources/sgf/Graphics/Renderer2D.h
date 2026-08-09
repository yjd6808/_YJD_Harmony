/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:24:00 AM
 * =====================
 * 2D 즉시 렌더러 (Immediate-mode Renderer)
 *
 * [설계 의도]
 *  Cocos2d-x는 반드시 addChild로 씬 그래프에 넣어야 그려지지만,
 *  sgf는 이 클래스로 "그리고 싶을 때 바로 그리기"가 가능하다.
 *  씬 그래프(Node/Sprite)는 편의 기능일 뿐, 내부적으로는 결국 이 클래스를 쓴다.
 *
 * [배치(Batch) 렌더링 원리]
 *  DrawXXX 호출마다 바로 GPU 명령을 내리면 느리다. (드로우 콜 비용이 큼)
 *  그래서 정점들을 CPU 배열에 모아두었다가(배치),
 *  텍스처가 바뀌거나 Flush 시점에 한 번에 GPU로 보낸다.
 *
 * [사용 순서]
 *  Begin(뷰프로젝션 행렬) -> DrawXXX ... -> End()
 *
 * [구조]
 *  Begin/End/Initialize의 공통 흐름은 BatchRenderer 베이스가 처리하고,
 *  이 클래스는 정점/인덱스 버퍼와 흰색 텍스처, 드로우 콜만 담당한다.
 */

#pragma once

#include "jc/Math.h"
#include "sgf/Graphics/Vertex.h"
#include "sgf/Graphics/BatchRenderer.h"

#include <jc/Container/Vector.h>

NS_SGF_BEGIN

using namespace jc;

class Texture;

class Renderer2D : public BatchRenderer
{
public:
	Renderer2D();
	virtual ~Renderer2D() override;

	// 흰색 텍스처 해제 (공통 리소스는 BatchRenderer::Finalize가 처리)
	virtual void Finalize() override;

	// === 그리기 API (좌표는 월드 좌표계) ===

	// 단색 사각형 (중심 기준)
	// @param _center : 사각형 중심 위치
	// @param _size   : 가로/세로 크기
	// @param _color  : 색상
	// @param _radian : Z축 회전각 (반시계 +)
	void DrawRect(const vec2& _center, const vec2& _size, const color& _color, _f32 _radian = 0.0f);

	// 텍스처 스프라이트 (중심 기준)
	// @param _pTexture : 그릴 텍스처
	// @param _center   : 중심 위치
	// @param _size     : 출력 크기 (텍스처 원본 크기와 무관)
	// @param _tint     : 색상 틴트 (흰색이면 원본 그대로)
	// @param _radian   : Z축 회전각
	// @param _uvMin    : 텍스처 영역 좌상단 UV (기본 전체)
	// @param _uvMax    : 텍스처 영역 우하단 UV
	void DrawSprite(Texture* _pTexture, const vec2& _center, const vec2& _size,
		const color& _tint = color(1.0f, 1.0f, 1.0f, 1.0f), _f32 _radian = 0.0f,
		const vec2& _uvMin = vec2(0.0f, 0.0f), const vec2& _uvMax = vec2(1.0f, 1.0f));

	// 선분 (두께 있는 사각형으로 그린다)
	// @param _from/_to : 양 끝점
	// @param _thickness: 선 두께 (픽셀)
	void DrawLine(const vec2& _from, const vec2& _to, const color& _color, _f32 _thickness = 1.0f);

	// 원 (삼각형 부채꼴로 근사)
	// @param _segments : 분할 수. 클수록 매끄럽지만 정점이 늘어난다.
	void DrawCircle(const vec2& _center, _f32 _radius, const color& _color, int _segments = 32);

	// 모아둔 배치를 즉시 GPU로 보낸다. (보통 End가 알아서 호출)
	virtual void Flush() override;

protected:
	// === BatchRenderer 훅 구현 ===
	virtual const char* ShaderSource() const override;
	virtual const D3D11_INPUT_ELEMENT_DESC* VertexLayout(UINT* _outCount) const override;
	virtual bool CreateBatchResources(GraphicDevice* _pDevice) override;
	virtual void OnBegin() override;

private:
	// 사각형 4정점을 배치에 추가하는 공통 헬퍼
	// 텍스처가 바뀌면 자동으로 먼저 Flush한다.
	void PushQuad(Texture* _pTexture, const VertexPTC (&_vertices)[4]);

private:
	static const int MAX_QUADS = 2048;					// 배치 한 번에 담을 최대 사각형 수
	static const int MAX_VERTICES = MAX_QUADS * 4;	// 최대 정점 수
	static const int MAX_INDICES = MAX_QUADS * 6;		// 최대 인덱스 수

	VertexBuffer vertexBuffer_;			// DYNAMIC 정점 버퍼
	IndexBuffer indexBuffer_;				// 사각형용 고정 인덱스 버퍼
	Texture* pWhiteTexture_;				// 1x1 흰색 텍스처 (단색 도형용)

	jc::Vector<VertexPTC> vertices_;		// CPU 쪽 정점 배치
	Texture* pCurrentTexture_;				// 현재 배치가 사용 중인 텍스처
};

NS_SGF_END
