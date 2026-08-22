/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:24:00 AM
 * 수정일: 8/16/2026 (예약/그리기 분리 — DeclareStatic/RenderStatic/RenderDynamic + FlushStatic/Dynamic)
 * =====================
 * 2D 배치 렌더러 (— 예약/그리기 분리 + 스태틱 캐시)
 *
 * [즉시 모드]
 * Begin(행렬) -> DrawRect/DrawSprite ... -> End()
 * 호출 즉시 CPU 배치에 쌓고 Flush 시점에 GPU로 보낸다. (씬 밖/디버그 용도로 유지)
 *
 * [예약/그리기 분리]
 * 게임 코드는 "그리기 예약"만 하고, 실제 GPU 드로우는 RenderScene 끝의
 * FlushStatic/FlushDynamic이 한 번에 처리한다. (트리 순회 순서 = 드로우 순서)
 *
 * 1. DeclareStatic(params): 선언 = 채움 1회 bake → 캐시 저장 → staticId (OnEnter에서 호출)
 * 2. RenderStatic(staticId): 예약 = 캐시 참조만 (매 프레임 OnRender에서 호출)
 * 3. RenderDynamic(params): 예약 = 파라미터 복사 (매 프레임, FlushDynamic이 bake)
 * 4. FlushStatic(): 스태틱 세그먼트 전부 드로우 (버킷 순서 Bottom→Default→Top)
 * 5. FlushDynamic(): 다이나믹 세그먼트 전부 드로우 (버킷 순서)
 *
 * [버킷] RenderParams.layer_ (Bottom/Default/Top) — 트리와 무관한 씬 전역 최상단/최하단.
 * [배칭] 같은 텍스처 연속분은 PushTriangles가 하나의 DrawCall로 모은다. (기존과 동일)
 * [임의 도형] PushTriangles — 9-패치/폴리곤/원 등 FillResult가 쿼드가 아닌 도형도 수용.
 *
 * [사용 순서]
 * Scene2D::RenderScene이 BeginScene(this) → FlushStatic/FlushDynamic → EndScene을 자동 수행.
 * 씬 밖(디버그)에서는 기존 Begin/End + DrawRect 등 즉시 API를 그대로 쓴다.
 */

#pragma once

#include "jc/Math.h"
#include "jc/Container/Vector.h"
#include "jc/Pool/IdProvider.h"
#include "sgf/Graphics/Vertex.h"
#include "sgf/Graphics/BatchRenderer.h"
#include "sgf/Graphics/Fill.h"
#include "sgf/Graphics/Renderer3D.h"	// ObjectConstants (b1 오브젝트 상수)
#include "sgf/Graphics/Material.h"	// MaterialConstants (b2 머티리얼 상수)

NS_SGF_BEGIN

using namespace jc;

class Scene2D;
class Mesh;
class Material;
class Texture;

class Renderer2D : public BatchRenderer
{
public:
	Renderer2D();
	virtual ~Renderer2D() override;

	// 스태틱 캐시 정리 (공통 리소스는 BatchRenderer::Finalize가 처리)
	virtual void Finalize() override;

	////////////////////////////////////////////////////////////////////////////////////////
	// 씬 배치 수명 (— 씬 카메라 기반, Scene2D::RenderScene이 자동 호출)
	void BeginScene(Scene2D* _pScene);	// 씬 카메라 ViewProjection으로 배치 시작
	void EndScene();					// 잔여 세그먼트 플러시 + 배치 종료

	////////////////////////////////////////////////////////////////////////////////////////
	// 예약 (— 씬 트래버설 중 호출, GPU 무접촉)
	_u64 DeclareStatic(const RenderParams& _params);	// 선언 = 1회 빌드 → 캐시 → staticId (채움·버킷은 _params.fill_/_params.layer_)
	void RenderStatic(_u64 _staticId);					// 선언 시 고정 — id만 예약
	void RenderDynamic(const RenderParams& _params);

	////////////////////////////////////////////////////////////////////////////////////////
	// 플러시 (— 드로우 스트림 순회)
	// 버킷 순서 고정: Bottom → Default(트리 밴드) → Top. 버킷 경계에서 Flush() 분리 — 버킷 간 절대 섞이지 않음
	void FlushStatic();		// 스태틱 세그먼트 드로우 (캐시 버텍스, 텍스처 배칭)
	void FlushDynamic();	// 다이나믹 세그먼트 드로우 (스크래치 버텍스, 텍스처 배칭)

	////////////////////////////////////////////////////////////////////////////////////////
	// 즉시 그리기 (기존 유지 — 씬 밖/디버그, 하위 호환)
	void DrawRect(const vec2& _center, const vec2& _size, const color& _color, _f32 _radian = 0.0f);
	void DrawSprite(Texture* _pTexture, const vec2& _center, const vec2& _size,
		const color& _tint = color::WHITE, _f32 _radian = 0.0f,
		const vec2& _uvMin = vec2(0.0f, 0.0f), const vec2& _uvMax = vec2(1.0f, 1.0f));
	void DrawLine(const vec2& _from, const vec2& _to, const color& _color, _f32 _thickness = 1.0f);
	void DrawCircle(const vec2& _center, _f32 _radius, const color& _color, _s32 _segments = 32);

