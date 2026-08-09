/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 1:00:00 AM
 * =====================
 * 렌더 상태 저장소 구현부
 */

#include "Core.h"
#include "sgf/Graphics/RenderStates.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
RenderStates::RenderStates()
	: pDevice_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 자주 쓰는 상태를 미리 만든다. 실패 시 false.
bool RenderStates::Initialize(ID3D11Device* _pDevice)
{
	pDevice_ = _pDevice;

	// 지연 생성 경로를 그대로 타서 미리 만들어둔다. (첫 프레임 끈상 방지)
	if (GetBlendState(BlendMode::Opaque) == nullptr) { return false; }
	if (GetBlendState(BlendMode::Alpha) == nullptr) { return false; }
	if (GetDepthState(true) == nullptr) { return false; }
	if (GetDepthState(false) == nullptr) { return false; }
	if (GetRasterizerState(false, CullMode::Back) == nullptr) { return false; }
	if (GetSamplerState(SamplerFilter::Linear, SamplerAddress::Clamp) == nullptr) { return false; }
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void RenderStates::Finalize()
{
	for (int i = 0; i < static_cast<int>(BlendMode::Max); ++i)
	{
		pBlendStates_[i].Reset();
	}
	pDepthStates_[0].Reset();
	pDepthStates_[1].Reset();
	for (int fill = 0; fill < 2; ++fill)
	{
		for (int cull = 0; cull < static_cast<int>(CullMode::Max); ++cull)
		{
			pRasterizerStates_[fill][cull].Reset();
		}
	}
	for (int filter = 0; filter < static_cast<int>(SamplerFilter::Max); ++filter)
	{
		for (int address = 0; address < static_cast<int>(SamplerAddress::Max); ++address)
		{
			pSamplerStates_[filter][address].Reset();
		}
	}
	pDevice_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 블렌드 상태: "새로 그리는 색과 이미 그려진 색을 어떻게 섞을지"의 공식 설정
ID3D11BlendState* RenderStates::GetBlendState(BlendMode _mode)
{
	const int index = static_cast<int>(_mode);
	if (index < 0 || index >= static_cast<int>(BlendMode::Max))
	{
		return nullptr;
	}
	if (pBlendStates_[index])
	{
		return pBlendStates_[index].Get();
	}

	D3D11_BLEND_DESC desc;
	memset(&desc, 0, sizeof(desc));
	D3D11_RENDER_TARGET_BLEND_DESC& rt = desc.RenderTarget[0];
	rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	rt.BlendOp = D3D11_BLEND_OP_ADD;
	rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rt.SrcBlendAlpha = D3D11_BLEND_ONE;
	rt.DestBlendAlpha = D3D11_BLEND_ZERO;

	switch (_mode)
	{
	case BlendMode::Opaque:
		// 섞지 않고 덮어쓴다.
		rt.BlendEnable = FALSE;
		rt.SrcBlend = D3D11_BLEND_ONE;
		rt.DestBlend = D3D11_BLEND_ZERO;
		break;
	case BlendMode::Alpha:
		// 최종 = 새색*알파 + 기존색*(1-알파)
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		break;
	case BlendMode::Additive:
		// 최종 = 새색*알파 + 기존색  (쌓일수록 밝아진다 - 빛 효과)
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D11_BLEND_ONE;
		break;
	case BlendMode::Multiply:
		// 최종 = 새색 * 기존색  (쌓일수록 어두워진다 - 그림자/어둠)
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D11_BLEND_ZERO;
		rt.DestBlend = D3D11_BLEND_SRC_COLOR;
		break;
	default:
		return nullptr;
	}

	if (FAILED(pDevice_->CreateBlendState(&desc, pBlendStates_[index].GetAddressOf())))
	{
		return nullptr;
	}
	return pBlendStates_[index].Get();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 깊이 상태: 깊이 테스트(가림 처리) ON/OFF
ID3D11DepthStencilState* RenderStates::GetDepthState(bool _bEnable)
{
	const int index = _bEnable ? 1 : 0;
	if (pDepthStates_[index])
	{
		return pDepthStates_[index].Get();
	}

	D3D11_DEPTH_STENCIL_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.DepthEnable = _bEnable ? TRUE : FALSE;
	desc.DepthWriteMask = _bEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_LESS;		// 더 가까우면(값이 작으면) 통과

	if (FAILED(pDevice_->CreateDepthStencilState(&desc, pDepthStates_[index].GetAddressOf())))
	{
		return nullptr;
	}
	return pDepthStates_[index].Get();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 래스터라이저 상태: 면 채우기 방식(솔리드/와이어) + 컸링 방향
ID3D11RasterizerState* RenderStates::GetRasterizerState(bool _bWireframe, CullMode _cull)
{
	const int fillIndex = _bWireframe ? 1 : 0;
	const int cullIndex = static_cast<int>(_cull);
	if (cullIndex < 0 || cullIndex >= static_cast<int>(CullMode::Max))
	{
		return nullptr;
	}
	if (pRasterizerStates_[fillIndex][cullIndex])
	{
		return pRasterizerStates_[fillIndex][cullIndex].Get();
	}

	static const D3D11_CULL_MODE s_CullModes[] = { D3D11_CULL_NONE, D3D11_CULL_BACK, D3D11_CULL_FRONT };

	D3D11_RASTERIZER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.FillMode = _bWireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	desc.CullMode = s_CullModes[cullIndex];
	desc.FrontCounterClockwise = FALSE;		// 시계방향이 앞면 (DX 기본 규약)
	desc.DepthClipEnable = TRUE;

	if (FAILED(pDevice_->CreateRasterizerState(&desc, pRasterizerStates_[fillIndex][cullIndex].GetAddressOf())))
	{
		return nullptr;
	}
	return pRasterizerStates_[fillIndex][cullIndex].Get();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 샘플러 상태: 텍스처를 읽는 방법 (필터 + UV 범위 밖 처리)
ID3D11SamplerState* RenderStates::GetSamplerState(SamplerFilter _filter, SamplerAddress _address)
{
	const int filterIndex = static_cast<int>(_filter);
	const int addressIndex = static_cast<int>(_address);
	if (filterIndex < 0 || filterIndex >= static_cast<int>(SamplerFilter::Max)
		|| addressIndex < 0 || addressIndex >= static_cast<int>(SamplerAddress::Max))
	{
		return nullptr;
	}
	if (pSamplerStates_[filterIndex][addressIndex])
	{
		return pSamplerStates_[filterIndex][addressIndex].Get();
	}

	static const D3D11_TEXTURE_ADDRESS_MODE s_AddressModes[] = {
		D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_MIRROR
	};

	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Filter = (_filter == SamplerFilter::Point)
		? D3D11_FILTER_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = s_AddressModes[addressIndex];
	desc.AddressV = s_AddressModes[addressIndex];
	desc.AddressW = s_AddressModes[addressIndex];
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MinLOD = 0.0f;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(pDevice_->CreateSamplerState(&desc, pSamplerStates_[filterIndex][addressIndex].GetAddressOf())))
	{
		return nullptr;
	}
	return pSamplerStates_[filterIndex][addressIndex].Get();
}

NS_SGF_END
