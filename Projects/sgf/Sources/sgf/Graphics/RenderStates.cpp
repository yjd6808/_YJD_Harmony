/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 1:00:00 AM
 * 수정일: 8/9/2026 5:20:00 PM (v3: GraphicsEnums 통합, U/V 분리 샘플러, FrontFace/ReadOnly 지원)
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

	// 지연 생성 경로를 그대로 타서 미리 만들어둔다. (첫 프레임 끊김 방지)
	if (GetBlendState(BlendMode::bmNone) == nullptr) { return false; }
	if (GetBlendState(BlendMode::bmAlpha) == nullptr) { return false; }
	if (GetDepthState(DepthMode::dmReadWrite) == nullptr) { return false; }
	if (GetDepthState(DepthMode::dmDisabled) == nullptr) { return false; }
	if (GetRasterizerState(CullMode::cmBack) == nullptr) { return false; }
	if (GetSamplerState(FilterMode::fmLinear, AddressMode::amClamp) == nullptr) { return false; }
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void RenderStates::Finalize()
{
	for (int i = 0; i < static_cast<int>(BlendMode::Max); ++i)
	{
		pBlendStates_[i].Reset();
	}
	for (int i = 0; i < static_cast<int>(DepthMode::Max); ++i)
	{
		pDepthStates_[i].Reset();
	}
	for (int fill = 0; fill < static_cast<int>(FillMode::Max); ++fill)
	{
		for (int cull = 0; cull < static_cast<int>(CullMode::Max); ++cull)
		{
			for (int face = 0; face < static_cast<int>(FrontFace::Max); ++face)
			{
				pRasterizerStates_[fill][cull][face].Reset();
			}
		}
	}
	for (int filter = 0; filter < static_cast<int>(FilterMode::Max); ++filter)
	{
		for (int u = 0; u < static_cast<int>(AddressMode::Max); ++u)
		{
			for (int v = 0; v < static_cast<int>(AddressMode::Max); ++v)
			{
				pSamplerStates_[filter][u][v].Reset();
			}
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
	case BlendMode::bmNone:
		// 섞지 않고 덮어쓴다.
		rt.BlendEnable = FALSE;
		rt.SrcBlend = D3D11_BLEND_ONE;
		rt.DestBlend = D3D11_BLEND_ZERO;
		break;
	case BlendMode::bmAlpha:
		// 최종 = 새색*알파 + 기존색*(1-알파)
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		break;
	case BlendMode::bmAdd:
		// 최종 = 새색*알파 + 기존색  (쌓일수록 밝아진다 - 빛 효과)
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D11_BLEND_ONE;
		break;
	case BlendMode::bmMultiply:
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
// 깊이 상태: 깊이 테스트(가림 처리) 모드
ID3D11DepthStencilState* RenderStates::GetDepthState(DepthMode _mode)
{
	const int index = static_cast<int>(_mode);
	if (index < 0 || index >= static_cast<int>(DepthMode::Max))
	{
		return nullptr;
	}
	if (pDepthStates_[index])
	{
		return pDepthStates_[index].Get();
	}

	D3D11_DEPTH_STENCIL_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.DepthFunc = D3D11_COMPARISON_LESS;		// 더 가까우면(값이 작으면) 통과

	switch (_mode)
	{
	case DepthMode::dmDisabled:
		desc.DepthEnable = FALSE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		break;
	case DepthMode::dmReadWrite:
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		break;
	case DepthMode::dmReadOnly:
		// 테스트는 하되 기록은 안 한다. (반투명 3D 물체가 서로를 가리지 않게)
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		break;
	default:
		return nullptr;
	}

	if (FAILED(pDevice_->CreateDepthStencilState(&desc, pDepthStates_[index].GetAddressOf())))
	{
		return nullptr;
	}
	return pDepthStates_[index].Get();
}

//////////////////////////////////////////////////////////////////////////////////////////
// [하위 호환] true=일반 3D, false=2D 적층
ID3D11DepthStencilState* RenderStates::GetDepthState(bool _bEnable)
{
	return GetDepthState(_bEnable ? DepthMode::dmReadWrite : DepthMode::dmDisabled);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 래스터라이저 상태: 채우기 + 컬링 + 앞면 판정 조합
ID3D11RasterizerState* RenderStates::GetRasterizerState(CullMode _cull, FillMode _fill, FrontFace _frontFace)
{
	const int fillIndex = static_cast<int>(_fill);
	const int cullIndex = static_cast<int>(_cull);
	const int faceIndex = static_cast<int>(_frontFace);
	if (fillIndex < 0 || fillIndex >= static_cast<int>(FillMode::Max)
		|| cullIndex < 0 || cullIndex >= static_cast<int>(CullMode::Max)
		|| faceIndex < 0 || faceIndex >= static_cast<int>(FrontFace::Max))
	{
		return nullptr;
	}
	if (pRasterizerStates_[fillIndex][cullIndex][faceIndex])
	{
		return pRasterizerStates_[fillIndex][cullIndex][faceIndex].Get();
	}

	D3D11_RASTERIZER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.FillMode = ToD3D11(_fill);
	desc.CullMode = ToD3D11(_cull);
	desc.FrontCounterClockwise = (_frontFace == FrontFace::ffCounterClockwise) ? TRUE : FALSE;
	desc.DepthClipEnable = TRUE;

	if (FAILED(pDevice_->CreateRasterizerState(&desc, pRasterizerStates_[fillIndex][cullIndex][faceIndex].GetAddressOf())))
	{
		return nullptr;
	}
	return pRasterizerStates_[fillIndex][cullIndex][faceIndex].Get();
}

//////////////////////////////////////////////////////////////////////////////////////////
// [하위 호환] 와이어프레임 여부 + 컬링
ID3D11RasterizerState* RenderStates::GetRasterizerState(bool _bWireframe, CullMode _cull)
{
	return GetRasterizerState(_cull, _bWireframe ? FillMode::fmWireframe : FillMode::fmSolid);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 샘플러 상태: 텍스처를 읽는 방법 (필터 + UV 범위 밖 처리. U/V 분리 지원)
ID3D11SamplerState* RenderStates::GetSamplerState(FilterMode _filter, AddressMode _addressU, AddressMode _addressV)
{
	const int filterIndex = static_cast<int>(_filter);
	const int uIndex = static_cast<int>(_addressU);
	const int vIndex = static_cast<int>(_addressV);
	if (filterIndex < 0 || filterIndex >= static_cast<int>(FilterMode::Max)
		|| uIndex < 0 || uIndex >= static_cast<int>(AddressMode::Max)
		|| vIndex < 0 || vIndex >= static_cast<int>(AddressMode::Max))
	{
		return nullptr;
	}
	if (pSamplerStates_[filterIndex][uIndex][vIndex])
	{
		return pSamplerStates_[filterIndex][uIndex][vIndex].Get();
	}

	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Filter = ToD3D11(_filter);
	desc.AddressU = ToD3D11(_addressU);
	desc.AddressV = ToD3D11(_addressV);
	desc.AddressW = ToD3D11(_addressV);
	desc.MaxAnisotropy = (_filter == FilterMode::fmAnisotropic) ? 16 : 1;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MinLOD = 0.0f;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	// amBorder용 테두리 색: 불투명 검정 (그림자 맵 범위 밖 = 그림자 없음 처리 등)
	desc.BorderColor[0] = 0.0f;
	desc.BorderColor[1] = 0.0f;
	desc.BorderColor[2] = 0.0f;
	desc.BorderColor[3] = 1.0f;

	if (FAILED(pDevice_->CreateSamplerState(&desc, pSamplerStates_[filterIndex][uIndex][vIndex].GetAddressOf())))
	{
		return nullptr;
	}
	return pSamplerStates_[filterIndex][uIndex][vIndex].Get();
}

//////////////////////////////////////////////////////////////////////////////////////////
// [하위 호환] U/V 동일 주소 모드
ID3D11SamplerState* RenderStates::GetSamplerState(FilterMode _filter, AddressMode _address)
{
	return GetSamplerState(_filter, _address, _address);
}

NS_SGF_END
