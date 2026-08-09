/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:40:00 PM
 * 수정일: 8/9/2026 1:00:00 AM (v2: 파사드로 재구성)
 * =====================
 * D3D11 그래픽 디바이스 (파사드) 구현부
 */

#include "Core.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/RenderTarget.h"
#include "sgf/Core/Window.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
GraphicDevice::GraphicDevice()
	: pBoundWindow_(nullptr)
	, bWireframe_(false)
	, cullMode_(CullMode::Back)
	, width_(0)
	, height_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
GraphicDevice::~GraphicDevice()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
// D3D11 초기화 전체 진행
// [순서가 중요하다]
//  1. 디바이스+스왑체인 -> 2. 깊이버퍼 -> 3. 상태 캐시 -> 4. 뷰포트/기본상태.
//  앞 단계 산출물이 뒤 단계 입력이 된다.
// [v2.1] 디바이스/컨텍스트와 상태 캐시만 초기화한다. (표면 없음)
// 멀티 윈도우 경로: 이후 각 Window::CreateSurface가 자기 표면을 만든다.
bool GraphicDevice::Initialize()
{
	// 1. 디바이스/컨텍스트 생성
	if (!CreateDeviceOnly()) { return false; }

	// 2. 상태 객체 캐시 준비
	if (!states_.Initialize(pDevice_.Get())) { return false; }

	// 3. 기본 상태 적용 (샘플러 슬롯0, 솔리드+백컸링)
	ID3D11SamplerState* pSamplers[] = {
		states_.GetSamplerState(SamplerFilter::Linear, SamplerAddress::Clamp)
	};
	pContext_->PSSetSamplers(0, 1, pSamplers);
	ApplyRasterizerState();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GraphicDevice::Initialize(HWND _hWnd, _s32 _width, _s32 _height)
{
	width_ = _width;
	height_ = _height;

	// 1. 디바이스/컨텍스트/스왑체인 생성 (스왑체인은 SwapChain 부품이 소유)
	if (!CreateDeviceAndSwapChain(_hWnd)) { return false; }

	// 2. 깊이/스텐실 버퍼 생성
	if (!depthSurface_.Initialize(pDevice_.Get(), _width, _height)) { return false; }

	// 3. 상태 객체 캐시 준비 (자주 쓰는 상태는 미리 생성)
	if (!states_.Initialize(pDevice_.Get())) { return false; }

	// 4. 뷰포트 + 기본 상태 적용 (샘플러 슬롯0, 솔리드+백컬링)
	swapChain_.ApplyFullViewport(pContext_.Get());

	ID3D11SamplerState* pSamplers[] = {
		states_.GetSamplerState(SamplerFilter::Linear, SamplerAddress::Clamp)
	};
	pContext_->PSSetSamplers(0, 1, pSamplers);
	ApplyRasterizerState();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 모든 D3D 객체 해제. 부품별 Finalize를 순서대로 부른다.
void GraphicDevice::Finalize()
{
	// 파이프라인에 묶인 객체들을 먼저 해제하려면 상태를 비운다.
	if (pContext_ != nullptr)
	{
		pContext_->ClearState();
	}

	pBoundWindow_ = nullptr;
	states_.Finalize();
	depthSurface_.Finalize();
	swapChain_.Finalize();
	pContext_.Reset();
	pDevice_.Reset();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 디바이스와 스왑체인 생성
// D3D11CreateDeviceAndSwapChain 함수 하나로 둘 다 만든 뒤,
// 스왑체인 소유권은 SwapChain 부품에 넘긴다.
bool GraphicDevice::CreateDeviceAndSwapChain(HWND _hWnd)
{
	// 스왑체인 설정: 어떤 버퍼를 어떻게 교체할지 기술한다.
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = 1;									// 백버퍼 1개 (더블버퍼링)
	scd.BufferDesc.Width = width_;
	scd.BufferDesc.Height = height_;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// 픽셀당 RGBA 각 8비트
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// 그리기 대상으로 사용
	scd.OutputWindow = _hWnd;
	scd.SampleDesc.Count = 1;								// 멀티샘플링(MSAA) 사용 안 함
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;									// 창모드
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;				// 교체 후 이전 내용 버림

	// 디버그 빌드에서는 D3D11 디버그 레이어를 켠다.
	UINT createFlags = 0;
#if defined(_DEBUG)
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL selectedLevel = D3D_FEATURE_LEVEL_11_0;

	IDXGISwapChain* pRawSwapChain = nullptr;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlags,
		featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
		&scd,
		&pRawSwapChain,
		pDevice_.GetAddressOf(),
		&selectedLevel,
		pContext_.GetAddressOf());

	if (FAILED(hr))
	{
		// 디버그 레이어가 설치되지 않은 PC라면 플래그 없이 재시도한다.
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
			featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
			&scd,
			&pRawSwapChain,
			pDevice_.GetAddressOf(),
			&selectedLevel,
			pContext_.GetAddressOf());
	}

	if (FAILED(hr)) { return false; }

	// 스왑체인 소유권 이관. (SwapChain::Initialize가 Attach 방식이므로
	//  여기서 따로 Release하지 않는다. 이제 해제 책임은 SwapChain에 있다)
	return swapChain_.Initialize(pRawSwapChain, pDevice_.Get(), width_, height_);
}

//////////////////////////////////////////////////////////////////////////////////////////
// [v2.1] 디바이스만 생성한다. (스왕체인 없음 - 표면은 각 Window가 만든다)
bool GraphicDevice::CreateDeviceOnly()
{
	UINT createFlags = 0;
#if defined(_DEBUG)
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;	// 디버그 빌드에서는 디버그 레이어
#endif

	const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL selectedLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT hr = D3D11CreateDevice(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlags,
		featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
		pDevice_.GetAddressOf(),
		&selectedLevel,
		pContext_.GetAddressOf());

	if (FAILED(hr))
	{
		// 디버그 레이어가 설치되지 않은 PC라면 플래그 없이 재시도한다.
		hr = D3D11CreateDevice(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
			featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
			pDevice_.GetAddressOf(),
			&selectedLevel,
			pContext_.GetAddressOf());
	}

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////////////////////
// [v2.1] 지정 창용 스왕체인을 만들어준다. (Window::CreateSurface가 호출)
// [DXGI 팩토리 역추적이란?]
//  스왕체인은 "디바이스를 만든 팩토리"로 만들어야 궁합이 맞는다.
//  ID3D11Device -> IDXGIDevice -> IDXGIAdapter -> IDXGIFactory 순으로
//  부모를 타고 올라가면 그 팩토리를 얻을 수 있다.
bool GraphicDevice::CreateSwapChainForWindow(HWND _hWnd, _s32 _width, _s32 _height, IDXGISwapChain** _ppOutSwapChain)
{
	if (pDevice_ == nullptr || _ppOutSwapChain == nullptr)
	{
		return false;
	}

	// 1. 디바이스를 만든 DXGI 팩토리를 역추적한다.
	SgfComPtr<IDXGIDevice> pDxgiDevice;
	if (FAILED(pDevice_->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(pDxgiDevice.GetAddressOf()))))
	{
		return false;
	}

	SgfComPtr<IDXGIAdapter> pAdapter;
	if (FAILED(pDxgiDevice->GetAdapter(pAdapter.GetAddressOf())))
	{
		return false;
	}

	SgfComPtr<IDXGIFactory> pFactory;
	if (FAILED(pAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()))))
	{
		return false;
	}

	// 2. 스왕체인 설정 (구버전 경로와 동일한 구성)
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = 1;
	scd.BufferDesc.Width = _width;
	scd.BufferDesc.Height = _height;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = _hWnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	return SUCCEEDED(pFactory->CreateSwapChain(pDevice_.Get(), &scd, _ppOutSwapChain));
}

