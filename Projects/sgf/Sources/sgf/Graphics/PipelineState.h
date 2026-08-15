/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 3:30:00 PM
 * =====================
 * 독립 파이프라인 상태 객체 4종 (v3)
 *
 * [상태 객체(State Object)란?]
 *  DX11은 "반투명 설정", "깊이 테스트 설정" 같은 설정값 묶음을 미리 만들어두고
 *  그리기 전에 꺼내 끼우는 방식이다. 이 파일의 클래스들은 상태 객체 하나를
 *  "직접 소유"하는 래퍼다. (09번 튜토리얼: 상태 객체를 직접 만들어 보기)
 *
 * [RenderStates(플라이웨이트)와의 관계]
 *  같은 설정 조합은 앱 전체에서 1개만 있으면 충분하다. 그래서 엔진 내부
 *  (Material 등)는 RenderStates 캐시에서 "빌려" 쓰고(비소유 핸들),
 *  이 래퍼들은 학습/실험용으로 상태를 직접 만들어 볼 때 사용한다.
 */

#pragma once

#include "sgf/Graphics/GraphicsEnums.h"

NS_SGF_BEGIN

class GraphicDevice;

// 래스터라이저 상태: 컴링/채우기/앞면 판정 기준
class RasterizerState
{
public:
	// 상태 객체를 생성한다.
	// @param _pDevice   : 그래픽 디바이스
	// @param _cull      : 컴링 모드 (기본 cmBack)
	// @param _fill      : 채우기 모드 (기본 fmSolid)
	// @param _frontFace : 앞면 판정 기준 (기본 ffClockwise)
	bool Initialize(
		GraphicDevice* _pDevice,
		CullMode _cull = CullMode::cmBack,
		FillMode _fill = FillMode::fmSolid,
		FrontFace _frontFace = FrontFace::ffClockwise);

	void Finalize();

	bool IsValid() const { return pState_ != nullptr; }
	ID3D11RasterizerState* Raw() const { return pState_.Get(); }

	CullMode Cull() const { return cull_; }
	FillMode Fill() const { return fill_; }
	FrontFace Front() const { return frontFace_; }

private:
	SgfComPtr<ID3D11RasterizerState> pState_;
	CullMode cull_ = CullMode::cmBack;
	FillMode fill_ = FillMode::fmSolid;
	FrontFace frontFace_ = FrontFace::ffClockwise;
};

// 블렌드 상태: 색 섞기 공식
class BlendState
{
public:
	bool Initialize(GraphicDevice* _pDevice, BlendMode _mode = BlendMode::bmNone);
	void Finalize();

	bool IsValid() const { return pState_ != nullptr; }
	ID3D11BlendState* Raw() const { return pState_.Get(); }

	BlendMode Mode() const { return mode_; }

private:
	SgfComPtr<ID3D11BlendState> pState_;
	BlendMode mode_ = BlendMode::bmNone;
};

// 깊이/스텐실 상태: 깊이 테스트/기록 설정
class DepthStencilState
{
public:
	bool Initialize(GraphicDevice* _pDevice, DepthMode _mode = DepthMode::dmReadWrite);
	void Finalize();

	bool IsValid() const { return pState_ != nullptr; }
	ID3D11DepthStencilState* Raw() const { return pState_.Get(); }

	DepthMode Mode() const { return mode_; }

private:
	SgfComPtr<ID3D11DepthStencilState> pState_;
	DepthMode mode_ = DepthMode::dmReadWrite;
};

// 샘플러 상태: 텍스처 읽는 방법 (필터 + 주소 모드)
class SamplerState
{
public:
	bool Initialize(
		GraphicDevice* _pDevice,
		FilterMode _filter = FilterMode::fmLinear,
		AddressMode _addressU = AddressMode::amClamp,
		AddressMode _addressV = AddressMode::amClamp);

	void Finalize();

	bool IsValid() const { return pState_ != nullptr; }
	ID3D11SamplerState* Raw() const { return pState_.Get(); }

	FilterMode Filter() const { return filter_; }

private:
	SgfComPtr<ID3D11SamplerState> pState_;
	FilterMode filter_ = FilterMode::fmLinear;
};

// 상태 설명자 채우기 헬퍼 (생성은 호출자가)
void FillRasterizerDesc(D3D11_RASTERIZER_DESC* _pOutDesc, CullMode _cull, FillMode _fill, FrontFace _frontFace);
void FillBlendDesc(D3D11_BLEND_DESC* _pOutDesc, BlendMode _mode);
void FillDepthStencilDesc(D3D11_DEPTH_STENCIL_DESC* _pOutDesc, DepthMode _mode);
void FillSamplerDesc(D3D11_SAMPLER_DESC* _pOutDesc, FilterMode _filter, AddressMode _addressU, AddressMode _addressV);

NS_SGF_END
