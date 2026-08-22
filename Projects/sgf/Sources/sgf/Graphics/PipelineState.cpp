/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 3:30:00 PM
 * =====================
 * 독립 파이프라인 상태 객체 4종 구현 + 열거형 변환 헬퍼
 */

#include "Core.h"
#include "sgf/Graphics/PipelineState.h"
#include "sgf/Graphics/GraphicDevice.h"

NS_SGF_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// 상태 설명자 채우기 헬퍼
void FillRasterizerDesc(D3D11_RASTERIZER_DESC* _pOutDesc, CullMode _cull, FillMode _fill, FrontFace _frontFace)
{
	*_pOutDesc = {};
	_pOutDesc->FillMode = ToD3D11(_fill);
	_pOutDesc->CullMode = ToD3D11(_cull);
	_pOutDesc->FrontCounterClockwise = (_frontFace == FrontFace::ffCounterClockwise) ? TRUE : FALSE;
	_pOutDesc->DepthClipEnable = TRUE;	// 근/원 평면 밖 깊이 잘라내기 (기본 동작)
}

////////////////////////////////////////////////////////////////////////////////////////////
void FillBlendDesc(D3D11_BLEND_DESC* _pOutDesc, BlendMode _mode)
{
	*_pOutDesc = {};
	D3D11_RENDER_TARGET_BLEND_DESC& rt = _pOutDesc->RenderTarget[0];
	rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	switch (_mode)
	{
	case BlendMode::bmAlpha:
		// 최종색 = 새색*알파 + 기존색*(1-알파)
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rt.BlendOp = D3D11_BLEND_OP_ADD;
		rt.SrcBlendAlpha = D3D11_BLEND_ONE;
		rt.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		break;

	case BlendMode::bmAdd:
		// 최종색 = 새색*알파 + 기존색 (밝아짐)
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D11_BLEND_ONE;
		rt.BlendOp = D3D11_BLEND_OP_ADD;
		rt.SrcBlendAlpha = D3D11_BLEND_ONE;
		rt.DestBlendAlpha = D3D11_BLEND_ONE;
		rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		break;

	case BlendMode::bmMultiply:
		// 최종색 = 새색 * 기존색 (어두워짐)
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D11_BLEND_DEST_COLOR;
		rt.DestBlend = D3D11_BLEND_ZERO;
		rt.BlendOp = D3D11_BLEND_OP_ADD;
		rt.SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		rt.DestBlendAlpha = D3D11_BLEND_ZERO;
		rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		break;

	case BlendMode::bmNone:
	default:
		rt.BlendEnable = FALSE;
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
void FillDepthStencilDesc(D3D11_DEPTH_STENCIL_DESC* _pOutDesc, DepthMode _mode)
{
	*_pOutDesc = {};
	_pOutDesc->DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	switch (_mode)
	{
	case DepthMode::dmReadWrite:
		_pOutDesc->DepthEnable = TRUE;
		_pOutDesc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		break;

	case DepthMode::dmReadOnly:
		_pOutDesc->DepthEnable = TRUE;
		_pOutDesc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		break;

	case DepthMode::dmDisabled:
	default:
		_pOutDesc->DepthEnable = FALSE;
		_pOutDesc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
void FillSamplerDesc(D3D11_SAMPLER_DESC* _pOutDesc, FilterMode _filter, AddressMode _addressU, AddressMode _addressV)
{
	*_pOutDesc = {};
	_pOutDesc->Filter = ToD3D11(_filter);
	_pOutDesc->AddressU = ToD3D11(_addressU);
	_pOutDesc->AddressV = ToD3D11(_addressV);
	_pOutDesc->AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	_pOutDesc->MaxAnisotropy = (_filter == FilterMode::fmAnisotropic) ? D3D11_MAX_MAXANISOTROPY : 1;
	_pOutDesc->ComparisonFunc = D3D11_COMPARISON_NEVER;
	_pOutDesc->MinLOD = 0.0f;
	_pOutDesc->MaxLOD = D3D11_FLOAT32_MAX;
}

////////////////////////////////////////////////////////////////////////////////////////////
// RasterizerState
bool RasterizerState::Initialize(GraphicDevice* _pDevice, CullMode _cull, FillMode _fill, FrontFace _frontFace)
{
	jc_assert_msg(_pDevice != nullptr, "디바이스가 없습니다.");

	// 재생성 대비: 기존 상태를 먼저 정리한다.
	pState_.Reset();

	D3D11_RASTERIZER_DESC desc;
	FillRasterizerDesc(&desc, _cull, _fill, _frontFace);

	if (FAILED(_pDevice->Device()->CreateRasterizerState(&desc, pState_.GetAddressOf())))
	{
		return false;
	}

	cull_ = _cull;
	fill_ = _fill;
	frontFace_ = _frontFace;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
void RasterizerState::Finalize()
{
	pState_.Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////
// BlendState
bool BlendState::Initialize(GraphicDevice* _pDevice, BlendMode _mode)
{
	jc_assert_msg(_pDevice != nullptr, "디바이스가 없습니다.");

	pState_.Reset();

	D3D11_BLEND_DESC desc;
	FillBlendDesc(&desc, _mode);

	if (FAILED(_pDevice->Device()->CreateBlendState(&desc, pState_.GetAddressOf())))
	{
		return false;
	}

	mode_ = _mode;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
void BlendState::Finalize()
{
	pState_.Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////
// DepthStencilState
bool DepthStencilState::Initialize(GraphicDevice* _pDevice, DepthMode _mode)
{
	jc_assert_msg(_pDevice != nullptr, "디바이스가 없습니다.");

	pState_.Reset();

	D3D11_DEPTH_STENCIL_DESC desc;
	FillDepthStencilDesc(&desc, _mode);

	if (FAILED(_pDevice->Device()->CreateDepthStencilState(&desc, pState_.GetAddressOf())))
	{
		return false;
	}

	mode_ = _mode;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
void DepthStencilState::Finalize()
{
	pState_.Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////
// SamplerState
bool SamplerState::Initialize(GraphicDevice* _pDevice, FilterMode _filter, AddressMode _addressU, AddressMode _addressV)
{
	jc_assert_msg(_pDevice != nullptr, "디바이스가 없습니다.");

	pState_.Reset();

	D3D11_SAMPLER_DESC desc;
	FillSamplerDesc(&desc, _filter, _addressU, _addressV);

	if (FAILED(_pDevice->Device()->CreateSamplerState(&desc, pState_.GetAddressOf())))
	{
		return false;
	}

	filter_ = _filter;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
void SamplerState::Finalize()
{
	pState_.Reset();
}

NS_SGF_END