NS_SGF_END

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
// 프레임 시작: 그리기 대상을 백버퍼로 묶고 화면을 지운다.
// [v2.1] 지정 윈도우의 표면을 그리기 대상으로 묶고 지운다.
void GraphicDevice::BeginFrame(Window* _pWindow, const color& _clearColor)
{
	jc_assert(_pWindow != nullptr && _pWindow->HasSurface());

	// 현재 묶인 창을 기억해둔다. (SetRenderTarget(nullptr) 복귀용)
	pBoundWindow_ = _pWindow;
	width_ = _pWindow->Width();
	height_ = _pWindow->Height();

	SwapChain& swapChain = _pWindow->GetSwapChain();
	DepthStencilSurface& depthSurface = _pWindow->GetDepthSurface();

	// 그리기 대상을 "이 창의 백버퍼 + 깊이버퍼"로 묶는다.
	BindWindowSurface(_pWindow);

	// 배경색으로 지우고 깊이를 1.0(가장 멀리)으로 초기화한다.
	const _f32 clearColor[4] = { _clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a };
	pContext_->ClearRenderTargetView(swapChain.RTV(), clearColor);
	depthSurface.Clear(pContext_.Get());
}

//////////////////////////////////////////////////////////////////////////////////////////
// [v2.1] 지정 윈도우의 백버퍼를 화면에 표시한다.
void GraphicDevice::EndFrame(Window* _pWindow, bool _bVsync)
{
	jc_assert(_pWindow != nullptr && _pWindow->HasSurface());
	_pWindow->GetSwapChain().Present(_bVsync);

	// 이 창의 프레임이 끝났으므로 바인딩 기록을 해제한다.
	if (pBoundWindow_ == _pWindow)
	{
		pBoundWindow_ = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// [하위 호환] 단일 창 경로: 내장 백버퍼로 묶고 지운다.
void GraphicDevice::BeginFrame(const color& _clearColor)
{
	// 그리기 대상을 "백버퍼 + 기본 깊이버퍼"로 묶는다.
	// (중간에 렌더 타깃으로 바꿨다가 돌아오지 않은 경우를 대비해 매 프레임 재설정)
	BindBackBufferSurface();

	// 이전 프레임의 그림을 배경색으로 지운다.
	const _f32 clearColor[4] = { _clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a };
	pContext_->ClearRenderTargetView(swapChain_.RTV(), clearColor);

	// 깊이 버퍼는 "가장 멀다(1.0)"로 초기화한다.
	// 그래야 첫 픽셀이 무조건 깊이 테스트를 통과할 수 있다.
	depthSurface_.Clear(pContext_.Get());
}

//////////////////////////////////////////////////////////////////////////////////////////
// 프레임 끝: 백버퍼를 화면에 표시한다.
void GraphicDevice::EndFrame(bool _bVsync)
{
	swapChain_.Present(_bVsync);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 알파 블렌딩 켜기/끄기 (SetBlendMode의 간편 버전)
void GraphicDevice::SetAlphaBlending(bool _bEnable)
{
	SetBlendMode(_bEnable ? BlendMode::Alpha : BlendMode::Opaque);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 깊이 테스트 켜기/끄기
void GraphicDevice::SetDepthTest(bool _bEnable)
{
	pContext_->OMSetDepthStencilState(states_.GetDepthState(_bEnable), 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 블렌드 모드 설정 (상태 객체는 RenderStates가 캐싱)
void GraphicDevice::SetBlendMode(BlendMode _mode)
{
	pContext_->OMSetBlendState(states_.GetBlendState(_mode), nullptr, 0xFFFFFFFF);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 샘플러 설정
void GraphicDevice::SetSampler(SamplerFilter _filter, SamplerAddress _address, UINT _slot)
{
	ID3D11SamplerState* pSamplers[] = { states_.GetSamplerState(_filter, _address) };
	pContext_->PSSetSamplers(_slot, 1, pSamplers);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 와이어프레임 켜기/끄기
void GraphicDevice::SetWireframe(bool _bEnable)
{
	if (bWireframe_ != _bEnable)
	{
		bWireframe_ = _bEnable;
		ApplyRasterizerState();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 컬링 모드 설정
void GraphicDevice::SetCullMode(CullMode _mode)
{
	if (cullMode_ != _mode)
	{
		cullMode_ = _mode;
		ApplyRasterizerState();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 현재 와이어프레임/컬링 설정을 합쳤을 때의 래스터라이저 상태를 적용한다.
void GraphicDevice::ApplyRasterizerState()
{
	pContext_->RSSetState(states_.GetRasterizerState(bWireframe_, cullMode_));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 지정 창의 표면을 그리기 대상으로 묶고 뷰포트를 화면 크기로 채운다.
void GraphicDevice::BindWindowSurface(Window* _pWindow)
{
	SwapChain& swapChain = _pWindow->GetSwapChain();
	DepthStencilSurface& depthSurface = _pWindow->GetDepthSurface();
	ID3D11RenderTargetView* pTargets[] = { swapChain.RTV() };
	pContext_->OMSetRenderTargets(1, pTargets, depthSurface.DSV());
	swapChain.ApplyFullViewport(pContext_.Get());
}

//////////////////////////////////////////////////////////////////////////////////////////
// 내장 표면(백버퍼 RTV + 깊이 DSV)을 그리기 대상으로 묶고 뷰포트를 화면 크기로 채운다.
void GraphicDevice::BindBackBufferSurface()
{
	ID3D11RenderTargetView* pTargets[] = { swapChain_.RTV() };
	pContext_->OMSetRenderTargets(1, pTargets, depthSurface_.DSV());
	swapChain_.ApplyFullViewport(pContext_.Get());
}

//////////////////////////////////////////////////////////////////////////////////////////
// 그리기 대상 교체 (nullptr이면 백버퍼로 복귀)
void GraphicDevice::SetRenderTarget(RenderTarget* _pTarget)
{
	// 새 그리기 대상이 직전에 텍스처(t0/t1)로 읽히고 있었다면
	// "쓰기와 읽기 동시 바인딩" 충돌이 나므로 먼저 입력 바인딩을 해제한다.
	ID3D11ShaderResourceView* pNullSrvs[2] = { nullptr, nullptr };
	pContext_->PSSetShaderResources(0, 2, pNullSrvs);

	if (_pTarget == nullptr)
	{
		// v2.1: BeginFrame(Window*)로 그리던 중이면 "그 창의 표면"으로 복귀한다.
		if (pBoundWindow_ != nullptr)
		{
			BindWindowSurface(pBoundWindow_);
			return;
		}
		// 백버퍼 + 기본 깊이 버퍼로 복귀하고 뷰포트도 화면 크기로 되돌린다.
		BindBackBufferSurface();
		return;
	}

	// 렌더 타깃의 RTV/DSV를 바인딩한다. (깊이 전용이면 RTV는 nullptr = 색 기록 없음)
	ID3D11RenderTargetView* pTargets[] = { _pTarget->RTV() };
	pContext_->OMSetRenderTargets(1, pTargets, _pTarget->DSV());

	// 뷰포트를 렌더 타깃 크기에 맞춘다. (백버퍼와 크기가 다를 수 있다)
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(_pTarget->Width());
	viewport.Height = static_cast<float>(_pTarget->Height());
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	pContext_->RSSetViewports(1, &viewport);
}

NS_SGF_END
