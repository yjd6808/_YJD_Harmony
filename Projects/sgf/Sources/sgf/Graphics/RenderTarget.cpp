/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 1:32:00 PM
 * =====================
 * 렌더 타깃 구현부
 *
 * [만드는 순서 요약]
 *  1. 텍스처 생성 (BindFlags에 "그리기 대상 + 셰이더 입력" 두 용도를 함께 지정)
 *  2. 그 텍스처를 보는 RTV 생성 (그리기용 해석)
 *  3. 그 텍스처를 보는 SRV 생성 (읽기용 해석)
 *  4. 깊이 버퍼도 같은 크기로 별도 생성
 */

#include "Core.h"
#include "sgf/Graphics/RenderTarget.h"
#include "sgf/Graphics/GraphicDevice.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
// 생성자: 멤버 초기화
RenderTarget::RenderTarget()
	: depthOnly_(false)
	, width_(0)
	, height_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 소멸자: 리소스 자동 해제
RenderTarget::~RenderTarget()
{
	Destroy();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 색 + 깊이를 모두 가진 일반 렌더 타깃 생성 (미니맵/후처리용)
bool RenderTarget::Create(GraphicDevice* _pDevice, _s32 _width, _s32 _height)
{
	jc_assert(_pDevice != nullptr && _width > 0 && _height > 0);

	Destroy();
	width_ = _width;
	height_ = _height;
	depthOnly_ = false;

	ID3D11Device* pDevice = _pDevice->Device();

	// 1. 색 텍스처: 백버퍼와 같은 RGBA8 포맷.
	//    BIND_RENDER_TARGET(그리기 대상) + BIND_SHADER_RESOURCE(셰이더 입력)
	//    두 용도를 함께 지정하는 것이 핵심!
	D3D11_TEXTURE2D_DESC colorDesc = {};
	colorDesc.Width = _width;
	colorDesc.Height = _height;
	colorDesc.MipLevels = 1;
	colorDesc.ArraySize = 1;
	colorDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	colorDesc.SampleDesc.Count = 1;
	colorDesc.Usage = D3D11_USAGE_DEFAULT;
	colorDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = pDevice->CreateTexture2D(&colorDesc, nullptr, pColorTexture_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 2. RTV: 이 텍스처에 "그릴 수 있게" 하는 뷰
	hr = pDevice->CreateRenderTargetView(pColorTexture_.Get(), nullptr, pRTV_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 3. SRV: 이 텍스처를 "셰이더에서 읽을 수 있게" 하는 뷰
	hr = pDevice->CreateShaderResourceView(pColorTexture_.Get(), nullptr, pColorSRV_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 4. 깊이 버퍼: 이 타깃 안에서도 가림 처리가 되도록 같은 크기로 생성
	D3D11_TEXTURE2D_DESC depthDesc = colorDesc;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = pDevice->CreateTexture2D(&depthDesc, nullptr, pDepthTexture_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = pDevice->CreateDepthStencilView(pDepthTexture_.Get(), nullptr, pDSV_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 깊이 전용 렌더 타깃 생성 (그림자 맵용)
bool RenderTarget::CreateDepthOnly(GraphicDevice* _pDevice, _s32 _width, _s32 _height)
{
	jc_assert(_pDevice != nullptr && _width > 0 && _height > 0);

	Destroy();
	width_ = _width;
	height_ = _height;
	depthOnly_ = true;

	ID3D11Device* pDevice = _pDevice->Device();

	// 1. 깊이 텍스처를 TYPELESS(해석 미정) 포맷으로 만든다.
	//    같은 32비트 데이터를 DSV는 "깊이"로, SRV는 "실수"로
	//    서로 다르게 해석하기 위해서다.
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = _width;
	depthDesc.Height = _height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = pDevice->CreateTexture2D(&depthDesc, nullptr, pDepthTexture_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 2. DSV: 32비트를 "깊이값"으로 해석 (여기에 깊이가 기록된다)
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = pDevice->CreateDepthStencilView(pDepthTexture_.Get(), &dsvDesc, pDSV_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 3. SRV: 같은 32비트를 "실수 하나(R32_FLOAT)"로 해석 (셰이더에서 깊이를 읽는다)
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(pDepthTexture_.Get(), &srvDesc, pDepthSRV_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 모든 리소스 해제
void RenderTarget::Destroy()
{
	pColorSRV_.Reset();
	pRTV_.Reset();
	pColorTexture_.Reset();
	pDepthSRV_.Reset();
	pDSV_.Reset();
	pDepthTexture_.Reset();
	width_ = 0;
	height_ = 0;
	depthOnly_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 타깃 지우기
void RenderTarget::Clear(GraphicDevice* _pDevice, const color& _clearColor)
{
	ID3D11DeviceContext* pContext = _pDevice->Context();

	// 색 타깃이 있으면 배경색으로 지운다.
	if (pRTV_ != nullptr)
	{
		const _f32 clearColor[4] = { _clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a };
		pContext->ClearRenderTargetView(pRTV_.Get(), clearColor);
	}

	// 깊이는 1.0(가장 멀리)으로 지운다. 새로 그리는 픽셀이 항상 이길 수 있게.
	// (깊이 전용 D32_FLOAT 포맷에는 스텐실이 없으므로 깊이만 지운다)
	if (pDSV_ != nullptr)
	{
		const UINT clearFlags = depthOnly_
			? D3D11_CLEAR_DEPTH
			: (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
		pContext->ClearDepthStencilView(pDSV_.Get(), clearFlags, 1.0f, 0);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 다 그린 결과를 일반 텍스처처럼 셰이더 입력으로 바인딩
void RenderTarget::BindAsTexture(GraphicDevice* _pDevice, UINT _slot)
{
	ID3D11ShaderResourceView* pSrvs[] = { SRV() };
	_pDevice->Context()->PSSetShaderResources(_slot, 1, pSrvs);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 모드에 맞는 SRV 반환 (깊이 전용이면 깊이 SRV, 아니면 색 SRV)
ID3D11ShaderResourceView* RenderTarget::SRV() const
{
	return depthOnly_ ? pDepthSRV_.Get() : pColorSRV_.Get();
}

NS_SGF_END