	////////////////////////////////////////////////////////////////////////////////////////
	// 메시 그리기 (— Mesh+Material 자동 드로우, GPU 변환 즉시 드로우)
	// 2D 프리미티브 메시(vfPTC2D)만 지원. 로컬 정점을 그대로 그리며 월드 변환은 GPU(b1)가 수행.
	// 배치 경로와 달리 오브젝트당 1콜. 트래버설 중 GameObject::RenderSelf → Scene2D::DrawMesh → 이곳.
	void DrawMesh(Mesh* _pMesh, Material* _pMaterial, const mat4& _world);

	// 모아둔 배치를 즉시 GPU로 보낸다. (보통 End가 알아서 호출)
	virtual void Flush() override;

	////////////////////////////////////////////////////////////////////////////////////////
	// 드로콜 카운터 (프레임당 — 배칭 검증용)
	// Flush()가 실제 GPU 드로우콜(DrawIndexed)을 실행할 때마다 증가, OnBegin에서 리셋된다.
	// 2D 프리미티브들이 같은 텍스처로 연속되면 한 프레임에 1이 나와야 한다.
	_u32 GetDrawCallCount() const { return drawCallCount_; }

protected:
	////////////////////////////////////////////////////////////////////////////////////////
	// BatchRenderer 훅 구현
	virtual const char* ShaderSource() const override;
	virtual const D3D11_INPUT_ELEMENT_DESC* VertexLayout(UINT* _outCount) const override;
	virtual bool CreateBatchResources(GraphicDevice* _pDevice) override;
	virtual void OnBegin() override;

private:
	// 임의 정점/인덱스를 배치에 추가. 텍스처가 바뀌거나 버퍼가 꽉 차면 자동 Flush.
	// FillResult(쿼드가 아닌 9-패치/폴리곤/원 등)를 세그먼트에 배치한다.
	void PushTriangles(Texture* _pTexture, const VertexPTC* _pVertices, _u32 _vertexCount,
		const _u16* _pIndices, _u32 _indexCount);

	// 사각형 4정점을 배치에 추가하는 공통 헬퍼 (즉시 그리기용)
	void PushQuad(Texture* _pTexture, const VertexPTC (&_vertices)[4]);

	////////////////////////////////////////////////////////////////////////////////////////
	// 스태틱 캐시
	struct StaticSlot
	{
		FillResult geometry_;	// 채움 결과 (프레임 간 재사용 — 변경 없음)
		RenderLayer layer_;		// 렌더 버킷 (선언 시 고정)
	};
	StaticSlot* FindStatic(_u64 _staticId);
	void BuildFill(const Fill& _fill, const RenderParams& _params, FillResult& _out);   // 채움 콜백 호출

	// 드로우 스트림 항목 — 포인터가 아닌 "인덱스/키"로 참조해 벡터 재배치에 안전하다.
	struct DrawItem
	{
		bool isStatic_;		// true: staticCache_ 키, false: dynamicParams_ 인덱스
		_u64  key_;			// 스태틱: staticId / 다이나믹: dynamicParams_ 인덱스
		Texture* pTexture_;	// 세그먼트 배칭용
		RenderLayer layer_;	// 렌더 버킷 (예약 시 저장 — Flush가 3패스로 순회)
	};

private:
	static const _s32 MAX_QUADS = 2048;				// 배치 한 번에 담을 최대 사각형 수
	static const _s32 MAX_VERTICES = MAX_QUADS * 4;	// 최대 정점 수
	static const _s32 MAX_INDICES = 32768;			// 최대 인덱스 수 (임의 도형 여유분 포함)

	VertexBuffer vertexBuffer_;	// DYNAMIC 정점 버퍼
	IndexBuffer indexBuffer_;	// DYNAMIC 인덱스 버퍼 (임의 인덱스 지원)

	jc::Vector<VertexPTC> vertices_;	// CPU 쪽 정점 배치
	jc::Vector<_u32> indices_;			// CPU 쪽 인덱스 배치
	Texture* pCurrentTexture_;			// 현재 배치가 사용 중인 텍스처

	// 메시 GPU 파이프라인 상수 버퍼 (— Renderer3D와 동일 규약. 배치 경로는 b1=단위행렬/b2=흰색 고정)
	ConstantBuffer<ObjectConstants> objectCb_;		// b1 — 월드 행렬 (오브젝트당 1회 갱신)
	ConstantBuffer<MaterialConstants> materialCb_;	// b2 — 머티리얼 기본색/틴트

	////////////////////////////////////////////////////////////////////////////////////////
	// 드로우 스트림 + 스태틱 캐시
	jc::Vector<DrawItem> staticItems_;			// 스태틱 세그먼트 (이번 프레임)
	jc::Vector<DrawItem> dynamicItems_;			// 다이나믹 세그먼트 (이번 프레임)
	jc::Vector<RenderParams> dynamicParams_;	// 다이나믹 파라미터 보관 (수명 안정화 — 인덱스 참조)
	jc::Vector<StaticSlot> staticCache_;		// staticId-1 → 슬롯 (선언 시 확장, 순수 증가)
	IdProvider<_u64> staticIdProvider_;			// staticId 발급기 (순수 증가)
	FillResult scratch_;						// 다이나믹 채움 결과 스크래치 (프레임 내 재사용 — 할당 없음)

	Scene2D* pScene_ = nullptr;					// BeginScene에서 기억 (다음 프레임까지)
	_u32 drawCallCount_ = 0;					// [프리미티브] 프레임당 실제 GPU 드로우콜 수 (배칭 검증용)
};

NS_SGF_END
