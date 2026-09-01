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
#include "sgf/Graphics/VertexDeclaration.h"
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
// 모든 D3D 객체 해제. 부품별 Finalize를 순서대로 부른다. (C안: 자체 레지스트리 없음)
void GraphicDevice::Finalize()
{
	// 멱등 가드 — 이미 완전히 해제된 상태에서 재호출 시 아무 작업 없이 복귀
	if (!IsValid() && inputLayoutCache_.IsEmpty() && pDevice_ == nullptr)
	{
		return;
	}

	// 바인딩 해제 — ResourceMgr 리소스가 파이프라인에 묶인 채 삭제되지 않도록 선행
	if (context_.Raw() != nullptr)
	{
		context_.ClearState();
	}
	else
	{
		context_.InvalidateCache();
	}

	// 레이아웃 캐시 해제 — ClearState(바인딩 해제) 이후에 수행해야 라이브 오브젝트 경고가 없다
	inputLayoutCache_.Clear();

	// 외부에서 주입된 Registry는 외부에서 Finalize한다. Device는 소유하지 않으므로 여기서 Finalize하지 않는다.
	depthSurface_.Finalize();
	swapChain_.Finalize();
	context_.Finalize();
	states_.Finalize();
	pRegistry_ = nullptr;
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
bool GraphicDevice::EnsureRegistry() const
{
	if (pRegistry_ != nullptr) { return true; }
	jc_assert_msg(false, "ResourceMgr::Initialize 이후에만 리소스를 만들 수 있습니다. 외부에서 g_cResourceMgr.Initialize(&device)를 호출하세요.");
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////
_u64 GraphicDevice::CreateVertexShader(const jc::String& _hlslSource, const jc::String& _entry)
{
	if (!EnsureRegistry()) { return INVALID_RESOURCE_KEY; }
	VertexShader* pVs = dbg_new VertexShader();
	if (!pVs->InitializeFromSource(this, _hlslSource, _entry))
	{
		delete pVs;
		return INVALID_RESOURCE_KEY;
	}
	return pRegistry_->Register(pVs);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 GraphicDevice::CreatePixelShader(const jc::String& _hlslSource, const jc::String& _entry)
{
	if (!EnsureRegistry()) { return INVALID_RESOURCE_KEY; }
	PixelShader* pPs = dbg_new PixelShader();
	if (!pPs->InitializeFromSource(this, _hlslSource, _entry))
	{
		delete pPs;
		return INVALID_RESOURCE_KEY;
	}
	return pRegistry_->Register(pPs);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 GraphicDevice::CreateIndexBuffer(const _u32* _pIndices, _u32 _count, ResourceUsage _usage)
{
	if (!EnsureRegistry()) { return INVALID_RESOURCE_KEY; }
	IndexBuffer* pBuffer = dbg_new IndexBuffer();
	if (!pBuffer->Create(this, _pIndices, _count, _usage))
	{
		delete pBuffer;
		return INVALID_RESOURCE_KEY;
	}
	return pRegistry_->Register(pBuffer);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 GraphicDevice::CreateTexture(_s32 _width, _s32 _height, const _u8* _pPixels, PixelFormat _format)
{
	if (!EnsureRegistry()) { return INVALID_RESOURCE_KEY; }
	Texture* pTexture = dbg_new Texture();
	if (!pTexture->CreateFromMemory(this, _pPixels, _width, _height, _format))
	{
		delete pTexture;
		return INVALID_RESOURCE_KEY;
	}
	return pRegistry_->Register(pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 GraphicDevice::CreateVertexBufferInternal(const void* _pVertices, _u32 _count, ResourceUsage _usage, const VertexDeclaration* _pDecl)
{
	jc_assert(IsValid());
	if (!EnsureRegistry()) { return INVALID_RESOURCE_KEY; }
	VertexBuffer* pBuffer = dbg_new VertexBuffer();
	if (!pBuffer->Create(this, _pVertices, _count, _pDecl, _usage))
	{
		delete pBuffer;
		return INVALID_RESOURCE_KEY;
	}
	return pRegistry_->Register(pBuffer);
}

//////////////////////////////////////////////////////////////////////////////////////////
ID3D11InputLayout* GraphicDevice::GetOrCreateInputLayout(const VertexDeclaration* _pDecl, VertexShader* _pVs)
{
	if (_pDecl == nullptr || _pVs == nullptr || _pVs->Bytecode() == nullptr)
	{
		return nullptr;
	}

	const _u64 declHash = _pDecl->Hash();
	const _u64 sigHash = _pVs->InputSignatureHash();

	// 1. 캐시 조회 — 같은 (선언 × 시그니처) 조합은 앱 전체에서 1개만 존재한다
	for (_s32 i = 0; i < inputLayoutCache_.Size(); ++i)
	{
		InputLayoutEntry& entry = inputLayoutCache_[i];
		if (entry.declHash_ == declHash && entry.sigHash_ == sigHash)
		{
			return entry.pLayout_.Get();
		}
	}

#if defined(_DEBUG)
	// 2. 사전 검증 — VS가 소비하는 모든 시맨틱이 선언에 있어야 한다.
	for (_s32 i = 0; i < _pVs->InputSignature().Size(); ++i)
	{
		const VertexShader::SignatureElement& sig = _pVs->InputSignature()[i];
		jc_assert_msg(_pDecl->Contains(sig.semanticName_, sig.semanticIndex_),
			"VS가 요구하는 시맨틱이 VertexDeclaration에 없습니다: %s%u", sig.semanticName_, sig.semanticIndex_);
	}
#endif

	// 3. 생성 + 등록 (최초 조합에서만 도달)
	D3D11_INPUT_ELEMENT_DESC descs[MAX_VERTEX_ELEMENTS] = {};
	const _s32 descCount = BuildD3DElementDescs(*_pDecl, descs, MAX_VERTEX_ELEMENTS);
	if (descCount <= 0) { return nullptr; }

	ID3DBlob* pBytecode = _pVs->Bytecode();
	SgfComPtr<ID3D11InputLayout> pLayout;
	HRESULT hr = pDevice_->CreateInputLayout(
		descs, static_cast<UINT>(descCount),
		pBytecode->GetBufferPointer(), pBytecode->GetBufferSize(),
		pLayout.GetAddressOf());
	if (FAILED(hr))
	{
		_LogWarn_("[sgf] CreateInputLayout 실패 (declHash=%llx, sigHash=%llx)", declHash, sigHash);
		return nullptr;
	}

	SetDebugName(pLayout.Get(), "InputLayout(decl x vs)");

	InputLayoutEntry entry;
	entry.declHash_ = declHash;
	entry.sigHash_ = sigHash;
	entry.pLayout_ = pLayout;
	inputLayoutCache_.PushBack(entry);
	return inputLayoutCache_[inputLayoutCache_.Size() - 1].pLayout_.Get();
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
template <typename T>
T* GraphicDevice::ResolveTemplate(_u64 _key)
{
	if (pRegistry_ == nullptr || _key == INVALID_RESOURCE_KEY) { return nullptr; }
	if (GetResourceTypeFromKey(_key) != T::RESOURCE_TYPE) { return nullptr; }
	IResource* pRes = pRegistry_->Resolve(_key);
	if (pRes == nullptr || pRes->GetResourceType() != T::RESOURCE_TYPE) { return nullptr; }
	return static_cast<T*>(pRes);
}

////////////////////////////////////////////////////////////////////////////////////////
VertexShader* GraphicDevice::ResolveVertexShader(_u64 _key)
{
	return ResolveTemplate<VertexShader>(_key);
}

//////////////////////////////////////////////////////////////////////////////////////////
PixelShader* GraphicDevice::ResolvePixelShader(_u64 _key)
{
	return ResolveTemplate<PixelShader>(_key);
}

//////////////////////////////////////////////////////////////////////////////////////////
VertexBuffer* GraphicDevice::ResolveVertexBuffer(_u64 _key)
{
	return ResolveTemplate<VertexBuffer>(_key);
}

//////////////////////////////////////////////////////////////////////////////////////////
IndexBuffer* GraphicDevice::ResolveIndexBuffer(_u64 _key)
{
	return ResolveTemplate<IndexBuffer>(_key);
}

//////////////////////////////////////////////////////////////////////////////////////////
Texture* GraphicDevice::ResolveTexture(_u64 _key)
{
	return ResolveTemplate<Texture>(_key);
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
