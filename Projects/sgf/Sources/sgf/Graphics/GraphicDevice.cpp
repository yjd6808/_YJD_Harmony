/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:40:00 PM
 * 수정일: 8/9/2026 1:00:00 AM (파사드로 재구성)
 * =====================
 * D3D11 그래픽 디바이스 (파사드) 구현부
 */

#include "Core.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/Buffers.h"
#include "sgf/Graphics/Texture.h"
#include "sgf/Graphics/ShaderProgram.h"
#include "sgf/Graphics/RenderTarget.h"
#include "sgf/Core/Window.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
GraphicDevice::GraphicDevice()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
GraphicDevice::~GraphicDevice()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
GraphicDevice& GraphicDevice::Get()
{
	static GraphicDevice s_instance;
	return s_instance;
}

//////////////////////////////////////////////////////////////////////////////////////////
// D3D11 초기화 전체 진행
// [순서가 중요하다]
// 1. 디바이스+스왑체인 -> 2. 깊이버퍼 -> 3. 상태 캐시 -> 4. 뷰포트/기본상태.
// 앞 단계 산출물이 뒤 단계 입력이 된다.
// 디바이스/컨텍스트와 상태 캐시만 초기화한다. (표면 없음)
// 멀티 윈도우 경로: 이후 각 Window::CreateSurface가 자기 표면을 만든다.
bool GraphicDevice::Initialize()
{
	// 1. 디바이스/컨텍스트 생성 - CreateDeviceOnly가 context_.InitializeImmediate까지 수행한다
	if (!CreateDeviceOnly()) { return false; }

	// 2. 상태 객체 캐시 준비
	if (!states_.Initialize(pDevice_.Get())) { return false; }

	// 3. 기본 상태 적용 (샘플러 슬롯0, 솔리드+백컬링)
	ID3D11SamplerState* pSamplers[] = {
		states_.GetSamplerState(FilterMode::fmLinear, AddressMode::amClamp, AddressMode::amClamp)
	};
	context_.SetSamplerRaw(ShaderStage::ssPixel, 0, pSamplers[0]);
	context_.SetRasterizer(CullMode::cmBack, FillMode::fmSolid, FrontFace::ffClockwise);
	_LogInfo_("[sgf] GraphicDevice::Initialize OK — 디바이스/컨텍스트/상태 준비");
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 모든 D3D 객체 해제. 부품별 Finalize를 순서대로 부른다.
void GraphicDevice::Finalize()
{
	// 멱등 가드 — 이미 완전히 해제된 상태에서 재호출 시 아무 작업 없이 복귀
	if (!IsValid() && textures_.IsEmpty() && indexBuffers_.IsEmpty() && vertexBuffers_.IsEmpty() && vertexShaders_.IsEmpty() && pixelShaders_.IsEmpty())
	{
		return;
	}

	// 바인딩 해제 — 레지스트리 리소스가 파이프라인에 묶인 채 삭제되지 않도록 선행
	if (context_.Raw() != nullptr)
	{
		context_.ClearState();
	}
	else
	{
		context_.InvalidateCache();
	}

	// 레지스트리 역순 해제 — textures → indexBuffers → vertexBuffers → shaders
	for (_s32 i = 0; i < textures_.Size(); ++i)
	{
		delete textures_[i];
	}
	textures_.Clear();

	for (_s32 i = 0; i < indexBuffers_.Size(); ++i)
	{
		delete indexBuffers_[i];
	}
	indexBuffers_.Clear();

	for (_s32 i = 0; i < vertexBuffers_.Size(); ++i)
	{
		delete vertexBuffers_[i].pBuffer;
	}
	vertexBuffers_.Clear();

	for (_s32 i = 0; i < vertexShaders_.Size(); ++i)
	{
		delete vertexShaders_[i];
	}
	vertexShaders_.Clear();

	for (_s32 i = 0; i < pixelShaders_.Size(); ++i)
	{
		delete pixelShaders_[i];
	}
	pixelShaders_.Clear();

	depthSurface_.Finalize();
	swapChain_.Finalize();
	context_.Finalize();
	states_.Finalize();
	pDevice_.Reset();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 디바이스만 생성한다. (스왑체인 없음 - 표면은 각 Window가 만든다)
bool GraphicDevice::CreateDeviceOnly()
{
	UINT createFlags = 0;
#if defined(_DEBUG)
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;	// 디버그 빌드에서는 디버그 레이어
#endif

	const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL selectedLevel = D3D_FEATURE_LEVEL_11_0;

	SgfComPtr<ID3D11DeviceContext> pImmediate;
	HRESULT hr = D3D11CreateDevice(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlags,
		featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
		pDevice_.GetAddressOf(),
		&selectedLevel,
		pImmediate.GetAddressOf());

	if (FAILED(hr))
	{
		// 디버그 레이어가 설치되지 않은 PC라면 플래그 없이 재시도한다.
		hr = D3D11CreateDevice(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
			featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
			pDevice_.GetAddressOf(),
			&selectedLevel,
			pImmediate.GetAddressOf());
	}

	if (FAILED(hr)) { return false; }

	if (!context_.InitializeImmediate(this, pImmediate.Get())) { return false; }
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GraphicDevice::CreateSwapChainForWindow(HWND _hWnd, _s32 _width, _s32 _height, PixelFormat _format, IDXGISwapChain** _ppOutSwapChain)
{
	if (pDevice_ == nullptr || _ppOutSwapChain == nullptr)
	{
		return false;
	}

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

	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = 1;
	scd.BufferDesc.Width = _width;
	scd.BufferDesc.Height = _height;
	scd.BufferDesc.Format = ToD3D11(_format);
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = _hWnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT hr = pFactory->CreateSwapChain(pDevice_.Get(), &scd, _ppOutSwapChain);
	if (SUCCEEDED(hr))
	{
		pFactory->MakeWindowAssociation(_hWnd, DXGI_MWA_NO_ALT_ENTER);
	}
	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GraphicDevice::CreateSwapChain(HWND _hWnd, _s32 _width, _s32 _height, PixelFormat _format)
{
	IDXGISwapChain* pRaw = nullptr;
	if (!CreateSwapChainForWindow(_hWnd, _width, _height, _format, &pRaw))
	{
		return false;
	}
	if (!swapChain_.Initialize(pRaw, pDevice_.Get(), _width, _height))
	{
		if (pRaw != nullptr)
		{
			pRaw->Release();
		}
		return false;
	}
	if (!depthSurface_.Initialize(pDevice_.Get(), _width, _height))
	{
		swapChain_.Finalize();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 GraphicDevice::CreateVertexShader(const jc::String& _hlslSource, const jc::String& _entry)
{
	VertexShader* pVs = new VertexShader();
	if (!pVs->InitializeFromSource(this, _hlslSource, _entry))
	{
		delete pVs;
		return INVALID_HANDLE;
	}
	vertexShaders_.PushBack(pVs);
	const _u32 handle = static_cast<_u32>(vertexShaders_.Size()) - 1;
	context_.SetVertexShader(handle);
	return handle;
}

_u32 GraphicDevice::CreatePixelShader(const jc::String& _hlslSource, const jc::String& _entry)
{
	PixelShader* pPs = new PixelShader();
	if (!pPs->InitializeFromSource(this, _hlslSource, _entry))
	{
		delete pPs;
		return INVALID_HANDLE;
	}
	pixelShaders_.PushBack(pPs);
	const _u32 handle = static_cast<_u32>(pixelShaders_.Size()) - 1;
	context_.SetPixelShader(handle);
	return handle;
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 GraphicDevice::CreateIndexBuffer(const _u32* _pIndices, _u32 _count, ResourceUsage _usage)
{
	IndexBuffer* pBuffer = new IndexBuffer();
	if (!pBuffer->Create(this, _pIndices, _count, _usage))
	{
		delete pBuffer;
		return INVALID_HANDLE;
	}
	indexBuffers_.PushBack(pBuffer);
	const _u32 handle = static_cast<_u32>(indexBuffers_.Size()) - 1;
	context_.SetIndexBuffer(handle);
	return handle;
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 GraphicDevice::CreateTexture(_s32 _width, _s32 _height, const _u8* _pPixels, PixelFormat _format)
{
	Texture* pTexture = new Texture();
	if (!pTexture->CreateFromMemory(this, _pPixels, _width, _height, _format))
	{
		delete pTexture;
		return INVALID_HANDLE;
	}
	textures_.PushBack(pTexture);
	const _u32 handle = static_cast<_u32>(textures_.Size()) - 1;
	context_.SetTexture(0, handle);
	return handle;
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 GraphicDevice::CreateVertexBufferInternal(const void* _pVertices, UINT _stride, _u32 _count, ResourceUsage _usage, VertexLayoutSpan _layout)
{
	jc_assert(IsValid());
	VertexBuffer* pBuffer = new VertexBuffer();
	if (!pBuffer->Create(this, _pVertices, _stride, _count, _layout, _usage))
	{
		delete pBuffer;
		return INVALID_HANDLE;
	}
	vertexBuffers_.PushBack({ pBuffer });
	const _u32 handle = static_cast<_u32>(vertexBuffers_.Size()) - 1;
	context_.SetVertexBuffer(handle);
	return handle;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicDevice::BeginFrame(SwapChain& _swapChain, DepthStencilSurface* _pDepth, const color& _clearColor)
{
	ID3D11RenderTargetView* pRtv = _swapChain.RTV();
	ID3D11DepthStencilView* pDsv = (_pDepth != nullptr) ? _pDepth->DSV() : nullptr;
	context_.SetRenderTargets(&pRtv, 1, pDsv);
	context_.ClearRenderTarget(pRtv, _clearColor);
	if (_pDepth != nullptr)
	{
		_pDepth->Clear(context_);
	}
	_swapChain.ApplyFullViewport(context_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicDevice::BeginFrame(const color& _clearColor)
{
	BeginFrame(swapChain_, &depthSurface_, _clearColor);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicDevice::Present(bool _vsync)
{
	swapChain_.Present(_vsync);
}

//////////////////////////////////////////////////////////////////////////////////////////
VertexShader* GraphicDevice::ResolveVertexShader(_u32 _handle)
{
	if (_handle < static_cast<_u32>(vertexShaders_.Size()))
	{
		return vertexShaders_[_handle];
	}
	return nullptr;
}

PixelShader* GraphicDevice::ResolvePixelShader(_u32 _handle)
{
	if (_handle < static_cast<_u32>(pixelShaders_.Size()))
	{
		return pixelShaders_[_handle];
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
GraphicDevice::VertexBufferSlot* GraphicDevice::ResolveVertexBuffer(_u32 _handle)
{
	if (_handle < static_cast<_u32>(vertexBuffers_.Size()))
	{
		return &vertexBuffers_[_handle];
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
IndexBuffer* GraphicDevice::ResolveIndexBuffer(_u32 _handle)
{
	if (_handle < static_cast<_u32>(indexBuffers_.Size()))
	{
		return indexBuffers_[_handle];
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
Texture* GraphicDevice::ResolveTexture(_u32 _handle)
{
	if (_handle < static_cast<_u32>(textures_.Size()))
	{
		return textures_[_handle];
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicDevice::SetRenderTarget(RenderTarget* _pTarget)
{
	// 새 그리기 대상이 직전에 텍스처(t0/t1)로 읽히고 있었다면
	// "쓰기와 읽기 동시 바인딩" 충돌이 나므로 먼저 입력 바인딩을 해제한다.
	context_.SetTexture(ShaderStage::ssPixel, 0, nullptr);
	context_.SetTexture(ShaderStage::ssPixel, 1, nullptr);

	if (_pTarget == nullptr)
	{
		// 백버퍼 + 기본 깊이 버퍼로 복귀하고 뷰포트도 화면 크기로 되돌린다.
		ID3D11RenderTargetView* pTargets[] = { swapChain_.RTV() };
		context_.SetRenderTargets(pTargets, 1, depthSurface_.DSV());
		swapChain_.ApplyFullViewport(context_);
		return;
	}

	// 렌더 타깃의 RTV/DSV를 바인딩한다. (깊이 전용이면 RTV는 nullptr = 색 기록 없음)
	ID3D11RenderTargetView* pTargets[] = { _pTarget->RTV() };
	context_.SetRenderTargets(pTargets, 1, _pTarget->DSV());

	// 뷰포트를 렌더 타깃 크기에 맞춘다. (백버퍼와 크기가 다를 수 있다)
	Viewport viewport(0.0f, 0.0f, static_cast<_f32>(_pTarget->Width()), static_cast<_f32>(_pTarget->Height()));
	context_.SetViewport(viewport);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicDevice::SetDebugName(ID3D11DeviceChild* _pChild, const jc::String& _name)
{
#if defined(_DEBUG)
	if (_pChild == nullptr || _name.IsEmpty())
	{
		return;
	}
	_pChild->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(_name.Length()), _name.Source());
#else
	(void)_pChild;
	(void)_name;
#endif
}

NS_SGF_END
