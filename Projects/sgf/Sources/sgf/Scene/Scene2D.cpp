/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (게임 오브젝트 시스템)
 * =====================
 * Scene2D 구현부 — 예약 창구 전달 + DeclareStatic 6종 bake + RenderScene 트래버설
 */

#include "Core.h"
#include "sgf/Scene/Scene2D.h"
#include "sgf/Graphics/Renderer2D.h"
#include "sgf/Graphics/PrimitiveBuilder.h"
#include "sgf/Graphics/Texture.h"
#include "sgf/Core/Application.h"	// g_cRenderer2D 매크로

NS_SGF_BEGIN

using namespace jc;

// 엔진 내장 6종(Line/Circle/Polygon/Triangle) 전용 채움 콜백 데이터
// DeclareStatic(→ Renderer2D::DeclareStatic → BuildFill)이 동기적으로 실행되는 동안만 유효하면 된다.
// 실제 기하 생성은 PrimitiveBuilder(공용 빌더)에 위임 — Mesh 2D 팩토리와 로직 공유.
namespace
{
	struct LineData { vec2 p1_; vec2 p2_; _f32 thickness_; };
	struct CircleData { vec2 center_; _f32 radius_; _u32 segments_; };
	struct PolygonData { const vec2* pPoints_; _u32 count_; };
	struct TriangleData { vec2 p1_; vec2 p2_; vec2 p3_; };

	// 두께 있는 선분을 사각형(2삼각형)으로 bake한다.
	void FillEngineLine(const Fill& _fill, const RenderParams& _params, FillResult& _r)
	{
		const LineData* pData = static_cast<const LineData*>(_fill.GetData());
		if (pData == nullptr) return;
		PrimitiveBuilder::BuildLine(pData->p1_, pData->p2_, pData->thickness_, _params.color1_, _r);
	}

	// 원: 중심에서 부채꼴로 뻗치는 삼각형들로 근사.
	void FillEngineCircle(const Fill& _fill, const RenderParams& _params, FillResult& _r)
	{
		const CircleData* pData = static_cast<const CircleData*>(_fill.GetData());
		if (pData == nullptr) return;
		PrimitiveBuilder::BuildCircle(pData->center_, pData->radius_, _params.color1_, pData->segments_, _r);
	}

	// 볼록 다각형: 첫 점을 중심으로 삼각형 부채꼴.
	void FillEnginePolygon(const Fill& _fill, const RenderParams& _params, FillResult& _r)
	{
		const PolygonData* pData = static_cast<const PolygonData*>(_fill.GetData());
		if (pData == nullptr) return;
		PrimitiveBuilder::BuildPolygon(pData->pPoints_, pData->count_, _params.color1_, _r);
	}

	// 삼각형 1개.
	void FillEngineTriangle(const Fill& _fill, const RenderParams& _params, FillResult& _r)
	{
		const TriangleData* pData = static_cast<const TriangleData*>(_fill.GetData());
		if (pData == nullptr) return;
		PrimitiveBuilder::BuildTriangle(pData->p1_, pData->p2_, pData->p3_, _params.color1_, _r);
	}

