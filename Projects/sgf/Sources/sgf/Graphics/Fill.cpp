/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (게임 오브젝트 시스템)
 * =====================
 * 2D 채움(Fill) 내장 콜백 구현
 *
 * [내장 채움]
 * - Solid: 단색 쿼드 1개 (영역 전체를 color1로)
 * - Texture: 텍스처 쿼드 1개 (UV 지정, 틴트 = color1)
 * - NinePatch: 텍스처 9분할 (모서리 고정 / 가장자리 스트레치 / 중앙 확장)
 * - Gauge: 배경 쿼드(color1) + 비율만큼 채움 쿼드(color2)
 * - Custom: 사용자 콜백 — 이 파일 외부에서 정의된다.
 *
 * 모든 콜백은 "영역 → 버텍스/인덱스" 변환만 수행한다. GPU 접촉은 없고,
 * 호출 주체(렌더러)가 FillResult를 버퍼에 배치한다.
 */

#include "sgf/Graphics/Fill.h"
#include "sgf/Graphics/Texture.h"
#include "sgf/Graphics/PrimitiveBuilder.h"
#include "jc/Math.h"

NS_SGF_BEGIN

using namespace jc;

namespace
{
	// 공통: 텍스처가 유효한지 (단색은 nullptr 허용)
	Texture* ResolveTexture(const Fill& _fill, const RenderParams& _params)
	{
		const Texture* pTex = _fill.GetTexture();
		if (pTex != nullptr)
		{
			return const_cast<Texture*>(pTex);
		}
		return nullptr;
	}
}

// 단색: 영역을 color1로 채운 쿼드 1개 (텍스처 없음)
static void FillSolid(const Fill& _fill, const RenderParams& _p, FillResult& _r)
{
	PrimitiveBuilder::PushQuad(_r, _p.region_.Left(), _p.region_.Bottom(), _p.region_.Right(), _p.region_.Top(),
		vec2(0.0f, 0.0f), vec2(1.0f, 1.0f), _p.color1_);
	_r.pTexture_ = ResolveTexture(_fill, _p);	// 단색은 보통 nullptr
}

// 텍스처 쿼드: 영역에 _fill의 텍스처/uv를 입힌 쿼드 1개 (틴트 = color1)
static void FillTexture(const Fill& _fill, const RenderParams& _p, FillResult& _r)
{
	const rect& uv = _fill.GetUV();
	PrimitiveBuilder::PushQuad(_r, _p.region_.Left(), _p.region_.Bottom(), _p.region_.Right(), _p.region_.Top(),
		vec2(uv.Left(), uv.Bottom()), vec2(uv.Right(), uv.Top()), _p.color1_);
	_r.pTexture_ = ResolveTexture(_fill, _p);
}

