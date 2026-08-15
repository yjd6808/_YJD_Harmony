/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 1:00:00 AM
 * =====================
 * 깊이/스텐실 버퍼 구현부
 */

#include "Core.h"
#include "sgf/Graphics/DepthStencilSurface.h"

NS_SGF_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
// 깊이 텍스처 + DSV 생성
bool DepthStencilSurface::Initialize(ID3D11Device* _pDevice, _s32 _width, _s32 _height)
{
	// 재초기화 대비: 기존 DSV를 먼저 정리한다. (GetAddressOf 덮어쓰기 누수 방지)
	pDepthStencilView_.Reset();

	// 1. 깊이 값을 담을 텍스처를 만든다. (백버퍼와 같은 크기여야 함)
	D3D11_TEXTURE2D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof(textureDesc));
	textureDesc.Width = static_cast<UINT>(_width);
	textureDesc.Height = static_cast<UINT>(_height);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 깊이 24비트 + 스텐실 8비트
	textureDesc.SampleDesc.Count = 1;						// 멀티샘플링 사용 안 함
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	SgfComPtr<ID3D11Texture2D> pDepthTexture;
	if (FAILED(_pDevice->CreateTexture2D(&textureDesc, nullptr, pDepthTexture.GetAddressOf())))
	{
		return false;
	}

	// 2. 그 텍스처를 "깊이 기록 대상"으로 보는 뷰를 만든다.
	if (FAILED(_pDevice->CreateDepthStencilView(pDepthTexture.Get(), nullptr,
		pDepthStencilView_.GetAddressOf())))
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void DepthStencilSurface::Finalize()
{
	pDepthStencilView_.Reset();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 깊이 1.0(가장 멀리) / 스텐실 0으로 초기화
// 초기화하지 않으면 지난 프레임의 깊이 값이 남아 새 프레임 픽셀이 전부 탈락한다.
void DepthStencilSurface::Clear(ID3D11DeviceContext* _pContext)
{
	if (pDepthStencilView_)
	{
		_pContext->ClearDepthStencilView(pDepthStencilView_.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

NS_SGF_END