	// 로그 주기 제한용 프레임 카운터 (60프레임마다 1회만 출력 — A-3)
	_u32 sLogFrame = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
Scene2D::Scene2D()
{
	CreateDefaultCamera(Camera2D::CreateDefault());	// 기본 카메라 = 직교 2D (Camera2D)
}

//////////////////////////////////////////////////////////////////////////////////////////
Scene2D::~Scene2D()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Camera2D* Scene2D::GetCamera2D() const
{
	return static_cast<Camera2D*>(GetCamera());
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scene2D::RenderStatic(_u64 _staticId)
{
	g_cRenderer2D.RenderStatic(_staticId);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Scene2D::RenderDynamic(const rect& _region, const Fill& _fill,
	const color& _color1, const color& _color2, _u32 _option, RenderLayer _layer)
{
	RenderParams params;
	params.region_ = _region;
	params.color1_ = _color1;
	params.color2_ = _color2;
	params.option_ = _option;
	params.fill_  = _fill;
	params.layer_ = _layer;
	g_cRenderer2D.RenderDynamic(params);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 메시 그리기 — Mesh+Material 자동 드로우 (GameObject::RenderSelf → 이곳)
void Scene2D::DrawMesh(Mesh* _pMesh, Material* _pMaterial, const mat4& _world)
{
	g_cRenderer2D.DrawMesh(_pMesh, _pMaterial, _world);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 스태틱 선언 6종 — 모두 범용 DeclareStatic(Fill, ...)로 위임한다. (설계 4-2)
_u64 Scene2D::DeclareStaticRect(const rect& _region, const color& _color)
{
	return DeclareStatic(Fill::Solid(), _region, _color);    // 범용 선언으로 위임
}

_u64 Scene2D::DeclareStaticImage(Texture* _pTexture, const rect& _region)
{
	return DeclareStatic(Fill::Texture(_pTexture), _region, color::WHITE);
}

_u64 Scene2D::DeclareStaticImage(Texture* _pTexture, const rect& _region,
	_f32 _sliceLeft, _f32 _sliceTop, _f32 _sliceRight, _f32 _sliceBottom)
{
	return DeclareStatic(Fill::NinePatch(_pTexture, _sliceLeft, _sliceTop, _sliceRight, _sliceBottom),
		_region, color::WHITE);
}

_u64 Scene2D::DeclareStaticLine(const vec2& _p1, const vec2& _p2, _f32 _thickness, const color& _color)
{
	LineData data{ _p1, _p2, _thickness };
	return DeclareStatic(Fill::CustomData(&FillEngineLine, &data), rect(), _color);
}

_u64 Scene2D::DeclareStaticCircle(const vec2& _center, _f32 _radius, const color& _color, _u32 _segments)
{
	CircleData data{ _center, _radius, _segments };
	return DeclareStatic(Fill::CustomData(&FillEngineCircle, &data), rect(), _color);
}

_u64 Scene2D::DeclareStaticPolygon(const vec2* _pPoints, _u32 _count, const color& _color)
{
	PolygonData data{ _pPoints, _count };
	return DeclareStatic(Fill::CustomData(&FillEnginePolygon, &data), rect(), _color);
}

_u64 Scene2D::DeclareStaticTriangle(const vec2& _p1, const vec2& _p2, const vec2& _p3, const color& _color)
{
	TriangleData data{ _p1, _p2, _p3 };
	return DeclareStatic(Fill::CustomData(&FillEngineTriangle, &data), rect(), _color);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 Scene2D::DeclareStatic(const Fill& _fill, const rect& _region,
	const color& _color1, const color& _color2, _u32 _option, RenderLayer _layer)
{
	RenderParams params;
	params.region_ = _region;
	params.color1_ = _color1;
	params.color2_ = _color2;
	params.option_ = _option;
	params.fill_  = _fill;
	params.layer_ = _layer;
	return g_cRenderer2D.DeclareStatic(params);       // 선언 = bake 1회, 이후 캐시 히트
}

//////////////////////////////////////////////////////////////////////////////////////////
// ★ 2D 한 프레임의 총괄 — 예약(트리 순회) → 수동 그리기(OnRender) → 픽업(Flush) 3단계.
// - GPU와의 접촉은 ③에서 1회만 일어난다. ①②는 "그릴 것 목록 만들기"일 뿐이다.
// - (용어) 예약/픽업: 물건을 등록만 하고(예약), 프레임 끝에 일괄 처리(픽업)하는 방식. 배칭의 구조적 근거.
void Scene2D::RenderScene()
{
	// 카메라가 아직 구성되지 않았으면 윈도우 크기로 직교 투영을 자동 설정한다.
	if (!GetCamera2D()->IsConfigured())
		GetCamera2D()->SetOrthographic2D((_f32)GetWindow()->Width(), (_f32)GetWindow()->Height());

	g_cRenderer2D.BeginScene(this);            // 씬 카메라로 2D 배치 시작
	if ((++sLogFrame % 60) == 1)
		_LogDebug_("[sgf] Scene2D::RenderScene Begin (window=%p)", (void*)GetWindow());

	// ── ① 예약 (GPU 무접촉, 트리 순회 순서 = 드로우 순서) ──
	RenderNode(root_);                         // 레이어 zOrder 오름차순 DFS, 부모 먼저

	// ── ② 수동 그리기 (최상위 — 트래버설 후 1회) ──
	OnRender();                            // pScene_ 경유 없이 씬 자신 — RenderDynamic(...) 직접

	// ── ③ 픽업 (프레임 끝 1회 — 드로우 스트림 순회) ──
	g_cRenderer2D.FlushStatic();
	g_cRenderer2D.FlushDynamic();

	g_cRenderer2D.EndScene();
	if ((sLogFrame % 60) == 1)
		_LogDebug_("[sgf] Scene2D::RenderScene End");
}

NS_SGF_END