// 9-패치: 텍스처를 가로3×세로3 = 9분할, 모서리는 그대로·가장자리는 스트레치·중앙은 확장
// 슬라이스(l/t/r/b 픽셀)가 텍스처의 실제 픽셀 크기를 초과하지 않도록 클램프한다.
static void FillNinePatch(const Fill& _fill, const RenderParams& _p, FillResult& _r)
{
	const Texture* pTex = _fill.GetTexture();
	if (pTex == nullptr)
	{
		FillSolid(_fill, _p, _r);
		return;
	}

	const _f32 texW = (_f32)jc::Math::Max(1, pTex->Width());
	const _f32 texH = (_f32)jc::Math::Max(1, pTex->Height());

	const _f32 l = _fill.GetSliceLeft();
	const _f32 t = _fill.GetSliceTop();
	const _f32 r = _fill.GetSliceRight();
	const _f32 b = _fill.GetSliceBottom();

	// UV 좌표 (텍스처 기준)
	const _f32 uvL = l / texW;
	const _f32 uvR = 1.0f - r / texW;
	const _f32 uvB = b / texH;
	const _f32 uvT = 1.0f - t / texH;

	// 영역 좌표 (슬라이스 픽셀은 영역에 그대로 적용 — 모서리는 스트레치 없음)
	const _f32 x1 = _p.region_.Left(),    x4 = _p.region_.Right();
	const _f32 y1 = _p.region_.Bottom(),  y4 = _p.region_.Top();
	const _f32 x2 = x1 + l,               x3 = x4 - r;
	const _f32 y2 = y1 + b,               y3 = y4 - t;

	// 가로 4 x 세로 4 격자 (16정점)
	// uv 좌표: u = (0, uvL, uvR, 1), v = (0, uvB, uvT, 1)
	const _f32 xs[4] = { x1, x2, x3, x4 };
	const _f32 ys[4] = { y1, y2, y3, y4 };
	const _f32 us[4] = { 0.0f, uvL, uvR, 1.0f };
	const _f32 vs[4] = { 0.0f, uvB, uvT, 1.0f };

	const _s32 base = _r.vertices_.Size();
	for (int gy = 0; gy < 4; ++gy)
	{
		for (int gx = 0; gx < 4; ++gx)
		{
			_r.vertices_.PushBack(VertexPTC{
				vec3(xs[gx], ys[gy], 0.0f),
				vec2(us[gx], vs[gy]),
				_p.color1_
			});
		}
	}

	// 9셀 × 2삼각형 × 3인덱스 = 54 (격자 순서: 아래줄 → 위줄)
	for (int gy = 0; gy < 3; ++gy)
	{
		for (int gx = 0; gx < 3; ++gx)
		{
			const _u16 a = (_u16)(base + gy * 4 + gx);
			const _u16 b0 = (_u16)(base + gy * 4 + gx + 1);
			const _u16 c = (_u16)(base + (gy + 1) * 4 + gx + 1);
			const _u16 d = (_u16)(base + (gy + 1) * 4 + gx);

			_r.indices_.PushBack(a); _r.indices_.PushBack(b0); _r.indices_.PushBack(c);
			_r.indices_.PushBack(a); _r.indices_.PushBack(c);  _r.indices_.PushBack(d);
		}
	}

	_r.pTexture_ = const_cast<Texture*>(pTex);
}

// 수평 게이지: ratio만큼 color2로 채운 막대 + 나머지 color1 배경
static void FillGauge(const Fill& _fill, const RenderParams& _p, FillResult& _r)
{
	const _f32 ratio = jc::Clamp(_fill.GetRatio(), 0.0f, 1.0f);
	const _f32 x1 = _p.region_.Left(), y1 = _p.region_.Bottom();
	const _f32 x2 = _p.region_.Right(), y2 = _p.region_.Top();
	const _f32 fillX = x1 + (x2 - x1) * ratio;

	// 배경 쿼드 (color1)
	PrimitiveBuilder::PushQuad(_r, x1, y1, x2, y2, vec2(0.0f, 0.0f), vec2(1.0f, 1.0f), _p.color1_);
	// 채움 쿼드 (color2) — 폭 = 영역 폭 × ratio
	PrimitiveBuilder::PushQuad(_r, x1, y1, fillX, y2, vec2(0.0f, 0.0f), vec2(1.0f, 1.0f), _p.color2_);

	_r.pTexture_ = nullptr;
}

// Custom은 이 파일에 구현이 없다 — 사용자가 콜백을 정의해 Fill::Custom(fn)으로 전달한다.

Fill Fill::Solid()
{
	const _f32 slice[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	return Fill(FillSolid, nullptr, rect(0.0f, 0.0f, 1.0f, 1.0f), slice, 1.0f, 0);
}

Fill Fill::Texture(sgf::Texture* _pTex, const rect& _uv)
{
	const _f32 slice[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	return Fill(FillTexture, _pTex, _uv, slice, 1.0f, 0);
}

Fill Fill::NinePatch(sgf::Texture* _pTex, _f32 _l, _f32 _t, _f32 _r, _f32 _b, const rect& _uv)
{
	const _f32 slice[4] = { _l, _t, _r, _b };
	return Fill(FillNinePatch, _pTex, _uv, slice, 1.0f, 0);
}

Fill Fill::Gauge(_f32 _ratio)
{
	const _f32 slice[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	return Fill(FillGauge, nullptr, rect(0.0f, 0.0f, 1.0f, 1.0f), slice, _ratio, 0);
}

Fill Fill::Custom(FillCallback _pFn, _u32 _userData)
{
	const _f32 slice[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	return Fill(_pFn, nullptr, rect(0.0f, 0.0f, 1.0f, 1.0f), slice, 1.0f, _userData);
}

Fill Fill::CustomData(FillCallback _pFn, const void* _pData)
{
	const _f32 slice[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	return Fill(_pFn, nullptr, rect(0.0f, 0.0f, 1.0f, 1.0f), slice, 1.0f, 0, _pData);
}

NS_SGF_END